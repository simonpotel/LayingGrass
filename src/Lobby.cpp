#include "Lobby.hpp"
#include "Game/Game.hpp"
#include <algorithm>

Lobby::Lobby(int id) : lobbyId(id), gameStarted(false) {
}

Lobby::~Lobby() {
}

bool Lobby::addConnection(int connection, const std::string& playerName, int colorId) {
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
    playerColors[connection] = colorId; // on associe la couleur du joueur à la connexion
    return true;
}

void Lobby::removeConnection(int connection) {
    // on retire la connexion du lobby
    connections.erase(std::remove(connections.begin(), connections.end(), connection), connections.end());
    playerNames.erase(connection); // on retire le nom du joueur associé
    playerColors.erase(connection); // on retire la couleur du joueur associé
}

void Lobby::clear() {
    game.reset();
    connections.clear();
    playerNames.clear();
    playerColors.clear();
    gameStarted = false;
}

void Lobby::startGame() {
    if (game) {
        return;
    }
    game = std::make_unique<Game>(lobbyId, this);
    gameStarted = true;
}

void Lobby::endGame() {
    game.reset();
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

bool Lobby::hasColor(int colorId) const {
    // parcourt toutes les couleurs des joueurs dans le lobby
    for (const auto& pair : playerColors) {
        if (pair.second == colorId) {
            return true; // une joueur avec cette couleur existe déjà
        }
    }
    return false; // aucun joueur avec cette couleur n'existe
}

std::vector<std::pair<std::string, int>> Lobby::getPlayerColors() const {
    std::vector<std::pair<std::string, int>> result; // liste des joueurs avec leurs couleurs
    for (int conn : connections) {
        auto nameIt = playerNames.find(conn); // cherche le nom du joueur
        auto colorIt = playerColors.find(conn); // cherche la couleur du joueur
        if (nameIt != playerNames.end() && colorIt != playerColors.end()) {
            result.push_back({nameIt->second, colorIt->second}); // ajoute le joueur avec sa couleur
        }
    }
    return result; // retourne la liste des joueurs avec leurs couleurs
}

