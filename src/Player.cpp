#include "Player.hpp"
#include <cstring>

// constructeur pour le joueur
Player::Player(const char* name, int id, int conn)
    : playerId(id), connection(conn) {
    strncpy(playerName, name, sizeof(playerName) - 1); // copie le nom du joueur dans la structure
    playerName[sizeof(playerName) - 1] = '\0'; // ajoute un caractère nul à la fin du nom du joueur pour éviter les problèmes de segmentation
}

Player::~Player() {
}

