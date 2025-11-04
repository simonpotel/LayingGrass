#include "Render/screens/GameEnd.hpp"
#include "Render/utils/Text.hpp"
#include "GameState.hpp"
#include <sstream>

void GameEnd::draw(sf::RenderWindow& window, GameState& gameState) {
    int currentLobbyId = gameState.getGameEndLobbyId();
    int winnerId = gameState.getGameEndWinnerId();
    const std::string& winnerName = gameState.getGameEndWinnerName();

    std::stringstream ss;
    ss << "Game Ended!";
    Text::draw(window, ss.str(), 250, 180, 48);

    if (winnerId >= 0 && winnerId < 9) {
        sf::Color winnerColor = GameState::PLAYERS_COLORS[winnerId];
        Text::drawBox(window, 200, 250, 50, 50, winnerColor, sf::Color::White);
        
        ss.str("");
        if (!winnerName.empty()) {
            ss << winnerName;
        } else {
            ss << "Color " << winnerId;
        }
        Text::draw(window, ss.str(), 260, 265, 24);
        
        ss.str("");
        ss << "Winner!";
        Text::draw(window, ss.str(), 200, 320, 28);
    } else {
        Text::draw(window, "No winner", 250, 280, 32);
    }

    Text::draw(window, "Lobby #" + std::to_string(currentLobbyId), 300, 380, 24);
    Text::draw(window, "Returning to lobby selection...", 180, 440, 20);
}

bool GameEnd::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    return false;
}

