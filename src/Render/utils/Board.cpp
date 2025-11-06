#include "Render/utils/Board.hpp"
#include "GameState.hpp"

namespace BoardRenderer {
    void draw(sf::RenderWindow& window, const Board& board, float x, float y, float cellSize) {
        int size = board.getSize();
        for (int i = 0; i < size; ++i) { // parcourt la grille
            for (int j = 0; j < size; ++j) {
                float cellX = x + j * cellSize; // définit la position x de la cellule
                float cellY = y + i * cellSize; // définit la position y de la cellule
                
                sf::RectangleShape cell(sf::Vector2f(cellSize - 2, cellSize - 2));
                cell.setPosition(cellX + 1, cellY + 1); // définit la position de la cellule
                
                int cellValue = board.getCellValue(i, j);
                if (cellValue == -1) { // si la cellule est vide
                    cell.setFillColor(sf::Color::White);
                } else if (cellValue == 99) { // si la cellule est une pierre (STONE)
                    cell.setFillColor(sf::Color(128, 128, 128));
                } else if (cellValue == 100) { // BONUS_EXCHANGE
                    cell.setFillColor(sf::Color(255, 215, 0)); // Or
                } else if (cellValue == 101) { // BONUS_STONE
                    cell.setFillColor(sf::Color(64, 64, 64)); // Gris foncé
                } else if (cellValue == 102) { // BONUS_ROBBERY
                    cell.setFillColor(sf::Color(255, 0, 127)); // Rose foncé
                } else if (cellValue >= 0 && cellValue < 9) { // si la cellule est remplie avec une couleur (GRASS)
                    cell.setFillColor(GameState::PLAYERS_COLORS[cellValue]);
                } else {
                    cell.setFillColor(sf::Color::White); // si la cellule est vide
                }
                
                cell.setOutlineColor(sf::Color::Black); // définit la couleur de l'outline de la cellule
                cell.setOutlineThickness(1); // définit l'épaisseur de l'outline de la cellule
                
                window.draw(cell); // dessine la cellule
            }
        }
    }
    
    bool handleClick(float mouseX, float mouseY, float boardX, float boardY, float cellSize, int boardSize, int& outRow, int& outCol) {
        if (mouseX < boardX || mouseY < boardY) { // si la souris n'est pas sur le board
            return false;
        }
        
        float relativeX = mouseX - boardX; // définit la position x relative de la souris
        float relativeY = mouseY - boardY; // définit la position y relative de la souris
        
        int col = static_cast<int>(relativeX / cellSize); // définit la colonne de la cellule cliquée
        int row = static_cast<int>(relativeY / cellSize); // définit la ligne de la cellule cliquée
        
        if (row >= 0 && row < boardSize && col >= 0 && col < boardSize) {
            outRow = row; // définit la ligne de la cellule cliquée
            outCol = col; // définit la colonne de la cellule cliquée
            return true;
        }
        
        return false; // si la cellule cliquée n'est pas sur le board
    }
}

