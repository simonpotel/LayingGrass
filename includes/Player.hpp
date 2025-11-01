#ifndef PLAYER_HPP
#define PLAYER_HPP

class Player {
public:
    char playerName[256]; 
    int playerId; 
    int connection; // descripteur de socket du joueur

    Player(const char* name, int id, int conn);
    ~Player(); 
};

#endif

