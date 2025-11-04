#include "Server.hpp"
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

        Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response)); // envoie la réponse de connexion au client

        if (lobby->isFull()) { // si le lobby est plein, on lance la partie
            lobby->gameStarted = true;
            GameStartPacket gameStartPacket;
            gameStartPacket.lobbyId = packet->lobbyId;

            lobby->broadcast(PacketType::GAME_START, &gameStartPacket, sizeof(GameStartPacket)); // envoie le paquet de début de partie à tous les clients du lobby

            std::cout << "Lobby " << packet->lobbyId << " is full, starting game!" << std::endl; // affiche un message dans la console

            int lobbyId = packet->lobbyId;
            
            std::thread gameEndThread([lobbyId]() { // crée un thread pour la fin de la partie
                std::this_thread::sleep_for(std::chrono::seconds(5)); // attends 5sec pour finir la game (seulement pour debug pour le moment à retirer plus tard)

                if (g_server) { // si le serveur existe, on récupère le lobby
                    Lobby* lobby = g_server->getLobbyManager().findLobbyById(lobbyId); // récupère le lobby par son id
                    if (lobby) { // si le lobby existe, on envoie le paquet de fin de partie à tous les clients du lobby
                        GameEndPacket gameEndPacket;
                        gameEndPacket.lobbyId = lobbyId;
                        lobby->broadcast(PacketType::GAME_END, &gameEndPacket, sizeof(GameEndPacket)); // envoie le paquet de fin de partie à tous les clients du lobby
                    }
                    g_server->clearLobbyAndRemovePlayers(lobbyId); // vide le lobby et retire les joueurs du lobby
                }
                
                std::cout << "Game ended in lobby " << lobbyId << std::endl;
            });
            gameEndThread.detach();
        }
    } else {
        response.accepted = false;
        strncpy(response.reason, "Error adding to lobby", sizeof(response.reason) - 1);
        Packet::sendPacket(player->connection, PacketType::CONNECT_RESPONSE, &response, sizeof(response)); // envoie la réponse de connexion au client
    }
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

