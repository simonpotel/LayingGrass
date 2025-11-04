#include "Render/utils/Tooltip.hpp"
#include "Render/utils/Text.hpp"

namespace Tooltip {
    void draw(sf::RenderWindow& window, const std::string& text, float x, float y) {
        const float padding = 8.0f; 
        const float textSize = 16.0f;
        
        sf::Text sfText = Text::createText(text, textSize); //crée un texte à partir du texte et de la taille de texte
        sf::FloatRect textBounds = sfText.getLocalBounds();
        
        float tooltipWidth = textBounds.width + padding * 2; //calcule la largeur de la bulle d'aide
        float tooltipHeight = textBounds.height + padding * 2; //calcule la hauteur de la bulle d'aide
        
        float tooltipX = x - tooltipWidth / 2.0f; //calcule la position x de la bulle d'aide
        float tooltipY = y - tooltipHeight - 10.0f; //calcule la position y de la bulle d'aide
        
        if (tooltipX < 0) {
            tooltipX = 5.0f; //définit la position x de la bulle d'aide si elle est trop à gauche
        } else if (tooltipX + tooltipWidth > window.getSize().x) {
            tooltipX = window.getSize().x - tooltipWidth - 5.0f; //définit la position x de la bulle d'aide si elle est trop à droite
        }
        
        if (tooltipY < 0) {
            tooltipY = y + 10.0f; //définit la position y de la bulle d'aide si elle est trop en haut
        }
        
        Text::drawBox(window, tooltipX, tooltipY, tooltipWidth, tooltipHeight, sf::Color(40, 40, 40, 240), sf::Color::White); //affiche la bulle d'aide
        
        sfText.setPosition(tooltipX + padding - textBounds.left, tooltipY + padding - textBounds.top); //définit la position du texte dans la bulle d'aide
        window.draw(sfText);
    }
}

