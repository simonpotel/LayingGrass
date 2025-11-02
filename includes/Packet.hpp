#ifndef PACKET_HPP
#define PACKET_HPP

#include <cstring>
#include <cstddef>

// enumération des types de paquets
enum class PacketType {
    CONNECT_REQUEST, // demande de connexion au serveur avec nom et lobby
    CONNECT_RESPONSE, // réponse du serveur à la demande de connexion
    LOBBY_LIST, // liste des lobbies disponibles
    GAME_START, // début de la partie
    GAME_END // fin de la partie
};

#include "PacketCallback.hpp"

struct PacketHeader { // structure pour le paquet de header
    PacketType type; // type du paquet
    int size; // taille du paquet
};

struct ConnectRequestPacket {
    char playerName[256]; // nom du joueur
    int lobbyId; // identifiant du lobby choisi
};

struct ConnectResponsePacket {
    bool accepted; // true si la connexion est acceptée
    char reason[256]; // raison du refus si accepted = false
    int lobbyId; // identifiant du lobby si accepté
};

struct LobbyInfo { // structure pour les informations d'un lobby
    int lobbyId; // identifiant du lobby
    int playerCount; // nombre de joueurs connectés au lobby
    bool gameStarted; // true si la partie est lancée, false sinon
};

struct LobbyListPacket { // structure pour la liste des lobbies
    int lobbyCount; // nombre de lobbies dans la liste
    LobbyInfo lobbies[100]; // tableau des lobbies (max 100 lobbies)
};

struct GameStartPacket {
    int lobbyId; // identifiant du lobby
};

struct GameEndPacket {
    int lobbyId; // identifiant du lobby
};

namespace Packet {
    bool sendPacket(int socketFd, PacketType type, const void* data, size_t size); // envoie un paquet sur le socket
    bool receivePacket(int socket, PacketHeader& header, void*& data); // reçoit un paquet et le décode
}

#endif

