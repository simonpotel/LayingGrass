#include "Render/screens/WaitingForResponse.hpp"
#include "Render/utils/Text.hpp"

void WaitingForResponse::draw(sf::RenderWindow& window, GameState& gameState) {
    Text::draw(window, "Waiting for response...", 250, 280);
}

bool WaitingForResponse::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    return false;
}

