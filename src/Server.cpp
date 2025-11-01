#include "Server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>

// constructeur pour le serveur
Server::Server(int port) : port(port), running(false) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0); // crée un socket pour le serveur
    
    int opt = 1; // option pour le socket qui permet de réutiliser l'adresse
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // permet de réutiliser l'adresse du socket
    
    sockaddr_in address; // structure pour l'adresse du serveur
    address.sin_family = AF_INET; // famille d'adresse (IPv4)
    address.sin_addr.s_addr = INADDR_ANY; // adresse IP du serveur
    address.sin_port = htons(port); // port du serveur
    
    bind(serverSocket, (sockaddr*)&address, sizeof(address)); // lie le socket à l'adresse
    listen(serverSocket, 30); // écoute les connexions des clients (30 max)
}

Server::~Server() {
    stop(); // arrête le serveur
    close(serverSocket); // ferme le socket
}

void Server::registerCallback(PacketType type, std::function<void(Player*, const void*, size_t)> callback) {
    callbacks[type] = callback; // enregistre le callback pour le type de paquet dans la map des callbacks
}

void Server::start() {
    running = true; // le serveur est en cours d'exécution
    std::thread acceptThread(&Server::acceptConnections, this); // crée un thread pour accepter les connexions des clients
    acceptThread.detach(); // détache le thread pour que le serveur puisse continuer à fonctionner
}

void Server::stop() {
    running = false; // le serveur n'est plus en cours d'exécution
}

void Server::acceptConnections() {
    while (running) {
        sockaddr_in clientAddress; // structure pour l'adresse du client
        socklen_t clientLen = sizeof(clientAddress); // taille de l'adresse du client
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &clientLen); // accepte la connexion du client
        
        if (clientSocket < 0) {
            continue; // si la connexion échoue, on continue la boucle
        }
        
        auto player = std::make_unique<Player>("", 0, clientSocket); // crée un joueur pour le client
        players.push_back(std::move(player)); // ajoute le joueur à la liste des joueurs
        
        std::thread clientThread(&Server::handleClient, this, clientSocket); // crée un thread pour gérer le client
        clientThread.detach(); // détache le thread pour que le serveur puisse continuer à fonctionner
    }
}

void Server::handleClient(int clientSocket) {
    Player* player = nullptr; // pointeur vers le joueur
    for (auto& p : players) {
        if (p->connection == clientSocket) {
            player = p.get(); // on récupère le joueur
            break;
        }
    }
    
    if (!player) return; // si le joueur n'est pas trouvé, on continue la boucle
    
    while (running) {
        PacketHeader header; // structure pour le paquet de header
        void* data = nullptr;
        
        if (!receivePacket(clientSocket, header, data)) {
            break; // si le paquet n'est pas reçu, on continue la boucle
        }
        
        auto it = callbacks.find(header.type); // on cherche le callback pour le type de paquet
        if (it != callbacks.end()) {
            it->second(player, data, header.size); // on appelle le callback pour le type de paquet
        }
        
        if (data) {
            delete[] (char*)data; // on libère la mémoire allouée pour le paquet
        }
    }
    
    close(clientSocket); // ferme le socket du client
}

bool Server::receivePacket(int socket, PacketHeader& header, void*& data) {
    int typeNetwork, sizeNetwork;
    
    if (recv(socket, &typeNetwork, sizeof(int), 0) <= 0) {
        return false; // si le paquet n'est pas reçu, on continue la boucle
    }
    
    if (recv(socket, &sizeNetwork, sizeof(int), 0) <= 0) {
        return false; // si la taille du paquet n'est pas reçue, on continue la boucle
    }
    
    header.type = static_cast<PacketType>(ntohl(static_cast<uint32_t>(typeNetwork))); // on convertit le type du paquet en format binaire
    header.size = ntohl(static_cast<uint32_t>(sizeNetwork)); // on convertit la taille du paquet en format binaire
    
    if (header.size > 0) { 
        data = new char[header.size];
        recv(socket, data, header.size, 0); // on reçoit le paquet
    }
    
    return true; // succès de la réception du paquet
}

