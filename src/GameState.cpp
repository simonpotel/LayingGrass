#include "GameState.hpp"
#include <SFML/Graphics.hpp>

namespace {
    const sf::Color PLAYERS_COLORS_ARRAY[9] = {
        sf::Color(255, 0, 0),     // rouge
        sf::Color(0, 0, 255),     // bleu
        sf::Color(0, 255, 0),     // vert
        sf::Color(255, 255, 0),   // jaune
        sf::Color(255, 165, 0),   // orange
        sf::Color(128, 0, 128),   // violet
        sf::Color(0, 255, 255),   // cyan
        sf::Color(255, 0, 255),   // magenta
        sf::Color(255, 192, 203)  // rose
    };
}

const sf::Color* GameState::PLAYERS_COLORS = PLAYERS_COLORS_ARRAY;

GameState::GameState()
    : currentState(ClientState::WAITING_FOR_LOBBIES),
      selectedLobbyId(-1),
      currentLobbyId(-1),
      username(""),
      selectedColorId(-1),
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
