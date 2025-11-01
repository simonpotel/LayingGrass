#include "Server.hpp"
#include "Packet.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>

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

    // ajoute le joueur au lobby
    if (lobby->addConnection(player->connection, packet->playerName)) {
        strncpy(player->playerName, packet->playerName, sizeof(player->playerName) - 1);
        player->playerName[sizeof(player->playerName) - 1] = '\0';
        player->lobbyId = packet->lobbyId;

        response.accepted = true;
        response.lobbyId = packet->lobbyId;
        strncpy(response.reason, "Connection accepted", sizeof(response.reason) - 1);

        std::cout << "Player " << player->playerName << " connected to lobby " << packet->lobbyId << std::endl;
    } else {
        response.accepted = false;
        strncpy(response.reason, "Error adding to lobby", sizeof(response.reason) - 1);
    }

    Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response));
}

int main() {
    Server server(5555);
    g_server = &server; // stocke le pointeur global pour le callback

    server.getCallbackManager().registerCallback(PacketType::CONNECT_REQUEST, handleConnectRequest);
    server.start();

    std::cout << "Server started on port 5555" << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

