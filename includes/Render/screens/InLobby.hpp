#pragma once
#include <SFML/Graphics.hpp>
#include "Render.hpp"

class InLobby : public Render {
public:
    void draw(sf::RenderWindow& window, GameState& gameState) override;
    bool handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) override;
};
