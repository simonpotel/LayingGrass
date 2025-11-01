#include "Client.hpp"
#include "Packet.hpp"
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define close(s) closesocket(s)
    #define socklen_t int
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

Client::Client() : socketFd(-1), connected(false) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

Client::~Client() {
    disconnect();
#ifdef _WIN32
    WSACleanup();
#endif
}

bool Client::connect(const char* serverIp, int port) {
    socketFd = socket(AF_INET, SOCK_STREAM, 0); // crée un socket pour la connexion au serveur
    
    sockaddr_in serverAddress; // structure pour l'adresse du serveur
    serverAddress.sin_family = AF_INET; // famille d'adresse (IPv4)
    serverAddress.sin_port = htons(port); // port du serveur
    inet_pton(AF_INET, serverIp, &serverAddress.sin_addr); // convertit l'adresse IP en format binaire pour la connexion au serveur
    
    if (::connect(socketFd, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) { // connecte le client au serveur
        return false; // connexion échouée
    }
    
    connected = true; // le client est connecté au serveur
    return true;
}

void Client::disconnect() {
    close(socketFd); // ferme le socket
    socketFd = -1; // réinitialise le descripteur de socket à -1
    connected = false; // le client n'est plus connecté au serveur
}

bool Client::sendConnectRequest(const char* playerName) {
    ConnectRequestPacket packet; // structure pour la demande de connexion
    memset(&packet, 0, sizeof(packet)); // initialise la structure à 0
    strncpy(packet.playerName, playerName, sizeof(packet.playerName) - 1); // copie le nom du joueur dans la structure
    
    return Packet::sendPacket(socketFd, PacketType::CONNECT_REQUEST, &packet, sizeof(ConnectRequestPacket)); // envoie la demande de connexion au serveur
}

