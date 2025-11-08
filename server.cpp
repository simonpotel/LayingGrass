#include "Server.hpp"
#include "Game/Game.hpp"
#include "Packet.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
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

        if (lobby->isFull()) {
            GameStartPacket gameStartPacket;
            gameStartPacket.lobbyId = packet->lobbyId;

            lobby->broadcast(PacketType::GAME_START, &gameStartPacket, sizeof(GameStartPacket));

            std::cout << "Lobby " << packet->lobbyId << " is full, starting game!" << std::endl;

            lobby->startGame();
        }
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
    
    game->handleCellClick(player->connection, packet->row, packet->col, 
        packet->rotation, packet->flippedH, packet->flippedV, packet->useCoupon);
}

int main() {
    Server server(5555);
    g_server = &server; // stocke le pointeur global pour le callback

    server.getCallbackManager().registerCallback(PacketType::CONNECT_REQUEST, handleConnectRequest);
    server.getCallbackManager().registerCallback(PacketType::CELL_CLICK, handleCellClick);
    server.start();

    std::cout << "Server started on port 5555" << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

