#ifndef LOBBY_HPP
#define LOBBY_HPP

#include "Packet.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

class Game;

class Lobby {
public:
    int lobbyId;
    bool gameStarted;
    std::vector<int> connections; // liste des descripteurs de socket des clients connectés au lobby
    std::unordered_map<int, std::string> playerNames; // map connection -> nom du joueur
    std::unordered_map<int, int> playerColors; // map connection -> couleur du joueur   
    std::unique_ptr<Game> game;

    Lobby(int id);
    ~Lobby();

    bool addConnection(int connection, const std::string& playerName, int colorId); // ajoute une connexion au lobby avec le nom du joueur et la couleur du joueur
    void removeConnection(int connection); // retire une connexion du lobby
    void clear(); // vide le lobby et le réinitialise
    void broadcast(PacketType type, const void* data, size_t size) const; // envoie un paquet à tous les joueurs du lobby
    int getPlayerCount() const { return connections.size(); } // retourne le nombre de joueurs connectés au lobby
    bool isFull() const { return connections.size() >= 9; } // retourne true si le lobby est plein (9 joueurs max)
    bool canAcceptPlayers() const { return !gameStarted && !isFull(); } // retourne true si le lobby peut accepter des joueurs
    bool hasPlayerName(const std::string& name) const; // vérifie si un nom de joueur existe déjà dans le lobby
    bool hasColor(int colorId) const; // vérifie si une couleur existe déjà dans le lobby
    std::vector<std::pair<std::string, int>> getPlayerColors() const; // retourne la liste des joueurs avec leurs noms et couleurs
    
    Game* getGame() { return game.get(); } // retourne le jeu associé au lobby
    void startGame(); // démarre le jeu pour le lobby
    void endGame(); // termine le jeu pour le lobby
};

#endif

