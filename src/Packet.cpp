#include "Packet.hpp"
#include <cstring>
#include <cstdint>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
#endif

bool Packet::sendPacket(int socketFd, PacketType type, const void* data, size_t size) {
    if (socketFd < 0) { // si le socket est fermé
        return false; // échec de l'envoi
    }

    uint32_t typeNetwork = htonl(static_cast<uint32_t>(type)); // convertit le type du paquet en format binaire
    uint32_t sizeNetwork = htonl(static_cast<uint32_t>(size)); // convertit la taille du paquet en format binaire

    if (send(socketFd, (const char*)&typeNetwork, sizeof(uint32_t), 0) < 0) {
        return false; // échec de l'envoi
    }

    if (send(socketFd, (const char*)&sizeNetwork, sizeof(uint32_t), 0) < 0) {
        return false; // échec de l'envoi
    }

    if (size > 0 && send(socketFd, (const char*)data, size, 0) < 0) {
        return false; // échec de l'envoi
    }

    return true; // succès de l'envoi
}

bool Packet::receivePacket(int socket, PacketHeader& header, void*& data) {
    int typeNetwork, sizeNetwork;

    if (recv(socket, (char*)&typeNetwork, sizeof(int), 0) <= 0) {
        return false;
    }

    if (recv(socket, (char*)&sizeNetwork, sizeof(int), 0) <= 0) {
        return false;
    }

    header.type = static_cast<PacketType>(ntohl(static_cast<uint32_t>(typeNetwork)));
    header.size = ntohl(static_cast<uint32_t>(sizeNetwork));

    if (header.size > 0) {
        data = new char[header.size];
        recv(socket, (char*)data, header.size, 0);
    }

    return true;
}
