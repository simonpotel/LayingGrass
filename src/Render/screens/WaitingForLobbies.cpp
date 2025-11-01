#include "Render/screens/WaitingForLobbies.hpp"
#include "Render/utils/Text.hpp"

void WaitingForLobbies::draw(sf::RenderWindow& window, GameState& gameState) {
    Text::draw(window, "Waiting for lobbies...", 300, 280);
}

bool WaitingForLobbies::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    return false;
}
