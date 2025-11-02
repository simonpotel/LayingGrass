#include "Render/screens/GameEnd.hpp"
#include "Render/utils/Text.hpp"
#include <sstream>

void GameEnd::draw(sf::RenderWindow& window, GameState& gameState) {
    int currentLobbyId = gameState.getCurrentLobby();

    std::stringstream ss;
    ss << "Game Ended!";
    Text::draw(window, ss.str(), 250, 250, 48);

    Text::draw(window, "Lobby #" + std::to_string(currentLobbyId), 300, 320, 24);
    Text::draw(window, "Returning to lobby selection...", 180, 380, 20);
}

bool GameEnd::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    return false;
}

