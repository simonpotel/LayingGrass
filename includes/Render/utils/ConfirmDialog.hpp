#pragma once
#include <SFML/Graphics.hpp>
#include <string>

namespace ConfirmDialog {
    void draw(sf::RenderWindow& window, const std::string& message);
    bool isVisible();
    void show(const std::string& message);
    void hide();
    bool handleInput(sf::Event& event); // retourne true si confirmé, false si annulé
}

