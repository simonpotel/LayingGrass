#ifndef PACKET_HPP
#define PACKET_HPP

#include <cstring>
#include <cstddef>

// enumération des types de paquets 
enum class PacketType {
    CONNECT_REQUEST // demande de connexion au serveur
};

struct PacketHeader { // structure pour le paquet de header
    PacketType type; // type du paquet
    int size; // taille du paquet
};

struct ConnectRequestPacket { 
    char playerName[256]; 
};

namespace Packet {
    bool sendPacket(int socketFd, PacketType type, const void* data, size_t size); // envoie un paquet sur le socket
}

#endif

