#include "Render/utils/ConfirmDialog.hpp"
#include "Render/utils/Text.hpp"
#include "Render/utils/Element.hpp"
#include <vector>
#include <string>

namespace ConfirmDialog {
    static bool s_visible = false;
    static std::string s_message = "";
    
    void show(const std::string& message) {
        s_message = message;
        s_visible = true;
    }
    
    void hide() {
        s_visible = false;
        s_message = "";
    }
    
    bool isVisible() {
        return s_visible;
    }
    
    void draw(sf::RenderWindow& window, const std::string& message) {
        if (!s_visible) {
            return;
        }
        
        // utilise le message stocké si aucun message n'est fourni
        const std::string& displayMessage = message.empty() ? s_message : message;
        
        sf::Vector2u ws = window.getSize();
        float centerX = ws.x / 2.0f;
        float centerY = ws.y / 2.0f;
        
        // fond semi-transparent
        sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(ws.x), static_cast<float>(ws.y)));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        overlay.setPosition(0, 0);
        window.draw(overlay);
        
        // panneau de dialogue
        float panelWidth = 500.0f;
        float panelHeight = 200.0f;
        float panelX = centerX - panelWidth / 2.0f;
        float panelY = centerY - panelHeight / 2.0f;
        
        sf::RectangleShape panel(sf::Vector2f(panelWidth, panelHeight));
        panel.setPosition(panelX, panelY);
        panel.setFillColor(sf::Color(40, 40, 40));
        panel.setOutlineColor(sf::Color::White);
        panel.setOutlineThickness(3);
        window.draw(panel);
        
        // message (gère les retours à la ligne)
        std::vector<std::string> lines;
        std::string currentLine = "";
        for (char c : displayMessage) {
            if (c == '\n') {
                if (!currentLine.empty()) {
                    lines.push_back(currentLine);
                    currentLine = "";
                }
            } else {
                currentLine += c;
            }
        }
        if (!currentLine.empty()) {
            lines.push_back(currentLine);
        }
        
        float lineHeight = 25.0f;
        float startY = panelY + 30.0f;
        for (size_t i = 0; i < lines.size(); ++i) {
            sf::Text lineText = Text::createText(lines[i], 18);
            lineText.setFillColor(sf::Color::White);
            Element::centerH(lineText, ws.x, startY + i * lineHeight);
            window.draw(lineText);
        }
        
        // instructions
        sf::Text instruction = Text::createText("Press ENTER to confirm, ESC to cancel", 16);
        instruction.setFillColor(sf::Color(200, 200, 200));
        Element::centerH(instruction, ws.x, panelY + panelHeight - 40.0f);
        window.draw(instruction);
    }
    
    bool handleInput(sf::Event& event) {
        if (!s_visible) {
            return false;
        }
        
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Enter) {
                hide();
                return true; // confirmé
            } else if (event.key.code == sf::Keyboard::Escape) {
                hide();
                return false; // annulé
            }
        }
        
        return false;
    }
}

