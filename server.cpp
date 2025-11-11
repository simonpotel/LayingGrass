#include "Server.hpp"
#include "Game/Game.hpp"
#include "Packet.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <vector>

// variable globale pour accéder au serveur dans les callbacks
Server* g_server = nullptr;

void handleConnectRequest(Player* player, const void* data, size_t size) {
    const ConnectRequestPacket* packet = (const ConnectRequestPacket*)data;
    ConnectResponsePacket response;
    memset(&response, 0, sizeof(response));

    // vérifie que le lobby existe
    Lobby* lobby = g_server->getLobbyManager().findLobbyById(packet->lobbyId);
    if (!lobby) {
        response.accepted = false;
        strncpy(response.reason, "Lobby not found", sizeof(response.reason) - 1);
        Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
        return;
    }

    // vérifie que le lobby n'est pas plein
    if (lobby->isFull()) {
        response.accepted = false;
        strncpy(response.reason, "Lobby is full", sizeof(response.reason) - 1);
        Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
        return;
    }

    // vérifie que le nom n'est pas déjà utilisé dans ce lobby
    if (lobby->hasPlayerName(packet->playerName)) {
        response.accepted = false;
        strncpy(response.reason, "Name already used in this lobby", sizeof(response.reason) - 1);
        Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
        return;
    }

    // vérifie que la couleur n'est pas déjà prise dans ce lobby
    if (lobby->hasColor(packet->colorId)) {
        response.accepted = false;
        strncpy(response.reason, "Color already taken in this lobby", sizeof(response.reason) - 1);
        Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
        return;
    }

    // vérifie que la couleur est valide (0-8)
    if (packet->colorId < 0 || packet->colorId >= 9) {
        response.accepted = false;
        strncpy(response.reason, "Invalid color", sizeof(response.reason) - 1);
        Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
        return;
    }

    // ajoute le joueur au lobby
    if (lobby->addConnection(player->connection, packet->playerName, packet->colorId)) {
        strncpy(player->playerName, packet->playerName, sizeof(player->playerName) - 1);
        player->playerName[sizeof(player->playerName) - 1] = '\0';
        player->lobbyId = packet->lobbyId;

        response.accepted = true;
        response.lobbyId = packet->lobbyId;
        strncpy(response.reason, "Connection accepted", sizeof(response.reason) - 1);

        std::cout << "Player " << player->playerName << " connected to lobby " << packet->lobbyId << std::endl;

        Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
    } else {
        response.accepted = false;
        strncpy(response.reason, "Error adding to lobby", sizeof(response.reason) - 1);
        Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response)); // envoie la réponse de connexion au client
    }
}

void handleCellClick(Player* player, const void* data, size_t size) {
    const CellClickPacket* packet = (const CellClickPacket*)data;
    
    if (!g_server) {
        return;
    }
    
    Lobby* lobby = g_server->getLobbyManager().findLobbyById(packet->lobbyId);
    if (!lobby) {
        return;
    }
    
    Game* game = lobby->getGame();
    if (!game) {
        return;
    }
    
    // vérifie si le joueur a un bonus en attente (doit être utilisé immédiatement)
    if (game->hasPendingStoneBonus(player->connection) || game->hasPendingRobberyBonus(player->connection)) {
        return; // le joueur doit d'abord utiliser son bonus
    }
    
    game->handleCellClick(player->connection, packet->row, packet->col, 
        packet->rotation, packet->flippedH, packet->flippedV, packet->useCoupon, packet->couponChoice);
}

void handlePlaceStone(Player* player, const void* data, size_t size) {
    const PlaceStonePacket* packet = (const PlaceStonePacket*)data;
    
    if (!g_server) {
        return;
    }
    
    Lobby* lobby = g_server->getLobbyManager().findLobbyById(packet->lobbyId);
    if (!lobby) {
        return;
    }
    
    Game* game = lobby->getGame();
    if (!game) {
        return;
    }
    
    game->placeStone(player->connection, packet->row, packet->col);
}

void handleRobTile(Player* player, const void* data, size_t size) {
    const RobTilePacket* packet = (const RobTilePacket*)data;
    
    if (!g_server) {
        return;
    }
    
    Lobby* lobby = g_server->getLobbyManager().findLobbyById(packet->lobbyId);
    if (!lobby) {
        return;
    }
    
    Game* game = lobby->getGame();
    if (!game) {
        return;
    }
    
    game->robTile(player->connection, packet->targetPlayerColorId);
}

void handleDiscardTile(Player* player, const void* data, size_t size) {
    const DiscardTilePacket* packet = (const DiscardTilePacket*)data;
    
    if (!g_server) {
        return;
    }
    
    Lobby* lobby = g_server->getLobbyManager().findLobbyById(packet->lobbyId);
    if (!lobby) {
        return;
    }
    
    Game* game = lobby->getGame();
    if (!game) {
        return;
    }
    
    game->discardTile(player->connection);
}

void handleStartGameRequest(Player* player, const void* data, size_t size) {
    const StartGameRequestPacket* packet = (const StartGameRequestPacket*)data;
    
    if (!g_server) {
        return;
    }
    
    Lobby* lobby = g_server->getLobbyManager().findLobbyById(packet->lobbyId);
    if (!lobby) {
        return;
    }
    
    // vérifie que le lobby n'est pas déjà lancé
    if (lobby->gameStarted) {
        return;
    }
    
    // vérifie qu'il y a au moins 2 joueurs
    if (lobby->getPlayerCount() < 2) {
        return;
    }
    
    // lance la partie
    GameStartPacket gameStartPacket;
    gameStartPacket.lobbyId = packet->lobbyId;
    
    lobby->broadcast(PacketType::GAME_START, &gameStartPacket, sizeof(GameStartPacket));
    
    std::cout << "Lobby " << packet->lobbyId << " game started by player!" << std::endl;
    
    lobby->startGame();
}

void handleTilePreview(Player* player, const void* data, size_t size) {
    const TilePreviewPacket* packet = (const TilePreviewPacket*)data;
    
    if (!g_server) {
        return;
    }
    
    Lobby* lobby = g_server->getLobbyManager().findLobbyById(packet->lobbyId);
    if (!lobby) {
        return;
    }
    
    // vérifie que le lobby a une partie en cours
    if (!lobby->gameStarted || !lobby->getGame()) {
        return;
    }
    
    // broadcast la prévisualisation à tous les joueurs du lobby (sauf l'expéditeur)
    // on envoie à tous pour que tout le monde voie la prévisualisation
    lobby->broadcast(PacketType::TILE_PREVIEW, packet, sizeof(TilePreviewPacket));
}

int main(int argc, char* argv[]) {
    int port = 5555;
    
    if (argc >= 2) {
        port = std::atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            std::cerr << "Error: Invalid port number. Must be between 1 and 65535." << std::endl;
            std::cerr << "Usage: " << argv[0] << " [port]" << std::endl;
            return 1;
        }
    }
    
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [port]" << std::endl;
        std::cerr << "Default port: 5555" << std::endl;
        return 1;
    }
    
    Server server(port);
    g_server = &server; // stocke le pointeur global pour le callback

    server.getCallbackManager().registerCallback(PacketType::CONNECT_REQUEST, handleConnectRequest);
    server.getCallbackManager().registerCallback(PacketType::CELL_CLICK, handleCellClick);
    server.getCallbackManager().registerCallback(PacketType::START_GAME_REQUEST, handleStartGameRequest);
    server.getCallbackManager().registerCallback(PacketType::TILE_PREVIEW, handleTilePreview);
    server.getCallbackManager().registerCallback(PacketType::PLACE_STONE, handlePlaceStone);
    server.getCallbackManager().registerCallback(PacketType::ROB_TILE, handleRobTile);
    server.getCallbackManager().registerCallback(PacketType::DISCARD_TILE, handleDiscardTile);
    server.start();

    std::cout << "Server started on port " << port << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

