#ifndef SERVER_HPP
#define SERVER_HPP

#include "Packet.hpp"
#include "Player.hpp"
#include "LobbyManager.hpp"
#include <functional>
#include <vector>
#include <memory>

class Server {
public:
    Server(int port);
    ~Server();

    // callbacks
    PacketCallbackManager<std::function<void(Player*, const void*, size_t)>>& getCallbackManager() { return callbackManager; }

    void start();
    void stop();

    LobbyManager& getLobbyManager() { return lobbyManager; } // retourne le gestionnaire de lobbies
    void clearLobbyAndRemovePlayers(int lobbyId);

private:
    int serverSocket; // descripteur de socket du serveur
    int port;
    bool running;
    PacketCallbackManager<std::function<void(Player*, const void*, size_t)>> callbackManager; // gestionnaire de callbacks pour les paquets (server side)
    std::vector<std::unique_ptr<Player>> players; // liste des joueurs connectés
    LobbyManager lobbyManager; // gestionnaire des lobbies

    void acceptConnections(); // accepte les connexions des clients
    void handleClient(int clientSocket); // gère les clients connectés
    void removePlayer(int clientSocket); // retire un joueur et le déconnecte du lobby
    void gameUpdateLoop(); // boucle de mise à jour des jeux
};

#endif

