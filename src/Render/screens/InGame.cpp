#include "Render/screens/InGame.hpp"
#include "Render/utils/Text.hpp"
#include <sstream>

void InGame::draw(sf::RenderWindow& window, GameState& gameState) {
    int currentLobbyId = gameState.getCurrentLobby();

    std::stringstream ss;
    ss << "Game Started!";
    Text::draw(window, ss.str(), 250, 250, 48);

    Text::draw(window, "Playing in Lobby #" + std::to_string(currentLobbyId), 200, 320, 24);
}

bool InGame::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    return false;
}

