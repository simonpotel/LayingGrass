#include "Client.hpp"
#include "Render.hpp"
#include "Packet.hpp"
#include "GameState.hpp"
#include <iostream>
#include <memory>
#include <mutex>

Client* g_client = nullptr;
std::shared_ptr<GameState> g_gameState;
std::mutex g_gameStateMutex;

void handleLobbyList(const void* data, size_t size) {
    const LobbyListPacket* packet = (const LobbyListPacket*)data;

    std::lock_guard<std::mutex> lock(g_gameStateMutex);
    if (g_gameState) {
        g_gameState->updateLobbies(*packet);
    }
}

void handleConnectResponse(const void* data, size_t size) {
    const ConnectResponsePacket* packet = (const ConnectResponsePacket*)data;

    std::lock_guard<std::mutex> lock(g_gameStateMutex);
    if (packet->accepted) {
        std::cout << "Connection accepted to lobby " << packet->lobbyId << std::endl;
        if (g_gameState) {
            g_gameState->setCurrentLobby(packet->lobbyId);
            g_gameState->setState(ClientState::IN_LOBBY);
        }
    } else {
        std::cout << "Connection refused: " << packet->reason << std::endl;
        if (g_gameState) {
            g_gameState->setState(ClientState::SELECTING_LOBBY);
            g_gameState->setRequestSent(false);
        }
    }
}

void handleGameStart(const void* data, size_t size) {
    const GameStartPacket* packet = (const GameStartPacket*)data;

    std::cout << "Game started in lobby " << packet->lobbyId << std::endl;
    std::lock_guard<std::mutex> lock(g_gameStateMutex);
    if (g_gameState) {
        g_gameState->setState(ClientState::IN_GAME);
    }
}

void handleGameEnd(const void* data, size_t size) {
    const GameEndPacket* packet = (const GameEndPacket*)data;

    std::cout << "Game ended in lobby " << packet->lobbyId << std::endl;
    
    int winnerId = packet->winnerId;
    int lobbyId = packet->lobbyId;
    std::string winnerName = packet->winnerName;
    
    auto newGameState = std::make_shared<GameState>();
    newGameState->setGameEndWinnerId(winnerId);
    newGameState->setGameEndLobbyId(lobbyId);
    newGameState->setGameEndWinnerName(winnerName);
    newGameState->setState(ClientState::GAME_END);
    
    std::lock_guard<std::mutex> lock(g_gameStateMutex);
    g_gameState = newGameState;
}

void handleBoardUpdate(const void* data, size_t size) {
    const BoardUpdatePacket* packet = (const BoardUpdatePacket*)data;
    
    std::lock_guard<std::mutex> lock(g_gameStateMutex);
    if (g_gameState) {
        g_gameState->updateBoard(*packet);
    }
}

int main() {
    Client client;
    auto gameState = std::make_shared<GameState>();

    g_client = &client;
    g_gameState = gameState;

    if (!client.connect("127.0.0.1", 5555)) {
        std::cerr << "Error: Cannot connect to server" << std::endl;
        return 1;
    }

    std::cout << "Connected to server" << std::endl;

    client.getCallbackManager().registerCallback(PacketType::LOBBY_LIST, handleLobbyList);
    client.getCallbackManager().registerCallback(PacketType::CONNECT_RESPONSE, handleConnectResponse);
    client.getCallbackManager().registerCallback(PacketType::GAME_START, handleGameStart);
    client.getCallbackManager().registerCallback(PacketType::GAME_END, handleGameEnd);
    client.getCallbackManager().registerCallback(PacketType::BOARD_UPDATE, handleBoardUpdate);

    client.startReceiving();

    auto window = Render::createWindow();
    if (!window || !window->isOpen()) {
        std::cerr << "Error: Cannot create window" << std::endl;
        return 1;
    }

    while (window->isOpen()) {
        std::shared_ptr<GameState> currentGameState;
        {
            std::lock_guard<std::mutex> lock(g_gameStateMutex);
            currentGameState = g_gameState;
        }
        
        if (!currentGameState) {
            continue;
        }
        
        if (Render::handleInput(*window, *currentGameState)) {
            break;
        }

        if (currentGameState->getState() == ClientState::WAITING_FOR_RESPONSE && !currentGameState->isRequestSent()) {
            client.sendConnectRequest(currentGameState->getUsername().c_str(), currentGameState->getSelectedLobby(), currentGameState->getSelectedColor());
            {
                std::lock_guard<std::mutex> lock(g_gameStateMutex);
                if (g_gameState == currentGameState) {
                    g_gameState->setRequestSent(true);
                }
            }
        }

        Render::render(*window, *currentGameState);
    }
}
