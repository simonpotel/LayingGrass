#include "Lobby.hpp"
#include <algorithm>

Lobby::Lobby(int id) : lobbyId(id), gameStarted(false) {
}

Lobby::~Lobby() {
}

bool Lobby::addConnection(int connection, const std::string& playerName) {
    if (isFull() || gameStarted) { // si le lobby est plein ou si la partie est lancée, on retourne false
        return false;
    }
    for (int conn : connections) { // on vérifie si la connexion est déjà présente dans le lobby
        if (conn == connection) {
            return false;
        }
    }
    connections.push_back(connection); // on ajoute la connexion au lobby
    playerNames[connection] = playerName; // on associe le nom du joueur à la connexion
    return true;
}

void Lobby::removeConnection(int connection) {
    // on retire la connexion du lobby
    connections.erase(std::remove(connections.begin(), connections.end(), connection), connections.end());
    playerNames.erase(connection); // on retire le nom du joueur associé
}

void Lobby::clear() {
    connections.clear();
    playerNames.clear();
    gameStarted = false;
}

void Lobby::broadcast(PacketType type, const void* data, size_t size) const {
    for (int conn : connections) {
        Packet::sendPacket(conn, type, data, size);
    }
}

bool Lobby::hasPlayerName(const std::string& name) const {
    // parcourt tous les noms de joueurs dans le lobby
    for (const auto& pair : playerNames) {
        if (pair.second == name) {
            return true; // un joueur avec ce nom existe déjà
        }
    }
    return false; // aucun joueur avec ce nom n'existe
}

