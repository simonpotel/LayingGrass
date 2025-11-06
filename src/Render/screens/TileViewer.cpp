#include "Render/screens/TileViewer.hpp"
#include "Render/utils/Text.hpp"
#include "Render/utils/TileRenderer.hpp"
#include "Game/Tile.hpp"
#include <sstream>
#include <cmath>
#include <algorithm>

void TileViewer::draw(sf::RenderWindow& window, GameState& gameState) {
    Text::draw(window, "Visualisation de toutes les tuiles (96 total)", 50, 20, 24);
    Text::draw(window, "Appuyez sur ECHAP pour retourner", 50, 50, 18);
    
    const int TILES_PER_ROW = 12;
    const float TILE_CELL_SIZE = 10.0f;
    const float TILE_SPACING_X = 55.0f;
    const float TILE_SPACING_Y = 65.0f;
    const float START_X = 30.0f;
    const float START_Y = 85.0f;
    
    for (int tileId = 0; tileId < static_cast<int>(TileId::TOTAL_TILES); ++tileId) {
        const Tile& tile = Tile::getTile(Tile::fromInt(tileId));
        
        if (!tile.isValid()) {
            continue;
        }
        
        int row = tileId / TILES_PER_ROW;
        int col = tileId % TILES_PER_ROW;
        
        float tileX = START_X + col * TILE_SPACING_X;
        float tileY = START_Y + row * TILE_SPACING_Y;
        
        TileRenderer::draw(window, tile, tileX, tileY, TILE_CELL_SIZE, 0);
        
        std::stringstream ss;
        ss << "#" << tileId;
        float maxHeight = 0;
        for (const auto& [blockRow, blockCol] : tile.blocks) {
            maxHeight = std::max(maxHeight, (float)(blockRow + 1) * TILE_CELL_SIZE);
        }
        Text::draw(window, ss.str(), tileX, tileY + maxHeight + 3, 10);
    }
    
    std::stringstream ss;
    ss << "Total: " << static_cast<int>(TileId::TOTAL_TILES) << " tuiles | Appuyez sur ECHAP pour retourner";
    Text::draw(window, ss.str(), 50, 580, 16);
}

bool TileViewer::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            // Retourne à l'écran précédent (IN_GAME)
            gameState.setState(ClientState::IN_GAME);
        }
    }
    return false;
}

