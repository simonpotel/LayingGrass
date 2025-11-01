#include "Render/screens/InLobby.hpp"
#include "Render/utils/Text.hpp"
#include <sstream>

void InLobby::draw(sf::RenderWindow& window, GameState& gameState) {
    int currentLobbyId = gameState.getCurrentLobby();

    std::stringstream ss;
    ss << "Connected to Lobby #" << currentLobbyId;
    Text::draw(window, ss.str(), 200, 200, 32);

    Text::draw(window, "Waiting for game to start...", 200, 300, 24);
}

bool InLobby::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    return false;
}
