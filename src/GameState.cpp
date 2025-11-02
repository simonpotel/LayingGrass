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
    lobbies.clear();

    for (int i = 0; i < packet.lobbyCount; ++i) {
        lobbies.push_back(packet.lobbies[i]);
    }

    if (currentState == ClientState::WAITING_FOR_LOBBIES && !lobbies.empty()) {
        currentState = ClientState::SELECTING_LOBBY;
    } else if (currentState == ClientState::GAME_END && !lobbies.empty()) {
        currentState = ClientState::SELECTING_LOBBY;
    }
}
