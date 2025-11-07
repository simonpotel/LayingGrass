#include "Render/screens/TileViewer.hpp"
#include "Render/utils/Text.hpp"
#include "Render/utils/TileRenderer.hpp"
#include "Render/utils/Theme.hpp"
#include "Render/utils/Element.hpp"
#include "Game/Tile.hpp"
#include <sstream>
#include <algorithm>

static const int TOTAL_TILES = 96; // nombre total de tuiles
static const int TILES_PER_ROW = 12; // nombre de tuiles par ligne
static const int TOTAL_ROWS = (TOTAL_TILES + TILES_PER_ROW - 1) / TILES_PER_ROW; // nombre de lignes nécessaires

void TileViewer::draw(sf::RenderWindow& window, GameState& gameState) {
    sf::Vector2u ws = window.getSize();
    
    // calcul de la hauteur disponible (720px - titre - espace bas)
    float availableHeight = 720.0f - (40.0f + 60.0f + 20.0f) - 50.0f; // hauteur fenêtre - (position titre + hauteur titre + espace) - marge bas
    float tileSpacingY = availableHeight / static_cast<float>(TOTAL_ROWS); // espacement vertical: hauteur disponible / nombre de lignes
    
    // conteneur (descendu pour ne pas écraser le titre)
    float cx = (ws.x - 1000.0f) / 2.0f; // position x centrée: (largeur fenêtre - largeur conteneur) / 2
    float cy = 40.0f + 80.0f; // position y: titre + espacement
    float containerHeight = TOTAL_ROWS * tileSpacingY; // hauteur du conteneur: nombre de lignes * espacement vertical
    
    sf::RectangleShape container(sf::Vector2f(1000.0f, containerHeight));
    container.setPosition(cx, cy);
    container.setFillColor(sf::Color(30, 30, 30));
    container.setOutlineColor(sf::Color::White);
    container.setOutlineThickness(2);
    window.draw(container);
    
    // calcul de l'espacement horizontal réduit
    float tileSpacingX = 1000.0f / static_cast<float>(TILES_PER_ROW); // espacement horizontal: largeur conteneur / nombre de tuiles par ligne
    float startX = cx; // position x de départ: début du conteneur
    
    // calculer la hauteur maximale d'une tuile pour ajuster le padding
    float maxTileHeight = 0;
    for (int i = 0; i < static_cast<int>(TileId::TOTAL_TILES); ++i) {
        const Tile& tile = Tile::getTile(Tile::fromInt(i));
        if (!tile.isValid()) continue;
        for (const auto& [r, c] : tile.blocks) {
            maxTileHeight = std::max(maxTileHeight, (float)(r + 1) * 10.0f); // hauteur maximale: (ligne + 1) * taille cellule
        }
    }
    
    // ajouter un petit padding en haut pour que les tuiles ne dépassent pas
    float startY = cy + maxTileHeight / 2.0f + 5.0f; // position y de départ: début conteneur + moitié hauteur max + marge
    
    // tuiles
    for (int tileId = 0; tileId < static_cast<int>(TileId::TOTAL_TILES); ++tileId) {
        const Tile& tile = Tile::getTile(Tile::fromInt(tileId));
        if (!tile.isValid()) continue;
        
        int row = tileId / TILES_PER_ROW; // ligne de la tuile: index / nombre de tuiles par ligne
        int col = tileId % TILES_PER_ROW; // colonne de la tuile: index % nombre de tuiles par ligne
        float tx = startX + col * tileSpacingX + tileSpacingX / 2.0f; // position x: début + (colonne * espacement) + moitié espacement (centré dans la colonne)
        float ty = startY + row * tileSpacingY; // position y: début + (ligne * espacement vertical)
        
        // centrer la tuile dans sa colonne
        float maxW = 0;
        for (const auto& [r, c] : tile.blocks) {
            maxW = std::max(maxW, (float)(c + 1) * 10.0f); // largeur maximale: (colonne + 1) * taille cellule
        }
        tx -= maxW / 2.0f; // ajustement pour centrer: soustraire la moitié de la largeur
        
        TileRenderer::draw(window, tile, tx, ty, 10.0f, 0);
        
        std::stringstream ts;
        ts << "#" << tileId;
        float maxH = 0;
        for (const auto& [r, c] : tile.blocks) {
            maxH = std::max(maxH, (float)(r + 1) * 10.0f);
        }
        sf::Text id = Text::createText(ts.str(), 10);
        id.setPosition(tx, ty + maxH + 3);
        id.setFillColor(sf::Color::White);
        window.draw(id);
    }
    
    // aide
    sf::Text help = Text::createText("Press ESC to return", 18);
    Element::centerH(help, static_cast<float>(ws.x), cy + containerHeight + 20.0f);
    help.setFillColor(sf::Color::White);
    window.draw(help);
}

bool TileViewer::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        gameState.setState(ClientState::IN_GAME);
    }
    return false;
}
