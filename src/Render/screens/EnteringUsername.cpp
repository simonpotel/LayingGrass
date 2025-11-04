#include "Render/screens/EnteringUsername.hpp"
#include "Render/utils/Text.hpp"
#include "Render/utils/Tooltip.hpp"
#include "GameState.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>

static const float COLOR_SIZE = 50.0f;
static const float COLOR_SPACING = 10.0f;
static const float COLOR_START_X = 250.0f;
static const float COLOR_START_Y = 360.0f;

void EnteringUsername::draw(sf::RenderWindow& window, GameState& gameState) {
    Text::draw(window, "Enter your username:", 250, 150, 30);

    std::string displayName = gameState.getUsername();
    if (displayName.empty()) {
        displayName = "_";
    }

    Text::drawBox(window, 200, 190, 400, 50, sf::Color(40, 40, 40), sf::Color::White); //affiche le box
    Text::draw(window, displayName, 220, 200, 24);

    Text::draw(window, "Select a color:", 250, 320, 24); //affiche le texte

    int selectedLobbyId = gameState.getSelectedLobby(); //récupère l'id du lobby sélectionné
    std::vector<int> takenColors;
    std::unordered_map<int, std::string> colorToPlayerName;
    if (selectedLobbyId != -1) {
        const auto& lobbies = gameState.getLobbies(); //récupère la liste des lobbies
        for (const auto& lobby : lobbies) {
            if (lobby.lobbyId == selectedLobbyId) {
                for (int i = 0; i < lobby.playerCount; i++) {
                    takenColors.push_back(lobby.players[i].colorId); //ajoute la couleur prise par le joueur
                    colorToPlayerName[lobby.players[i].colorId] = lobby.players[i].playerName;
                }
                break;
            }
        }
    }

    sf::Vector2i mousePos = sf::Mouse::getPosition(window); //récupère la position de la souris
    int hoveredColorIndex = -1;

    for (int i = 0; i < 9; i++) {
        float x = COLOR_START_X + (i % 3) * (COLOR_SIZE + COLOR_SPACING); //calcule la position x de la couleur
        float y = COLOR_START_Y + (i / 3) * (COLOR_SIZE + COLOR_SPACING); //calcule la position y de la couleur
        
        bool isTaken = false; //vérifie si la couleur est prise
        for (int takenColor : takenColors) {
            if (takenColor == i) {
                isTaken = true;
                break;
            }
        }
        
        bool isSelected = gameState.getSelectedColor() == i; //vérifie si la couleur est sélectionnée
        
        if (mousePos.x >= x && mousePos.x <= x + COLOR_SIZE && 
            mousePos.y >= y && mousePos.y <= y + COLOR_SIZE) {
            hoveredColorIndex = i; //définit l'index de la couleur survolée
        }
        
        sf::Color color = GameState::PLAYERS_COLORS[i]; //récupère la couleur du joueur
        if (isTaken) {
            color = sf::Color(color.r / 2, color.g / 2, color.b / 2);
        }
        
        sf::Color outlineColor = isSelected ? sf::Color::White : sf::Color(100, 100, 100);
        if (hoveredColorIndex == i && !isTaken) {
            outlineColor = sf::Color(200, 200, 200);
        }
        
        Text::drawBox(window, x, y, COLOR_SIZE, COLOR_SIZE, color, outlineColor);
    }

    if (hoveredColorIndex != -1) { //vérifie si une couleur est survolée
        auto it = colorToPlayerName.find(hoveredColorIndex); //cherche le nom du joueur qui a la couleur survolée
        if (it != colorToPlayerName.end()) {
            float x = COLOR_START_X + (hoveredColorIndex % 3) * (COLOR_SIZE + COLOR_SPACING) + COLOR_SIZE / 2.0f; //calcule la position x de la couleur
            float y = COLOR_START_Y + (hoveredColorIndex / 3) * (COLOR_SIZE + COLOR_SPACING) + COLOR_SIZE / 2.0f; //calcule la position y de la couleur
            std::string tooltipText = "Player \"" + it->second + "\" already has this color !"; //crée le texte de la bulle d'aide
            Tooltip::draw(window, tooltipText, x, y);
        }
    }

    Text::draw(window, "Press Enter to confirm", 250, 520, 20); //affiche le texte
}

bool EnteringUsername::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    if (event.type == sf::Event::TextEntered) { //vérifie si un texte est entré
        if (event.text.unicode == 8) {
            std::string currentName = gameState.getUsername(); //récupère le nom d'utilisateur
            if (!currentName.empty()) {
                currentName.pop_back(); //supprime le dernier caractère du nom d'utilisateur
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

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) { //vérifie si un bouton de la souris est pressé
        float mouseX = static_cast<float>(event.mouseButton.x); //récupère la position x de la souris
        float mouseY = static_cast<float>(event.mouseButton.y); //récupère la position y de la souris
        
        int selectedLobbyId = gameState.getSelectedLobby(); //récupère l'id du lobby sélectionné
        std::vector<int> takenColors;
        if (selectedLobbyId != -1) {
            const auto& lobbies = gameState.getLobbies(); //récupère la liste des lobbies
            for (const auto& lobby : lobbies) {
                if (lobby.lobbyId == selectedLobbyId) {
                    for (int i = 0; i < lobby.playerCount; i++) {
                        takenColors.push_back(lobby.players[i].colorId); //ajoute la couleur prise par le joueur
                    }
                    break;
                }
            }
        }
        
        for (int i = 0; i < 9; i++) {
            float x = COLOR_START_X + (i % 3) * (COLOR_SIZE + COLOR_SPACING); //calcule la position x de la couleur
            float y = COLOR_START_Y + (i / 3) * (COLOR_SIZE + COLOR_SPACING); //calcule la position y de la couleur
            
            bool isTaken = false; //vérifie si la couleur est prise
            for (int takenColor : takenColors) {
                if (takenColor == i) {
                    isTaken = true;
                    break;
                }
            }
            
            if (mouseX >= x && mouseX <= x + COLOR_SIZE && mouseY >= y && mouseY <= y + COLOR_SIZE && !isTaken) {
                gameState.setSelectedColor(i); //définit la couleur sélectionnée
                return false;
            }
        }
    }

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) { //vérifie si une touche est pressée
        if (!gameState.getUsername().empty() && gameState.getSelectedColor() != -1) {
            gameState.setState(ClientState::WAITING_FOR_RESPONSE); //passe à l'état d'attente de la réponse
        }
    }
    return false;
}
