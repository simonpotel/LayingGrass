#pragma once
#include <SFML/Graphics.hpp>
#include "Game/Board.hpp"
#include "GameState.hpp"

namespace BoardRenderer {
    void draw(sf::RenderWindow& window, const Board& board, float x, float y, float cellSize);
    bool handleClick(float mouseX, float mouseY, float boardX, float boardY, float cellSize, int boardSize, int& outRow, int& outCol);
}

