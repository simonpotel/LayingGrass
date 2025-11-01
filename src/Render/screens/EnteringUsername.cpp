#include "Render/screens/EnteringUsername.hpp"
#include "Render/utils/Text.hpp"

void EnteringUsername::draw(sf::RenderWindow& window, GameState& gameState) {
    Text::draw(window, "Enter your username:", 250, 200, 30);

    // affiche le nom d'utilisateur en cours de saisie
    std::string displayName = gameState.getUsername();
    if (displayName.empty()) {
        displayName = "_";
    }

    Text::drawBox(window, 200, 280, 400, 50, sf::Color(40, 40, 40), sf::Color::White);
    Text::draw(window, displayName, 220, 290, 24);

    Text::draw(window, "Press Enter to confirm", 250, 400, 20);
}

bool EnteringUsername::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode == 8) { // backspace
            std::string currentName = gameState.getUsername();
            if (!currentName.empty()) {
                currentName.pop_back();
                gameState.setUsername(currentName);
            }
        } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
            std::string currentName = gameState.getUsername();
            if (currentName.length() < 20) {
                currentName += static_cast<char>(event.text.unicode);
                gameState.setUsername(currentName);
            }
        }
    }

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
        if (!gameState.getUsername().empty()) {
            gameState.setState(ClientState::WAITING_FOR_RESPONSE);
        }
    }
    return false;
}
