#include "Lobby.hpp"
#include <algorithm>

Lobby::Lobby(int id) : lobbyId(id), gameStarted(false) {
}

Lobby::~Lobby() {
}

bool Lobby::addConnection(int connection) { // ajoute une connexion au lobby
    if (isFull() || gameStarted) { // si le lobby est plein ou si la partie est lancée, on retourne false
        return false;
    }
    for (int conn : connections) { // on vérifie si la connexion est déjà présente dans le lobby
        if (conn == connection) {
            return false;
        }
    }
    connections.push_back(connection); // on ajoute la connexion au lobby
    return true;
}

void Lobby::removeConnection(int connection) {
    // on retire la connexion du lobby
    connections.erase(std::remove(connections.begin(), connections.end(), connection), connections.end());
}

