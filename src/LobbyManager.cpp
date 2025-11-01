#include "LobbyManager.hpp"
#include <thread>
#include <chrono>
#include <cstring>
#include <algorithm>

LobbyManager::LobbyManager() : nextLobbyId(1), running(false) {
    auto lobby = std::make_unique<Lobby>(nextLobbyId++);
    lobbies.push_back(std::move(lobby)); // ajoute le lobby à la liste des lobbies
}

LobbyManager::~LobbyManager() {
    stop();
}

Lobby* LobbyManager::findOrCreateAvailableLobby() {
    auto it = std::find_if(lobbies.begin(), lobbies.end(),
        [](const auto& lobby) { return lobby->canAcceptPlayers(); });

    if (it != lobbies.end()) {
        return it->get();
    }

    auto newLobby = std::make_unique<Lobby>(nextLobbyId++);
    lobbies.push_back(std::move(newLobby));
    return lobbies.back().get();
}

Lobby* LobbyManager::findLobbyById(int lobbyId) {
    // cherche un lobby par son identifiant
    auto it = std::find_if(lobbies.begin(), lobbies.end(),
        [lobbyId](const auto& lobby) { return lobby->lobbyId == lobbyId; });

    if (it != lobbies.end()) {
        return it->get(); // retourne le lobby trouvé
    }

    return nullptr; // lobby non trouvé
}

void LobbyManager::updateLobbies() {
    bool hasAvailableLobby = std::any_of(lobbies.begin(), lobbies.end(), // vérifie si il y a un lobby disponible
        [](const auto& lobby) { return lobby->canAcceptPlayers(); }); // lambda pour vérifier si le lobby peut accepter des joueurs
    
    if (!hasAvailableLobby) { // si il n'y a pas de lobby disponible
        auto newLobby = std::make_unique<Lobby>(nextLobbyId++); // crée un nouveau lobby
        lobbies.push_back(std::move(newLobby)); // ajoute le nouveau lobby à la liste
    }
}

LobbyListPacket LobbyManager::getLobbyListPacket() const {
    LobbyListPacket packet; // structure pour le paquet de liste des lobbies
    memset(&packet, 0, sizeof(packet));
    
    int count = std::min(static_cast<int>(lobbies.size()), 100); // nombre de lobbies dans la liste
    packet.lobbyCount = count; // définit le nombre de lobbies dans la liste
    
    for (int i = 0; i < count; ++i) {
        packet.lobbies[i].lobbyId = lobbies[i]->lobbyId; // définit l'id du lobby
        packet.lobbies[i].playerCount = lobbies[i]->getPlayerCount(); // définit le nombre de joueurs dans le lobby
        packet.lobbies[i].gameStarted = lobbies[i]->gameStarted; // définit si la partie est lancée
    }
    
    return packet; // retourne le paquet de liste des lobbies
}

void LobbyManager::startPeriodicUpdates(std::function<void(const LobbyListPacket&)> callback) {
    if (running) {
        return;
    }
    
    running = true; // active le thread périodique
    updateCallback = callback; // stocke le callback
    
    std::thread updateThread([this]() {
        while (running) {
            updateLobbies(); // met à jour les lobbies
            
            if (updateCallback) {
                LobbyListPacket packet = getLobbyListPacket(); // obtient le paquet de liste des lobbies
                updateCallback(packet); // appelle le callback
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(2)); // attend 2 secondes avant la prochaine mise à jour
        }
    });
    updateThread.detach(); // détache le thread pour que le manager puisse continuer à fonctionner
}

void LobbyManager::stop() {
    running = false; // arrête le thread périodique
}
