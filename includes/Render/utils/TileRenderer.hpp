#pragma once
#include <SFML/Graphics.hpp>
#include "Game/Tile.hpp"

namespace TileRenderer {
    // Dessine une tuile à une position donnée
    void draw(sf::RenderWindow& window, const Tile& tile, float x, float y, float cellSize, int playerColorId);
}

