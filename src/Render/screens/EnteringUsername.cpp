#include "Render/screens/EnteringUsername.hpp"
#include "Render/utils/Text.hpp"
#include "Render/utils/Tooltip.hpp"
#include "Render/utils/Theme.hpp"
#include "Render/utils/Element.hpp"
#include "GameState.hpp"
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <algorithm>

void EnteringUsername::draw(sf::RenderWindow& window, GameState& gameState) {
    sf::Vector2u ws = window.getSize();
    
    // conteneur
    float cx = (ws.x - 600.0f) / 2.0f; // position x centrée: (largeur fenêtre - largeur conteneur) / 2
    float cy = (ws.y - 500.0f) / 2.0f; // position y centrée: (hauteur fenêtre - hauteur conteneur) / 2
    
    sf::RectangleShape container(sf::Vector2f(600.0f, 500.0f));
    container.setPosition(cx, cy);
    container.setFillColor(sf::Color(30, 30, 30));
    container.setOutlineColor(sf::Color::White);
    container.setOutlineThickness(2);
    window.draw(container);
    
    // username
    sf::Text label = Text::createText("Enter your username:", 30);
    Element::centerH(label, static_cast<float>(ws.x), cy + 40.0f); // centré horizontalement, positionné dans le conteneur
    label.setFillColor(sf::Color::White);
    window.draw(label);
    
    float ux = (ws.x - 400.0f) / 2.0f; // position x centrée de la boîte de saisie: (largeur fenêtre - largeur boîte) / 2
    float uy = cy + 90.0f; // position y: début conteneur + offset vertical
    Text::drawBox(window, ux, uy, 400.0f, 50.0f, sf::Color(40, 40, 40), sf::Color::White);
    
    std::string name = gameState.getUsername();
    if (name.empty()) name = "_";
    sf::Text nameText = Text::createText(name, 24);
    nameText.setPosition(ux + 20, uy + 10);
    nameText.setFillColor(sf::Color::White);
    window.draw(nameText);
    
    // couleurs
    sf::Text colorLabel = Text::createText("Select a color:", 28);
    Element::centerH(colorLabel, static_cast<float>(ws.x), cy + 180.0f); // centré horizontalement, positionné dans le conteneur
    colorLabel.setFillColor(sf::Color::White);
    window.draw(colorLabel);
    
    int lobbyId = gameState.getSelectedLobby();
    std::vector<int> taken;
    std::unordered_map<int, std::string> colorToName;
    if (lobbyId != -1) {
        for (const auto& lobby : gameState.getLobbies()) {
            if (lobby.lobbyId == lobbyId) {
                for (int i = 0; i < lobby.playerCount; i++) {
                    taken.push_back(lobby.players[i].colorId);
                    colorToName[lobby.players[i].colorId] = lobby.players[i].playerName;
                }
                break;
            }
        }
    }
    
    sf::Vector2i mouse = sf::Mouse::getPosition(window);
    int hovered = -1;
    float csx = (ws.x - (3 * 50.0f + 2 * 10.0f)) / 2.0f; // position x centrée de la grille: (largeur fenêtre - (3 cases * taille + 2 espacements)) / 2
    float csy = cy + 230.0f; // position y: début conteneur + offset vertical
    
    for (int i = 0; i < 9; i++) {
        float x = csx + (i % 3) * (50.0f + 10.0f); // position x: début grille + (colonne * (taille case + espacement))
        float y = csy + (i / 3) * (50.0f + 10.0f); // position y: début grille + (ligne * (taille case + espacement))
        
        bool isTaken = std::find(taken.begin(), taken.end(), i) != taken.end();
        bool selected = gameState.getSelectedColor() == i;
        
        if (mouse.x >= x && mouse.x <= x + 50.0f && mouse.y >= y && mouse.y <= y + 50.0f) {
            hovered = i;
        }
        
        sf::Color color = GameState::PLAYERS_COLORS[i];
        if (isTaken) color = sf::Color(color.r / 2, color.g / 2, color.b / 2); // assombrir la couleur si elle est déjà prise (diviser chaque composante par 2)
        
        sf::Color outline = selected ? sf::Color::White : (hovered == i && !isTaken ? sf::Color(200, 200, 200) : sf::Color(100, 100, 100));
        Text::drawBox(window, x, y, 50.0f, 50.0f, color, outline);
    }
    
    if (hovered != -1) {
        auto it = colorToName.find(hovered);
        if (it != colorToName.end()) {
            float x = csx + (hovered % 3) * (50.0f + 10.0f) + 50.0f / 2.0f; // position x du centre de la case: position case + moitié de la taille
            float y = csy + (hovered / 3) * (50.0f + 10.0f) + 50.0f / 2.0f; // position y du centre de la case: position case + moitié de la taille
            Tooltip::draw(window, "Player \"" + it->second + "\" already has this color !", x, y);
        }
    }
    
    sf::Text confirm = Text::createText("Press Enter to confirm", 20);
    Element::centerH(confirm, static_cast<float>(ws.x), cy + 500.0f - 40.0f); // centré horizontalement, positionné en bas du conteneur avec marge
    confirm.setFillColor(sf::Color::White);
    window.draw(confirm);
}

bool EnteringUsername::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode == 8) {
            std::string name = gameState.getUsername();
            if (!name.empty()) {
                name.pop_back();
                gameState.setUsername(name);
            }
        } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
            std::string name = gameState.getUsername();
            if (name.length() < 20) {
                name += static_cast<char>(event.text.unicode);
                gameState.setUsername(name);
            }
        }
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2u ws = window.getSize();
        float mx = static_cast<float>(event.mouseButton.x);
        float my = static_cast<float>(event.mouseButton.y);
        
        int lobbyId = gameState.getSelectedLobby();
        std::vector<int> taken;
        if (lobbyId != -1) {
            for (const auto& lobby : gameState.getLobbies()) {
                if (lobby.lobbyId == lobbyId) {
                    for (int i = 0; i < lobby.playerCount; i++) {
                        taken.push_back(lobby.players[i].colorId);
                    }
                    break;
                }
            }
        }
        
        float csx = (ws.x - (3 * 50.0f + 2 * 10.0f)) / 2.0f; // position x centrée de la grille: (largeur fenêtre - (3 cases * taille + 2 espacements)) / 2
        float cy = (ws.y - 500.0f) / 2.0f; // position y centrée du conteneur: (hauteur fenêtre - hauteur conteneur) / 2
        float csy = cy + 230.0f; // position y de la grille: début conteneur + offset vertical
        
        for (int i = 0; i < 9; i++) {
            float x = csx + (i % 3) * (50.0f + 10.0f); // position x: début grille + (colonne * (taille case + espacement))
            float y = csy + (i / 3) * (50.0f + 10.0f); // position y: début grille + (ligne * (taille case + espacement))
            
            bool isTaken = std::find(taken.begin(), taken.end(), i) != taken.end();
            if (mx >= x && mx <= x + 50.0f && my >= y && my <= y + 50.0f && !isTaken) {
                gameState.setSelectedColor(i);
                return false;
            }
        }
    }

    if (event.type == sf::Event::KeyPressed && 
        (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Enter)) {
        if (!gameState.getUsername().empty() && gameState.getSelectedColor() != -1) {
            gameState.setState(ClientState::WAITING_FOR_RESPONSE);
        }
    }
    return false;
}
