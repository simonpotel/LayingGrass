#ifndef LOBBYMANAGER_HPP
#define LOBBYMANAGER_HPP

#include "Lobby.hpp"
#include "Packet.hpp"
#include <vector>
#include <memory>
#include <functional>

class LobbyManager {
public:
    LobbyManager();
    ~LobbyManager();
    
    Lobby* findOrCreateAvailableLobby(); // trouve un lobby disponible ou en crée un nouveau
    Lobby* findLobbyById(int lobbyId); // trouve un lobby par son identifiant
    void updateLobbies(); // met à jour les lobbies et crée de nouveaux lobbies si nécessaire
    LobbyListPacket getLobbyListPacket() const; // retourne le paquet de liste des lobbies
    void startPeriodicUpdates(std::function<void(const LobbyListPacket&)> callback); // démarre le thread périodique de mise à jour
    void stop(); // arrête le thread périodique
    
private:
    std::vector<std::unique_ptr<Lobby>> lobbies; // liste des lobbies disponibles
    int nextLobbyId; // prochain identifiant de lobby à créer
    bool running; // true si le thread périodique est actif
    std::function<void(const LobbyListPacket&)> updateCallback; // callback pour envoyer les mises à jour
};

#endif

