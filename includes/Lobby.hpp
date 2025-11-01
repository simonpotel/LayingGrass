#ifndef LOBBY_HPP
#define LOBBY_HPP

#include <vector>
#include <string>
#include <unordered_map>

class Lobby {
public:
    int lobbyId;
    bool gameStarted;
    std::vector<int> connections; // liste des descripteurs de socket des clients connectés au lobby
    std::unordered_map<int, std::string> playerNames; // map connection -> nom du joueur

    Lobby(int id);
    ~Lobby();

    bool addConnection(int connection, const std::string& playerName); // ajoute une connexion au lobby avec le nom du joueur
    void removeConnection(int connection); // retire une connexion du lobby
    int getPlayerCount() const { return connections.size(); } // retourne le nombre de joueurs connectés au lobby
    bool isFull() const { return connections.size() >= 2; } // retourne true si le lobby est plein (2 joueurs max)
    bool canAcceptPlayers() const { return !gameStarted && !isFull(); } // retourne true si le lobby peut accepter des joueurs
    bool hasPlayerName(const std::string& name) const; // vérifie si un nom de joueur existe déjà dans le lobby
};

#endif

