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

// envoie un paquet sur le socket (Server to Client)
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
