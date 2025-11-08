#include "Client.hpp"
#include "Packet.hpp"
#include <cstring>
#include <thread>
#include <iostream>

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

Client::Client() : socketFd(-1), connected(false), receiving(false) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

Client::~Client() {
    stopReceiving(); // arrête le thread de réception
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
    stopReceiving(); // arrête le thread de réception
    close(socketFd); // ferme le socket
    socketFd = -1; // réinitialise le descripteur de socket à -1
    connected = false; // le client n'est plus connecté au serveur
}

bool Client::sendConnectRequest(const char* playerName, int lobbyId, int colorId) {
    ConnectRequestPacket packet; // structure pour la demande de connexion
    memset(&packet, 0, sizeof(packet)); // initialise la structure à 0
    strncpy(packet.playerName, playerName, sizeof(packet.playerName) - 1); // copie le nom du joueur dans la structure
    packet.lobbyId = lobbyId; // définit l'identifiant du lobby
    packet.colorId = colorId; // définit l'identifiant de la couleur

    return Packet::sendPacket(socketFd, PacketType::CONNECT_REQUEST, &packet, sizeof(ConnectRequestPacket)); // envoie la demande de connexion au serveur
}

bool Client::sendCellClick(int lobbyId, int row, int col, int rotation, bool flippedH, bool flippedV, bool useCoupon) {
    CellClickPacket packet; // structure pour le clic sur une cellule
    memset(&packet, 0, sizeof(packet)); // initialise la structure à 0
    packet.lobbyId = lobbyId; // définit l'identifiant du lobby
    packet.row = row; // définit la ligne
    packet.col = col; // définit la colonne
    packet.rotation = rotation; // définit la rotation
    packet.flippedH = flippedH; // définit le flip horizontal
    packet.flippedV = flippedV; // définit le flip vertical
    packet.useCoupon = useCoupon;

    return Packet::sendPacket(socketFd, PacketType::CELL_CLICK, &packet, sizeof(CellClickPacket)); // envoie le clic au serveur
}

void Client::startReceiving() {
    if (receiving || !connected) {
        return; // déjà en cours de réception ou non connecté
    }
    receiving = true; // active la réception
    std::thread receiveThread(&Client::receiveLoop, this); // crée un thread pour la réception des paquets
    receiveThread.detach(); // détache le thread pour que le client puisse continuer à fonctionner
}

void Client::stopReceiving() {
    receiving = false; // arrête la réception
}

void Client::receiveLoop() {
    while (receiving && connected) {
        PacketHeader header; // structure pour le paquet de header
        void* data = nullptr;

        if (!Packet::receivePacket(socketFd, header, data)) {
            break; // si le paquet n'est pas reçu, on arrête la boucle
        }

        // appelle le callback correspondant au type de paquet via le callback manager
        auto* callback = callbackManager.getCallback(header.type);
        if (callback) {
            std::cout << "[PACKET] Type: " << static_cast<int>(header.type) << " Size: " << header.size << std::endl;
            (*callback)(data, header.size);
        }
        if (data) {
            delete[] (char*)data; // on libère la mémoire allouée pour le paquet
        }
    }
    receiving = false; // indique que la réception est terminée
}

