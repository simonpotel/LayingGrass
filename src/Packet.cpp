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

// fonction pour envoyer tous les données d'un buffer à un socket
static bool sendAll(int socketFd, const char* buffer, size_t total) {
    size_t sent = 0;
    while (sent < total) {
#ifdef _WIN32
        int n = send(socketFd, buffer + sent, static_cast<int>(total - sent), 0);
#else
        ssize_t n = send(socketFd, buffer + sent, total - sent, 0);
#endif
        if (n <= 0) return false;
        sent += static_cast<size_t>(n);
    }
    return true;
}

// fonction pour recevoir tous les données d'un buffer à partir d'un socket
static bool recvAll(int socketFd, char* buffer, size_t total) {
    size_t recvd = 0;
    while (recvd < total) {
#ifdef _WIN32
        int n = recv(socketFd, buffer + recvd, static_cast<int>(total - recvd), 0);
#else
        ssize_t n = recv(socketFd, buffer + recvd, total - recvd, 0);
#endif
        if (n <= 0) return false;
        recvd += static_cast<size_t>(n);
    }
    return true;
}

bool Packet::sendPacket(int socketFd, PacketType type, const void* data, size_t size) {
    if (socketFd < 0) { // si le socket est fermé 
        return false; // échec de l'envoi 
    }

    uint32_t typeNetwork = htonl(static_cast<uint32_t>(type)); // convertit le type du paquet en format binaire pour le réseau
    uint32_t sizeNetwork = htonl(static_cast<uint32_t>(size)); // convertit la taille du paquet en format binaire pour le réseau

    if (!sendAll(socketFd, (const char*)&typeNetwork, sizeof(uint32_t))) {
        return false; // échec de l'envoi
    }
    if (!sendAll(socketFd, (const char*)&sizeNetwork, sizeof(uint32_t))) {
        return false; // échec de l'envoi
    }
    if (size > 0 && data) {
        if (!sendAll(socketFd, (const char*)data, size)) {
            return false; // échec de l'envoi
        }
    }

    return true;
}

bool Packet::receivePacket(int socketFd, PacketHeader& header, void*& data) {
    data = nullptr; // initialise le pointeur de données à nullptr

    uint32_t typeNetwork = 0; // initialise le type du paquet à 0
    uint32_t sizeNetwork = 0; // initialise la taille du paquet à 0

    if (!recvAll(socketFd, (char*)&typeNetwork, sizeof(uint32_t))) {
        return false;
    }
    if (!recvAll(socketFd, (char*)&sizeNetwork, sizeof(uint32_t))) {
        return false;
    }

    header.type = static_cast<PacketType>(ntohl(typeNetwork));
    header.size = ntohl(sizeNetwork);

    if (header.size > 0) {
        data = new char[header.size];
        if (!recvAll(socketFd, (char*)data, header.size)) {
            delete[] (char*)data;
            data = nullptr;
            return false;
        } // échec de la réception des données
    }

    return true;
}
