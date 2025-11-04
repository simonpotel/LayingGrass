#pragma once
#include <SFML/Graphics.hpp>
#include <string>

namespace Tooltip {
    void draw(sf::RenderWindow& window, const std::string& text, float x, float y);
}

