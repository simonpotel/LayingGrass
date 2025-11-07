#include "Render/screens/GameEnd.hpp"
#include "Render/utils/Text.hpp"
#include "Render/utils/Theme.hpp"
#include "Render/utils/Element.hpp"
#include "GameState.hpp"
#include <sstream>

void GameEnd::draw(sf::RenderWindow& window, GameState& gameState) {
    sf::Vector2u ws = window.getSize();
    
    // conteneur
    float cx = (ws.x - 600.0f) / 2.0f; // position x centrée: (largeur fenêtre - largeur conteneur) / 2
    float cy = (ws.y - 400.0f) / 2.0f; // position y centrée: (hauteur fenêtre - hauteur conteneur) / 2
    
    sf::RectangleShape container(sf::Vector2f(600.0f, 400.0f));
    container.setPosition(cx, cy);
    container.setFillColor(sf::Color(30, 30, 30));
    container.setOutlineColor(sf::Color::White);
    container.setOutlineThickness(2);
    window.draw(container);
    
    int lobbyId = gameState.getGameEndLobbyId();
    int winnerId = gameState.getGameEndWinnerId();
    const std::string& winnerName = gameState.getGameEndWinnerName();
    
    // "Game Ended!"
    sf::Text ended = Text::createText("Game Ended!", 48);
    Element::centerH(ended, static_cast<float>(ws.x), cy + 40.0f); // centré horizontalement, positionné dans le conteneur
    ended.setFillColor(sf::Color::White);
    window.draw(ended);
    
    if (winnerId >= 0 && winnerId < 9) {
        sf::Color winnerColor = GameState::PLAYERS_COLORS[winnerId];
        
        float colorX = (ws.x - 50.0f) / 2.0f; // position x centrée de la couleur: (largeur fenêtre - taille couleur) / 2
        float colorY = cy + 120.0f; // position y: début conteneur + offset vertical
        Text::drawBox(window, colorX, colorY, 50.0f, 50.0f, winnerColor, sf::Color::White);
        
        std::stringstream ss;
        ss << (winnerName.empty() ? "Color " + std::to_string(winnerId) : winnerName);
        sf::Text name = Text::createText(ss.str(), 24);
        Element::centerH(name, static_cast<float>(ws.x), colorY + 50.0f + 10); // centré horizontalement, positionné sous la couleur avec espacement
        name.setFillColor(sf::Color::White);
        window.draw(name);
        
        sf::Text winner = Text::createText("Winner!", 28);
        Element::centerH(winner, static_cast<float>(ws.x), cy + 220.0f); // centré horizontalement, positionné dans le conteneur
        winner.setFillColor(sf::Color::White);
        window.draw(winner);
    } else {
        sf::Text noWinner = Text::createText("No winner", 32);
        Element::centerH(noWinner, static_cast<float>(ws.x), cy + 150.0f); // centré horizontalement, positionné dans le conteneur
        noWinner.setFillColor(sf::Color::White);
        window.draw(noWinner);
    }
    
    std::stringstream ss;
    ss << "Lobby #" << lobbyId;
    sf::Text lobby = Text::createText(ss.str(), 24);
    Element::centerH(lobby, static_cast<float>(ws.x), cy + 280.0f); // centré horizontalement, positionné dans le conteneur
    lobby.setFillColor(sf::Color::White);
    window.draw(lobby);
    
    sf::Text returning = Text::createText("Returning to lobby selection...", 20);
    Element::centerH(returning, static_cast<float>(ws.x), cy + 400.0f - 40.0f); // centré horizontalement, positionné en bas du conteneur avec marge
    returning.setFillColor(sf::Color::White);
    window.draw(returning);
}

bool GameEnd::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    return false;
}
