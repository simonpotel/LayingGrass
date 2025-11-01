#ifndef SERVER_HPP
#define SERVER_HPP

#include "Packet.hpp"
#include "Player.hpp"
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>

class Server {
public:
    Server(int port); 
    ~Server();
    
    void registerCallback(PacketType type, std::function<void(Player*, const void*, size_t)> callback); // les callbacks sont des fonctions qui sont appelées lorsque des paquets sont reçus (server side)
    void start();
    void stop();
    
private:
    int serverSocket; // descripteur de socket du serveur
    int port;
    bool running;
    std::unordered_map<PacketType, std::function<void(Player*, const void*, size_t)>> callbacks; // map des callbacks pour chaque type de paquet à callback (server side)
    std::vector<std::unique_ptr<Player>> players; // liste des joueurs connectés
    
    void acceptConnections(); // accepte les connexions des clients
    void handleClient(int clientSocket); // gère les clients connectés
    bool receivePacket(int socket, PacketHeader& header, void*& data); // reçoit un paquet d'un client et le décode
};

#endif

