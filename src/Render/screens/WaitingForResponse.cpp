#include "Render/screens/WaitingForResponse.hpp"
#include "Render/utils/Text.hpp"
#include "Render/utils/Theme.hpp"
#include "Render/utils/Element.hpp"

void WaitingForResponse::draw(sf::RenderWindow& window, GameState& gameState) {
    sf::Vector2u ws = window.getSize();
    
    // texte centré
    sf::Text waiting = Text::createText("Waiting for response...", 30);
    Element::center(waiting, static_cast<float>(ws.x), static_cast<float>(ws.y)); // centré horizontalement et verticalement au milieu de l'écran
    waiting.setFillColor(sf::Color::White);
    window.draw(waiting);
}

bool WaitingForResponse::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    return false;
}
