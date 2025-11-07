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
    
    sf::Text waiting = Text::createText("Waiting for game to start...", 24);
    Element::centerH(waiting, static_cast<float>(ws.x), cy + 350.0f - 50.0f); // centré horizontalement, positionné en bas du conteneur avec marge
    waiting.setFillColor(sf::Color::White);
    window.draw(waiting);
}

bool InLobby::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    return false;
}
