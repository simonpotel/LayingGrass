#ifndef PLAYER_HPP
#define PLAYER_HPP

class Player {
public:
    char playerName[256]; 
    int playerId; 
    int connection; // descripteur de socket du joueur
    int lobbyId; // identifiant du lobby auquel le joueur est assigné (-1 si non assigné)

    Player(const char* name, int id, int conn);
    ~Player(); 
};

#endif

