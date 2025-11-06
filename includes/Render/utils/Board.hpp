#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

namespace Board {
    void draw(sf::RenderWindow& window, const std::vector<std::vector<int>>& grid, int size, float x, float y, float cellSize);
    bool handleClick(float mouseX, float mouseY, float boardX, float boardY, float cellSize, int boardSize, int& outRow, int& outCol);
}

