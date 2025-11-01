#pragma once
#include <SFML/Graphics.hpp>
#include <string>

namespace Text {
    void draw(sf::RenderWindow& window, const std::string& text, float x, float y, unsigned int size = 24); // affiche un texte
    void drawBox(sf::RenderWindow& window, float x, float y, float width, float height, sf::Color fillColor, sf::Color outlineColor); // affiche un rectangle
    sf::Text createText(const std::string& text, unsigned int size = 24); // crée un texte
}
