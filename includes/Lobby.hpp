#ifndef LOBBY_HPP
#define LOBBY_HPP

#include <vector>

class Lobby {
public:
    int lobbyId;
    bool gameStarted;
    std::vector<int> connections; // liste des descripteurs de socket des clients connectés au lobby

    Lobby(int id);
    ~Lobby();

    bool addConnection(int connection); // ajoute une connexion au lobby
    void removeConnection(int connection); // retire une connexion du lobby
    int getPlayerCount() const { return connections.size(); } // retourne le nombre de joueurs connectés au lobby
    bool isFull() const { return connections.size() >= 2; } // retourne true si le lobby est plein (2 joueurs max)
    bool canAcceptPlayers() const { return !gameStarted && !isFull(); } // retourne true si le lobby peut accepter des joueurs
};

#endif

