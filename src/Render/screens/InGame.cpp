#include "Render/screens/InGame.hpp"
#include "Render/utils/Text.hpp"
#include "Render/utils/Board.hpp"
#include "Render/utils/TileRenderer.hpp"
#include "Render/utils/Theme.hpp"
#include "Render/utils/Element.hpp"
#include "Game/Tile.hpp"
#include "Game/PlacementRules.hpp"
#include "Client.hpp"
#include <sstream>
#include <algorithm>

extern Client* g_client;

namespace {
    int s_hoverRow = -1; // ligne du plateau survolée par la souris
    int s_hoverCol = -1; // colonne du plateau survolée par la souris
}

void InGame::draw(sf::RenderWindow& window, GameState& gameState) {
    sf::Vector2u ws = window.getSize();
    
    int lobbyId = gameState.getCurrentLobby();
    const auto& board = gameState.getBoard();
    int bs = board.getSize();
    float boardSize = bs * 18.0f; // taille totale du plateau: nombre de cellules * taille d'une cellule
    
    float centerX = ws.x / 2.0f; // centre horizontal de la fenêtre
    float bx = centerX - boardSize / 2.0f; // position x du plateau: centre - moitié de la taille du plateau
    float by = 40.0f + 80.0f; // position y du plateau: titre + espacement
    
    // panneau info gauche
    float ix = bx - 250.0f - 30.0f; // position x: position plateau - largeur panneau - espacement
    float iy = by; // position y: aligné avec le plateau
    
    sf::RectangleShape info(sf::Vector2f(250.0f, 200.0f));
    info.setPosition(ix, iy);
    info.setFillColor(sf::Color(30, 30, 30));
    info.setOutlineColor(sf::Color::White);
    info.setOutlineThickness(2);
    window.draw(info);
    
    std::stringstream ss;
    ss << "Lobby #" << lobbyId;
    sf::Text lobby = Text::createText(ss.str(), 22);
    lobby.setPosition(ix + 20.0f, iy + 20.0f);
    lobby.setFillColor(sf::Color::White);
    window.draw(lobby);
    
    ss.str("");
    ss << "Tour: " << gameState.getTurnCount() << " / 9";
    sf::Text turn = Text::createText(ss.str(), 22);
    turn.setPosition(ix + 20.0f, iy + 20.0f + 35);
    turn.setFillColor(sf::Color::White);
    window.draw(turn);
    
    if (gameState.isGameOver()) {
        ss.str("");
        ss << (gameState.getWinnerId() >= 0 ? "Winner: Color " + std::to_string(gameState.getWinnerId()) : "Game Over");
        sf::Text over = Text::createText(ss.str(), 20);
        over.setPosition(ix + 20.0f, iy + 20.0f + 70);
        over.setFillColor(sf::Color::White);
        window.draw(over);
    } else {
        int turnId = gameState.getCurrentTurnColorId();
        int myId = gameState.getSelectedColor();
        sf::Text turnInfo = Text::createText(turnId == myId ? "Your turn!" : "Opponent's turn", 20);
        turnInfo.setPosition(ix + 20.0f, iy + 20.0f + 70);
        turnInfo.setFillColor(turnId == myId ? Theme::FOREST_GREEN : sf::Color::White);
        window.draw(turnInfo);
    }
    
    // panneau tuile droite
    float tx = bx + boardSize + 30.0f; // position x: position plateau + taille plateau + espacement
    float ty = by; // position y: aligné avec le plateau
    
    int turnId = gameState.getCurrentTurnColorId();
    int myId = gameState.getSelectedColor();
    
    if (!gameState.isGameOver() && turnId == myId) {
        int tileId = gameState.getCurrentPlayerTileId();
        if (tileId >= 0 && tileId < static_cast<int>(TileId::TOTAL_TILES)) {
            const Tile& tile = Tile::getTile(Tile::fromInt(tileId));
            if (tile.isValid()) {
                // calcul des dimensions de la tuile (min/max des coordonnées)
                int minR = 0, maxR = 0, minC = 0, maxC = 0;
                for (const auto& [r, c] : tile.blocks) {
                    minR = std::min(minR, r); // ligne minimale
                    maxR = std::max(maxR, r); // ligne maximale
                    minC = std::min(minC, c); // colonne minimale
                    maxC = std::max(maxC, c); // colonne maximale
                }
                int tw = maxC - minC + 1; // largeur de la tuile en cellules
                int th = maxR - minR + 1; // hauteur de la tuile en cellules
                
                float aw = 280.0f - 2 * 20.0f; // largeur disponible: largeur panneau - 2 * padding
                float ah = 350.0f - 2 * 20.0f - 40.0f; // hauteur disponible: hauteur panneau - 2 * padding - espace pour le label
                float csX = aw / static_cast<float>(tw); // taille de cellule en x pour remplir la largeur
                float csY = ah / static_cast<float>(th); // taille de cellule en y pour remplir la hauteur
                float cs = std::min({csX, csY, 25.0f}); // taille finale: minimum entre x, y et maximum autorisé
                
                sf::RectangleShape tilePanel(sf::Vector2f(280.0f, 350.0f));
                tilePanel.setPosition(tx, ty);
                tilePanel.setFillColor(sf::Color(30, 30, 30));
                tilePanel.setOutlineColor(sf::Color::White);
                tilePanel.setOutlineThickness(2);
                window.draw(tilePanel);
                
                ss.str("");
                ss << "Your tile: #" << tileId;
                sf::Text label = Text::createText(ss.str(), 20);
                label.setPosition(tx + 20.0f, ty + 20.0f);
                label.setFillColor(sf::Color::White);
                window.draw(label);
                
                float tileW = tw * cs; // largeur de la tuile affichée: largeur en cellules * taille cellule
                float tileH = th * cs; // hauteur de la tuile affichée: hauteur en cellules * taille cellule
                float tileX = tx + (280.0f - tileW) / 2.0f; // position x centrée: début panneau + (largeur panneau - largeur tuile) / 2
                float tileY = ty + 20.0f + 40.0f + (ah - tileH) / 2.0f; // position y centrée: début panneau + padding + label + (hauteur disponible - hauteur tuile) / 2
                tileX -= minC * cs; // ajustement pour aligner la tuile: soustraire la colonne minimale * taille cellule
                tileY -= minR * cs; // ajustement pour aligner la tuile: soustraire la ligne minimale * taille cellule
                
                TileRenderer::draw(window, tile, tileX, tileY, cs, myId);
            }
        }
    }
    
    // plateau
    BoardRenderer::draw(window, board, bx, by, 18.0f);
    
    // prévisualisation
    if (!gameState.isGameOver() && turnId == myId) {
        int tileId = gameState.getCurrentPlayerTileId();
        if (tileId >= 0 && tileId < static_cast<int>(TileId::TOTAL_TILES)) {
            const Tile& tile = Tile::getTile(Tile::fromInt(tileId));
            if (tile.isValid() && s_hoverRow >= 0 && s_hoverCol >= 0) {
                bool firstTurn = !PlacementRules::playerHasCells(board, myId);
                bool canPlace = PlacementRules::canPlaceTile(board, tile, s_hoverRow, s_hoverCol, myId, firstTurn);
                BoardRenderer::drawPreview(window, tile, bx, by, 18.0f, bs, s_hoverRow, s_hoverCol, canPlace, GameState::PLAYERS_COLORS[myId]);
            }
        }
    }
    
    // aide
    sf::Text help = Text::createText("Press Tab to view all tiles", 16);
    Element::centerH(help, static_cast<float>(ws.x), static_cast<float>(ws.y) - 30);
    help.setFillColor(sf::Color(150, 150, 150));
    window.draw(help);
}

bool InGame::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
        gameState.setState(ClientState::VIEWING_TILES);
        return false;
    }
    
    sf::Vector2u ws = window.getSize();
    float centerX = ws.x / 2.0f; // centre horizontal de la fenêtre
    int bs = gameState.getBoard().getSize();
    float boardSize = bs * 18.0f; // taille totale du plateau: nombre de cellules * taille d'une cellule
    float bx = centerX - boardSize / 2.0f; // position x du plateau: centre - moitié de la taille du plateau
    float by = 40.0f + 80.0f; // position y du plateau: titre + espacement
    
    if (event.type == sf::Event::MouseMoved) {
        int row, col;
        if (BoardRenderer::handleClick(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y), bx, by, 18.0f, bs, row, col)) {
            s_hoverRow = row;
            s_hoverCol = col;
        } else {
            s_hoverRow = -1;
            s_hoverCol = -1;
        }
    }
    
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (gameState.isGameOver()) return false;
        
        int turnId = gameState.getCurrentTurnColorId();
        int myId = gameState.getSelectedColor();
        if (turnId != myId) return false;
        
        int row, col;
        if (BoardRenderer::handleClick(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y), bx, by, 18.0f, bs, row, col)) {
            if (g_client) {
                g_client->sendCellClick(gameState.getCurrentLobby(), row, col);
            }
        }
    }
    return false;
}
