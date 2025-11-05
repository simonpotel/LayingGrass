#include "Render/utils/TileRenderer.hpp"
#include "GameState.hpp"

namespace TileRenderer {
    void draw(sf::RenderWindow& window, const Tile& tile, float x, float y, float cellSize, int playerColorId) {
        if (!tile.isValid() || playerColorId < 0 || playerColorId >= 9) {
            return;
        }
        
        // Récupère la couleur du joueur
        sf::Color tileColor = GameState::PLAYERS_COLORS[playerColorId];
        
        // Dessine chaque bloc de la tuile
        for (const auto& [blockRow, blockCol] : tile.blocks) {
            float cellX = x + blockCol * cellSize;
            float cellY = y + blockRow * cellSize;
            
            sf::RectangleShape cell(sf::Vector2f(cellSize - 2, cellSize - 2));
            cell.setPosition(cellX + 1, cellY + 1);
            cell.setFillColor(tileColor);
            cell.setOutlineColor(sf::Color::Black);
            cell.setOutlineThickness(1);
            
            window.draw(cell);
        }
    }
}

