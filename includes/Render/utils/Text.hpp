#pragma once
#include <SFML/Graphics.hpp>
#include <string>

namespace Text {
    void draw(sf::RenderWindow& window, const std::string& text, float x, float y, unsigned int size = 24);
    void drawBox(sf::RenderWindow& window, float x, float y, float width, float height, sf::Color fillColor, sf::Color outlineColor);
}
