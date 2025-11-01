#include "GameState.hpp"

GameState::GameState()
    : currentState(ClientState::WAITING_FOR_LOBBIES),
      selectedLobbyId(-1),
      currentLobbyId(-1),
      username(""),
      requestSent(false) {
}

GameState::~GameState() {
}

void GameState::updateLobbies(const LobbyListPacket& packet) {
    lobbies.clear(); // vide la liste des lobbies

    // copie les lobbies du packet dans le vecteur
    for (int i = 0; i < packet.lobbyCount; ++i) {
        lobbies.push_back(packet.lobbies[i]);
    }

    // si on était en attente de lobbies et qu'on en a reçu, on passe à la sélection
    if (currentState == ClientState::WAITING_FOR_LOBBIES && !lobbies.empty()) {
        currentState = ClientState::SELECTING_LOBBY;
    }
}
