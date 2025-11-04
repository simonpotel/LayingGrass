#include "Client.hpp"
#include "Render.hpp"
#include "Packet.hpp"
#include "GameState.hpp"
#include <iostream>

// variables globales pour accéder au client et gamestate dans les callbacks
Client* g_client = nullptr;
GameState* g_gameState = nullptr;

void handleLobbyList(const void* data, size_t size) {
    const LobbyListPacket* packet = (const LobbyListPacket*)data;

    // met à jour la liste des lobbies dans le gamestate
    if (g_gameState) {
        g_gameState->updateLobbies(*packet);
    }
}

void handleConnectResponse(const void* data, size_t size) {
    const ConnectResponsePacket* packet = (const ConnectResponsePacket*)data;

    if (packet->accepted) {
        std::cout << "Connection accepted to lobby " << packet->lobbyId << std::endl;
        // met à jour l'état du jeu
        if (g_gameState) {
            g_gameState->setCurrentLobby(packet->lobbyId);
            g_gameState->setState(ClientState::IN_LOBBY);
        }
    } else {
        std::cout << "Connection refused: " << packet->reason << std::endl;
        // retour à la sélection du lobby
        if (g_gameState) {
            g_gameState->setState(ClientState::SELECTING_LOBBY);
            g_gameState->setRequestSent(false); // réinitialise le flag pour permettre une nouvelle tentative
        }
    }
}

void handleGameStart(const void* data, size_t size) {
    const GameStartPacket* packet = (const GameStartPacket*)data;

    std::cout << "Game started in lobby " << packet->lobbyId << std::endl;
    if (g_gameState) {
        g_gameState->setState(ClientState::IN_GAME);
    }
}

void handleGameEnd(const void* data, size_t size) {
    const GameEndPacket* packet = (const GameEndPacket*)data;

    std::cout << "Game ended in lobby " << packet->lobbyId << std::endl;
    if (g_gameState) {
        int winnerId = packet->winnerId;
        int lobbyId = packet->lobbyId;
        std::string winnerName = packet->winnerName;
        
        g_gameState->~GameState();
        new(g_gameState) GameState();
        
        g_gameState->setGameEndWinnerId(winnerId);
        g_gameState->setGameEndLobbyId(lobbyId);
        g_gameState->setGameEndWinnerName(winnerName);
        g_gameState->setState(ClientState::GAME_END);
    }
}

void handleBoardUpdate(const void* data, size_t size) {
    const BoardUpdatePacket* packet = (const BoardUpdatePacket*)data;
    
    if (g_gameState) {
        g_gameState->updateBoard(*packet);
    }
}

int main() {
    Client client;
    GameState gameState;

    g_client = &client;
    g_gameState = &gameState;

    // connexion au serveur
    if (!client.connect("127.0.0.1", 5555)) {
        std::cerr << "Error: Cannot connect to server" << std::endl;
        return 1;
    }

    std::cout << "Connected to server" << std::endl;

    // enregistrement des callbacks
    client.getCallbackManager().registerCallback(PacketType::LOBBY_LIST, handleLobbyList);
    client.getCallbackManager().registerCallback(PacketType::CONNECT_RESPONSE, handleConnectResponse);
    client.getCallbackManager().registerCallback(PacketType::GAME_START, handleGameStart);
    client.getCallbackManager().registerCallback(PacketType::GAME_END, handleGameEnd);
    client.getCallbackManager().registerCallback(PacketType::BOARD_UPDATE, handleBoardUpdate);

    // démarre la réception des paquets
    client.startReceiving();

    // crée la fenêtre de rendu
    auto window = Render::createWindow();
    if (!window || !window->isOpen()) {
        std::cerr << "Error: Cannot create window" << std::endl;
        return 1;
    }

    // boucle principale du jeu
    while (window->isOpen()) {
        // gère les inputs utilisateur
        if (Render::handleInput(*window, gameState)) {
            break; // demande de quitter
        }

        // si l'utilisateur a validé son nom, envoie la demande de connexion une seule fois
        if (gameState.getState() == ClientState::WAITING_FOR_RESPONSE && !gameState.isRequestSent()) {
            client.sendConnectRequest(gameState.getUsername().c_str(), gameState.getSelectedLobby(), gameState.getSelectedColor());
            gameState.setRequestSent(true); // marque la requête comme envoyée
        }

        // rendu
        Render::render(*window, gameState);
    }
}
