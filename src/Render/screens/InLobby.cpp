#include "Render/screens/InLobby.hpp"
#include "Render/utils/Text.hpp"
#include "Render/utils/Theme.hpp"
#include "Render/utils/Element.hpp"
#include "GameState.hpp"
#include <sstream>

void InLobby::draw(sf::RenderWindow& window, GameState& gameState) {
    sf::Vector2u ws = window.getSize();
    
    // conteneur
    float cx = (ws.x - 500.0f) / 2.0f; // position x centrée: (largeur fenêtre - largeur conteneur) / 2
    float cy = (ws.y - 350.0f) / 2.0f; // position y centrée: (hauteur fenêtre - hauteur conteneur) / 2
    
    sf::RectangleShape container(sf::Vector2f(500.0f, 350.0f));
    container.setPosition(cx, cy);
    container.setFillColor(sf::Color(30, 30, 30));
    container.setOutlineColor(sf::Color::White);
    container.setOutlineThickness(2);
    window.draw(container);
    
    int lobbyId = gameState.getCurrentLobby();
    std::stringstream ss;
    ss << "Connected to Lobby #" << lobbyId;
    sf::Text lobby = Text::createText(ss.str(), 32);
    Element::centerH(lobby, static_cast<float>(ws.x), cy + 30.0f); // centré horizontalement, positionné dans le conteneur
    lobby.setFillColor(sf::Color::White);
    window.draw(lobby);
    
    sf::Text playersLabel = Text::createText("Players:", 28);
    Element::centerH(playersLabel, static_cast<float>(ws.x), cy + 80.0f); // centré horizontalement, positionné dans le conteneur
    playersLabel.setFillColor(sf::Color::White);
    window.draw(playersLabel);
    
    const auto& lobbies = gameState.getLobbies();
    float py = cy + 120.0f; // position y de départ des joueurs: début conteneur + offset vertical
    
    for (const auto& lobby : lobbies) {
        if (lobby.lobbyId == lobbyId) {
            for (int i = 0; i < lobby.playerCount; i++) {
                if (lobby.players[i].colorId >= 0 && lobby.players[i].colorId < 9) {
                    float px = (ws.x - (35.0f + 10 + 200)) / 2.0f; // position x centrée: (largeur fenêtre - (taille couleur + espacement + largeur texte)) / 2
                    sf::Color color = GameState::PLAYERS_COLORS[lobby.players[i].colorId];
                    Text::drawBox(window, px, py, 35.0f, 35.0f, color, sf::Color::White);
                    
                    sf::Text name = Text::createText(lobby.players[i].playerName, 22);
                    name.setPosition(px + 35.0f + 10, py + 5);
                    name.setFillColor(sf::Color::White);
                    window.draw(name);
                    
                    py += 50.0f;
                }
            }
            break;
        }
    }
    
    // bouton Start Game en bas de l'écran, centré horizontalement
    float btnWidth = 250.0f;
    float btnHeight = 60.0f;
    float btnX = (ws.x - btnWidth) / 2.0f; // centré horizontalement
    float btnY = ws.y - btnHeight - 30.0f; // en bas de l'écran avec marge de 30px
    
    // vérifie si le lobby a au moins 2 joueurs
    bool canStart = false;
    for (const auto& lobby : lobbies) {
        if (lobby.lobbyId == lobbyId) {
            canStart = lobby.playerCount >= 2;
            break;
        }
    }
    
    sf::RectangleShape btn(sf::Vector2f(btnWidth, btnHeight));
    btn.setPosition(btnX, btnY);
    btn.setFillColor(canStart ? Theme::FOREST_GREEN : Theme::NEUTRAL_GRAY);
    btn.setOutlineColor(sf::Color::White);
    btn.setOutlineThickness(2);
    window.draw(btn);
    
    sf::Text btnText = Text::createText("Start Game", 28);
    Element::centerInContainer(btnText, btnX, btnY, btnWidth, btnHeight);
    btnText.setFillColor(canStart ? sf::Color::White : sf::Color(128, 128, 128));
    window.draw(btnText);
}

bool InLobby::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    if (event.type != sf::Event::MouseButtonPressed || event.mouseButton.button != sf::Mouse::Left) {
        return false;
    }
    
    sf::Vector2f mouse(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
    sf::Vector2u ws = window.getSize();
    
    float cx = (ws.x - 500.0f) / 2.0f;
    float cy = (ws.y - 350.0f) / 2.0f;
    
    // bouton Start Game en bas de l'écran, centré horizontalement
    float btnWidth = 250.0f;
    float btnHeight = 60.0f;
    float btnX = (ws.x - btnWidth) / 2.0f;
    float btnY = ws.y - btnHeight - 30.0f; // en bas de l'écran avec marge de 30px
    sf::FloatRect btnBounds(btnX, btnY, btnWidth, btnHeight);
    
    if (btnBounds.contains(mouse.x, mouse.y)) {
        int lobbyId = gameState.getCurrentLobby();
        const auto& lobbies = gameState.getLobbies();
        
        // vérifie si le lobby a au moins 2 joueurs
        bool canStart = false;
        for (const auto& lobby : lobbies) {
            if (lobby.lobbyId == lobbyId) {
                canStart = lobby.playerCount >= 2;
                break;
            }
        }
        
        if (canStart && lobbyId != -1) {
            // le packet sera envoyé dans client.cpp
            gameState.setStartGameRequested(true);
            return false;
        }
    }
    
    return false;
}
