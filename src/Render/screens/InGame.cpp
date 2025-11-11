#include "Render/screens/InGame.hpp"
#include "Render/utils/Text.hpp"
#include "Render/utils/Board.hpp"
#include "Render/utils/TileRenderer.hpp"
#include "Render/utils/Theme.hpp"
#include "Render/utils/Element.hpp"
#include "Render/utils/Tooltip.hpp"
#include "Render/utils/ConfirmDialog.hpp"
#include "Render/utils/Button.hpp"
#include "Game/Tile.hpp"
#include "Game/PlacementRules.hpp"
#include "Game/Cell.hpp"
#include "Client.hpp"
#include <sstream>
#include <algorithm>
#include <memory>

extern Client* g_client;

namespace {
    int s_hoverRow = -1; // ligne du plateau survolée par la souris
    int s_hoverCol = -1; // colonne du plateau survolée par la souris
    bool s_selectingCoupon = false; // true si l'on choisit une tuile à l'aide d'un coupon
    int s_pendingRow = -1; // ligne du placement en attente de confirmation
    int s_pendingCol = -1; // colonne du placement en attente de confirmation
    int s_pendingRotation = 0; // rotation du placement en attente
    bool s_pendingFlippedH = false; // flip horizontal du placement en attente
    bool s_pendingFlippedV = false; // flip vertical du placement en attente
    std::unique_ptr<Button> s_discardButton; // bouton d'abandon de tuile
    
    // applique les transformations à une tuile
    Tile applyTileTransform(const Tile& baseTile, int rotation, bool flippedH, bool flippedV) {
        Tile result = baseTile;
        
        // applique les rotations (0-3 fois 90°)
        for (int i = 0; i < rotation; ++i) {
            result = result.rotate90();
        }
        
        // applique le flip horizontal si nécessaire
        if (flippedH) {
            result = result.flipHorizontal();
        }
        
        // applique le flip vertical si nécessaire
        if (flippedV) {
            result = result.flipVertical();
        }
        
        return result;
    }
}

void InGame::draw(sf::RenderWindow& window, GameState& gameState) {
    sf::Vector2u ws = window.getSize();

    if (gameState.isGameOver() || gameState.getExchangeCouponCount() <= 0) {
        s_selectingCoupon = false;
    }
    
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
    
    // déclare les variables pour le tour et le joueur
    int turnId = gameState.getCurrentTurnColorId();
    int myId = gameState.getSelectedColor();
    
    float infoHeight = 200.0f;
    
    sf::RectangleShape info(sf::Vector2f(250.0f, infoHeight));
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
        sf::Text turnInfo = Text::createText(turnId == myId ? "Your turn!" : "Opponent's turn", 20);
        turnInfo.setPosition(ix + 20.0f, iy + 20.0f + 70);
        turnInfo.setFillColor(turnId == myId ? Theme::FOREST_GREEN : sf::Color::White);
        window.draw(turnInfo);
    }
    
    ss.str("");
    ss << "Coupons: " << gameState.getExchangeCouponCount();
    sf::Text couponsText = Text::createText(ss.str(), 18);
    couponsText.setPosition(ix + 20.0f, iy + 20.0f + 105);
    couponsText.setFillColor(gameState.getExchangeCouponCount() > 0 ? Theme::FOREST_GREEN : sf::Color(180, 180, 180));
    window.draw(couponsText);
    
    // affiche les bonus en attente dans le panneau d'info
    if (!gameState.isGameOver() && turnId == myId) {
        if (gameState.hasPendingStoneBonus()) {
            sf::Text bonusText = Text::createText("STONE BONUS: Click empty cell", 16);
            bonusText.setPosition(ix + 20.0f, iy + 20.0f + 130);
            bonusText.setFillColor(sf::Color(255, 215, 0));
            bonusText.setStyle(sf::Text::Bold);
            window.draw(bonusText);
        } else if (gameState.hasPendingRobberyBonus()) {
            sf::Text bonusText = Text::createText("ROBBERY BONUS: Click opponent cell", 16);
            bonusText.setPosition(ix + 20.0f, iy + 20.0f + 130);
            bonusText.setFillColor(sf::Color(255, 69, 0));
            bonusText.setStyle(sf::Text::Bold);
            window.draw(bonusText);
        }
    }
    
    // affiche le bouton d'abandon de tuile si le placement est impossible
    if (!gameState.isGameOver() && turnId == myId && gameState.getCurrentPlayerTileId() >= 0 && 
        !gameState.canPlaceTile() && !gameState.hasPendingStoneBonus() && !gameState.hasPendingRobberyBonus()) {
        if (!s_discardButton) {
            s_discardButton = std::make_unique<Button>(ix + 20.0f, iy + infoHeight + 20.0f, 210.0f, 50.0f, 
                                                       "Abandon de tuile\n(si placement impossible)", 16);
        }
        s_discardButton->draw(window);
    } else {
        s_discardButton.reset();
    }

    if (gameState.isGameOver() && gameState.getWinnerId() < 0) {
        std::string promptText;
        if (gameState.getExchangeCouponCount() > 0) {
            promptText = "Press C: Place 1x1 tile or remove stone\nPress X: Remove stone\nPress N: Skip coupon";
        } else {
            promptText = "Waiting for other players to finish coupons...";
        }
        sf::Text prompt = Text::createText(promptText, 16);
        prompt.setPosition(ix + 20.0f, iy + 20.0f + 135);
        prompt.setFillColor(sf::Color(200, 200, 200));
        window.draw(prompt);
    }
    
    // panneau tuile droite
    float tx = bx + boardSize + 30.0f; // position x: position plateau + taille plateau + espacement
    float ty = by; // position y: aligné avec le plateau
    
    if (turnId != myId) {
        s_selectingCoupon = false;
    }
    const auto& upcomingTiles = gameState.getUpcomingTiles();
    bool hasUpcomingOptions = false;
    for (int tileId : upcomingTiles) {
        if (tileId >= 0) {
            hasUpcomingOptions = true;
            break;
        }
    }
    if (!hasUpcomingOptions) {
        s_selectingCoupon = false;
    }
    
    if (!gameState.isGameOver() && turnId == myId) {
        int tileId = gameState.getCurrentPlayerTileId();
        if (tileId >= 0 && tileId < static_cast<int>(TileId::TOTAL_TILES)) {
            const Tile& baseTile = Tile::getTile(Tile::fromInt(tileId));
            if (baseTile.isValid()) {
                // applique les transformations
                Tile tile = applyTileTransform(baseTile, gameState.getTileRotation(), gameState.getTileFlippedH(), gameState.getTileFlippedV());
                
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

    if (!gameState.isGameOver() && turnId == myId && gameState.getExchangeCouponCount() > 0 && hasUpcomingOptions) {
        sf::Text couponLabel = Text::createText("Coupons disponibles :", 18);
        couponLabel.setPosition(tx + 20.0f, ty + 20.0f + 360.0f);
        couponLabel.setFillColor(s_selectingCoupon ? Theme::FOREST_GREEN : sf::Color::White);
        window.draw(couponLabel);

        const float boxWidth = 62.0f;
        const float boxHeight = 70.0f;
        const float boxPadding = 8.0f;
        const float startX = tx + 20.0f;
        const float startY = ty + 20.0f + 390.0f;

        for (int i = 0; i < 5; ++i) {
            float boxX = startX + i * (boxWidth + 10.0f);
            float boxY = startY;

            bool tileValid = (upcomingTiles[i] >= 0 && upcomingTiles[i] < static_cast<int>(TileId::TOTAL_TILES));

            sf::RectangleShape box(sf::Vector2f(boxWidth, boxHeight));
            box.setPosition(boxX, boxY);
            box.setFillColor(sf::Color(35, 35, 35));
            if (tileValid) {
                box.setOutlineColor(s_selectingCoupon ? Theme::FOREST_GREEN : sf::Color(120, 120, 120));
            } else {
                box.setOutlineColor(sf::Color(80, 80, 80));
            }
            box.setOutlineThickness(2.0f);
            window.draw(box);

            sf::Text indexText = Text::createText(std::to_string(i + 1), 14);
            indexText.setFillColor(tileValid ? sf::Color::White : sf::Color(120, 120, 120));
            indexText.setPosition(boxX + 6.0f, boxY + 4.0f);
            window.draw(indexText);

            if (tileValid) {
                Tile tile = Tile::getTile(Tile::fromInt(upcomingTiles[i]));
                if (tile.isValid()) {
                    int minR = 0, maxR = 0, minC = 0, maxC = 0;
                    for (const auto& [r, c] : tile.blocks) {
                        minR = std::min(minR, r);
                        maxR = std::max(maxR, r);
                        minC = std::min(minC, c);
                        maxC = std::max(maxC, c);
                    }
                    int widthCells = maxC - minC + 1;
                    int heightCells = maxR - minR + 1;

                    float availWidth = boxWidth - 2.0f * boxPadding;
                    float availHeight = boxHeight - 2.0f * boxPadding - 16.0f; // espace pour l'index
                    float cellSize = std::min({availWidth / static_cast<float>(widthCells),
                                               availHeight / static_cast<float>(heightCells),
                                               14.0f});

                    float tileDrawWidth = widthCells * cellSize;
                    float tileDrawHeight = heightCells * cellSize;

                    float tileX = boxX + (boxWidth - tileDrawWidth) / 2.0f - minC * cellSize;
                    float tileY = boxY + boxHeight - boxPadding - tileDrawHeight - 4.0f - minR * cellSize;

                    TileRenderer::draw(window, tile, tileX, tileY, cellSize, myId);
                }
            }
        }
    }
    
    // plateau
    BoardRenderer::draw(window, board, bx, by, 18.0f);
    
    // prévisualisation
    if (!gameState.isGameOver()) {
        int previewColorId = gameState.getPreviewColorId();
        int previewRow = gameState.getPreviewRow();
        int previewCol = gameState.getPreviewCol();
        
        // affiche la prévisualisation si c'est le tour d'un autre joueur et que c'est bien son tour
        if (previewColorId >= 0 && previewColorId < 9 && previewRow >= 0 && previewCol >= 0 && 
            previewColorId != myId && previewColorId == turnId) {
            // utilise la tuile du joueur actif depuis le gameState
            int tileId = gameState.getCurrentPlayerTileId();
            if (tileId >= 0 && tileId < static_cast<int>(TileId::TOTAL_TILES)) {
                const Tile& baseTile = Tile::getTile(Tile::fromInt(tileId));
                if (baseTile.isValid()) {
                    // applique les transformations de la prévisualisation
                    Tile tile = applyTileTransform(baseTile, gameState.getPreviewRotation(), 
                                                   gameState.getPreviewFlippedH(), gameState.getPreviewFlippedV());
                    bool firstTurn = !PlacementRules::playerHasCells(board, previewColorId);
                    bool canPlace = PlacementRules::canPlaceTile(board, tile, previewRow, previewCol, previewColorId, firstTurn);
                    // utilise une couleur semi-transparente pour la prévisualisation des autres joueurs
                    sf::Color previewColor = GameState::PLAYERS_COLORS[previewColorId];
                    previewColor.a = 150; // semi-transparent
                    BoardRenderer::drawPreview(window, tile, bx, by, 18.0f, bs, previewRow, previewCol, canPlace, previewColor);
                }
            }
        }
        
        // prévisualisation locale si c'est mon tour
        if (turnId == myId) {
            int tileId = gameState.getCurrentPlayerTileId();
            if (tileId >= 0 && tileId < static_cast<int>(TileId::TOTAL_TILES)) {
                const Tile& baseTile = Tile::getTile(Tile::fromInt(tileId));
                if (baseTile.isValid() && s_hoverRow >= 0 && s_hoverCol >= 0) {
                    // applique les transformations pour la prévisualisation
                    Tile tile = applyTileTransform(baseTile, gameState.getTileRotation(), gameState.getTileFlippedH(), gameState.getTileFlippedV());
                    bool firstTurn = !PlacementRules::playerHasCells(board, myId);
                    bool canPlace = PlacementRules::canPlaceTile(board, tile, s_hoverRow, s_hoverCol, myId, firstTurn);
                    BoardRenderer::drawPreview(window, tile, bx, by, 18.0f, bs, s_hoverRow, s_hoverCol, canPlace, GameState::PLAYERS_COLORS[myId]);
                }
            }
        }
    }
    
    if (gameState.isGameOver() && gameState.getWinnerId() < 0 && gameState.getExchangeCouponCount() > 0 && myId >= 0) {
        if (s_hoverRow >= 0 && s_hoverCol >= 0) {
            const Board& board = gameState.getBoard();
            if (board.isValidPosition(s_hoverRow, s_hoverCol)) {
                int cellValue = board.getCellValue(s_hoverRow, s_hoverCol);
                if (cellValue == static_cast<int>(CellType::STONE)) {
                    float cellX = bx + s_hoverCol * 18.0f;
                    float cellY = by + s_hoverRow * 18.0f;
                    sf::RectangleShape highlight(sf::Vector2f(18.0f - 2, 18.0f - 2));
                    highlight.setPosition(cellX + 1, cellY + 1);
                    highlight.setFillColor(sf::Color(255, 100, 100, 150));
                    highlight.setOutlineColor(sf::Color(255, 0, 0, 220));
                    highlight.setOutlineThickness(2);
                    window.draw(highlight);
                } else if (cellValue == static_cast<int>(CellType::EMPTY)) {
                    Tile tile = Tile::getTile(Tile::fromInt(static_cast<int>(TileId::TILE_0)));
                    if (tile.isValid()) {
                        bool firstTurn = !PlacementRules::playerHasCells(board, myId);
                        bool canPlace = PlacementRules::canPlaceTile(board, tile, s_hoverRow, s_hoverCol, myId, firstTurn);
                        BoardRenderer::drawPreview(window, tile, bx, by, 18.0f, bs, s_hoverRow, s_hoverCol, canPlace, GameState::PLAYERS_COLORS[myId]);
                    }
                }
            }
        }
    }
    
    // aide
    std::string helpText;
    if (s_selectingCoupon) {
        helpText = "Coupon selection: 1-5 choose | Esc cancel";
    } else {
        helpText = "Press Tab to view all tiles";
        if (!gameState.isGameOver() && turnId == myId) {
            helpText += " | R: Rotate | F: Flip H | V: Flip V";
            const auto& upcomingTiles = gameState.getUpcomingTiles();
            bool hasUpcomingOptions = false;
            for (int tileId : upcomingTiles) {
                if (tileId >= 0) {
                    hasUpcomingOptions = true;
                    break;
                }
            }
            if (gameState.getExchangeCouponCount() > 0) {
                if (hasUpcomingOptions) {
                    helpText += " | C: Select coupon";
                }
                helpText += " | X: Remove stone";
            }
        } else if (gameState.isGameOver() && gameState.getExchangeCouponCount() > 0) {
            helpText += " | C: Place 1x1 tile or remove stone | X: Remove stone | N: Skip";
        }
    }
    sf::Text help = Text::createText(helpText, 16);
    Element::centerH(help, static_cast<float>(ws.x), static_cast<float>(ws.y) - 30);
    help.setFillColor(sf::Color(150, 150, 150));
    window.draw(help);
    
    // tooltip au survol d'une case
    if (s_hoverRow >= 0 && s_hoverCol >= 0 && s_hoverRow < bs && s_hoverCol < bs) {
        int cellValue = board.getCellValue(s_hoverRow, s_hoverCol);
        std::string tooltipText = "";
        
        if (cellValue == static_cast<int>(CellType::EMPTY)) {
            // case vide, pas de tooltip
        } else if (cellValue == static_cast<int>(CellType::STONE)) {
            // pierre
            if (!gameState.isGameOver() && turnId == myId && gameState.getExchangeCouponCount() > 0) {
                tooltipText = "Stone tile - Press X to remove with coupon";
            } else if (gameState.isGameOver() && gameState.getExchangeCouponCount() > 0) {
                tooltipText = "Stone tile - Press C or X to remove with coupon";
            } else {
                tooltipText = "This is a stone tile";
            }
        } else if (cellValue == static_cast<int>(CellType::BONUS_EXCHANGE)) {
            // bonus échange (non capturé, sinon ce serait une case joueur)
            tooltipText = "Exchange bonus: Place tiles on 4 directions (N/S/E/W) to capture. Reward: 1 exchange coupon.";
        } else if (cellValue == static_cast<int>(CellType::BONUS_STONE)) {
            // bonus pierre
            tooltipText = "Stone bonus: Place tiles on 4 directions (N/S/E/W) to capture. Reward: Place 1 stone immediately.";
        } else if (cellValue == static_cast<int>(CellType::BONUS_ROBBERY)) {
            // bonus vol
            tooltipText = "Robbery bonus: Place tiles on 4 directions (N/S/E/W) to capture. Reward: Steal 1 tile from opponent.";
        } else if (cellValue >= 0 && cellValue < 9) {
            // case d'un joueur
            tooltipText = gameState.getPlayerNameByColorId(lobbyId, cellValue);
        }
        
        if (!tooltipText.empty()) {
            // calcule la position de la souris pour le tooltip
            float cellX = bx + s_hoverCol * 18.0f + 9.0f; // centre de la cellule en x
            float cellY = by + s_hoverRow * 18.0f + 9.0f; // centre de la cellule en y
            Tooltip::draw(window, tooltipText, cellX, cellY);
        }
    }
    
    // affiche la popup de confirmation si visible
    if (ConfirmDialog::isVisible()) {
        ConfirmDialog::draw(window, "");
    }
}

bool InGame::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    // gère la popup de confirmation
    if (ConfirmDialog::isVisible()) {
        bool confirmed = ConfirmDialog::handleInput(event);
        if (confirmed && s_pendingRow >= 0 && s_pendingCol >= 0) {
            // l'utilisateur a confirmé, on envoie le placement en attente
            if (g_client) {
                int myId = gameState.getSelectedColor();
                g_client->sendCellClick(gameState.getCurrentLobby(), s_pendingRow, s_pendingCol, 
                    s_pendingRotation, s_pendingFlippedH, s_pendingFlippedV, false);
                // réinitialise la prévisualisation après le clic
                g_client->sendTilePreview(gameState.getCurrentLobby(), -1, -1, 
                                         s_pendingRotation, 
                                         s_pendingFlippedH, 
                                         s_pendingFlippedV, 
                                         myId);
            }
            s_pendingRow = -1;
            s_pendingCol = -1;
            return false;
        } else if (event.type == sf::Event::KeyPressed && 
                   (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Escape)) {
            // la popup a été fermée (annulée)
            s_pendingRow = -1;
            s_pendingCol = -1;
            return false;
        }
    }
    
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
        gameState.setState(ClientState::VIEWING_TILES);
        return false;
    }
    
    if (event.type == sf::Event::KeyPressed) {
        if (!gameState.isGameOver()) {
            // gestion des transformations ou échange pendant la partie
            int turnId = gameState.getCurrentTurnColorId();
            int myId = gameState.getSelectedColor();
            if (turnId == myId) {
                auto keyToChoice = [](sf::Keyboard::Key key) -> int {
                    if (key >= sf::Keyboard::Num1 && key <= sf::Keyboard::Num5) {
                        return static_cast<int>(key - sf::Keyboard::Num1);
                    }
                    if (key >= sf::Keyboard::Numpad1 && key <= sf::Keyboard::Numpad5) {
                        return static_cast<int>(key - sf::Keyboard::Numpad1);
                    }
                    return -1;
                };

                if (s_selectingCoupon) {
                    int choice = keyToChoice(event.key.code);
                    if (choice >= 0) {
                        const auto& upcoming = gameState.getUpcomingTiles();
                        if (choice < 5 && upcoming[choice] >= 0 && g_client) {
                            g_client->sendCellClick(gameState.getCurrentLobby(), -1, -1, 0, false, false, true, choice);
                            s_selectingCoupon = false;
                        }
                    } else if (event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::C) {
                        s_selectingCoupon = false;
                    }
                    return false;
                }

                bool transformChanged = false;
                if (event.key.code == sf::Keyboard::R) {
                    gameState.setTileRotation(gameState.getTileRotation() + 1);
                    transformChanged = true;
                } else if (event.key.code == sf::Keyboard::F) {
                    gameState.setTileFlippedH(!gameState.getTileFlippedH());
                    transformChanged = true;
                } else if (event.key.code == sf::Keyboard::V) {
                    gameState.setTileFlippedV(!gameState.getTileFlippedV());
                    transformChanged = true;
                } else if (event.key.code == sf::Keyboard::C) {
                    if (gameState.getExchangeCouponCount() > 0) {
                        const auto& upcoming = gameState.getUpcomingTiles();
                        bool hasOption = false;
                        for (int tile : upcoming) {
                            if (tile >= 0) {
                                hasOption = true;
                                break;
                            }
                        }
                        if (hasOption) {
                            s_selectingCoupon = !s_selectingCoupon;
                        }
                    }
                } else if (event.key.code == sf::Keyboard::X) {
                    if (gameState.getExchangeCouponCount() > 0 && g_client && s_hoverRow >= 0 && s_hoverCol >= 0) {
                        const Board& board = gameState.getBoard();
                        if (board.isValidPosition(s_hoverRow, s_hoverCol)) {
                            int cellValue = board.getCellValue(s_hoverRow, s_hoverCol);
                            if (cellValue == static_cast<int>(CellType::EMPTY) || cellValue == static_cast<int>(CellType::STONE)) {
                                if (cellValue == static_cast<int>(CellType::EMPTY)) {
                                    int myId = gameState.getSelectedColor();
                                    Tile tile = Tile::getTile(Tile::fromInt(static_cast<int>(TileId::TILE_0)));
                                    if (tile.isValid()) {
                                        bool firstTurn = !PlacementRules::playerHasCells(board, myId);
                                        if (!PlacementRules::canPlaceTile(board, tile, s_hoverRow, s_hoverCol, myId, firstTurn)) {
                                            return false;
                                        }
                                    }
                                }
                                g_client->sendCellClick(gameState.getCurrentLobby(), s_hoverRow, s_hoverCol, 0, false, false, true);
                            }
                        }
                    }
                }
                
                // envoie la prévisualisation mise à jour si les transformations ont changé
                if (transformChanged && g_client && s_hoverRow >= 0 && s_hoverCol >= 0) {
                    g_client->sendTilePreview(gameState.getCurrentLobby(), s_hoverRow, s_hoverCol, 
                                             gameState.getTileRotation(), 
                                             gameState.getTileFlippedH(), 
                                             gameState.getTileFlippedV(), 
                                             myId);
                }
            }
        } else {
            // phase finale : placement/skip des coupons restants
            if (event.key.code == sf::Keyboard::C) {
                if (gameState.getExchangeCouponCount() > 0 && g_client) {
                    if (s_hoverRow >= 0 && s_hoverCol >= 0) {
                        const Board& board = gameState.getBoard();
                        if (board.isValidPosition(s_hoverRow, s_hoverCol)) {
                            int cellValue = board.getCellValue(s_hoverRow, s_hoverCol);
                            if (cellValue == static_cast<int>(CellType::EMPTY) || cellValue == static_cast<int>(CellType::STONE)) {
                                if (cellValue == static_cast<int>(CellType::EMPTY)) {
                                    int myId = gameState.getSelectedColor();
                                    Tile tile = Tile::getTile(Tile::fromInt(static_cast<int>(TileId::TILE_0)));
                                    if (tile.isValid()) {
                                        bool firstTurn = !PlacementRules::playerHasCells(board, myId);
                                        if (!PlacementRules::canPlaceTile(board, tile, s_hoverRow, s_hoverCol, myId, firstTurn)) {
                                            return false;
                                        }
                                    }
                                }
                                g_client->sendCellClick(gameState.getCurrentLobby(), s_hoverRow, s_hoverCol, 0, false, false, true);
                            }
                        }
                    }
                }
            } else if (event.key.code == sf::Keyboard::N) {
                if (gameState.getExchangeCouponCount() > 0 && g_client) {
                    g_client->sendCellClick(gameState.getCurrentLobby(), -1, -1, 0, false, false, true, -1);
                }
            }
        }
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
            
            // envoie la prévisualisation si c'est mon tour
            if (!gameState.isGameOver()) {
                int turnId = gameState.getCurrentTurnColorId();
                int myId = gameState.getSelectedColor();
                if (turnId == myId && g_client) {
                    g_client->sendTilePreview(gameState.getCurrentLobby(), row, col, 
                                             gameState.getTileRotation(), 
                                             gameState.getTileFlippedH(), 
                                             gameState.getTileFlippedV(), 
                                             myId);
                }
            }
        } else {
            s_hoverRow = -1;
            s_hoverCol = -1;
            
            // envoie une prévisualisation vide si on sort du board
            if (!gameState.isGameOver()) {
                int turnId = gameState.getCurrentTurnColorId();
                int myId = gameState.getSelectedColor();
                if (turnId == myId && g_client) {
                    g_client->sendTilePreview(gameState.getCurrentLobby(), -1, -1, 
                                             gameState.getTileRotation(), 
                                             gameState.getTileFlippedH(), 
                                             gameState.getTileFlippedV(), 
                                             myId);
                }
            }
        }
    }
    
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (gameState.isGameOver() && gameState.getWinnerId() < 0 && gameState.getExchangeCouponCount() > 0) {
            int row, col;
            if (BoardRenderer::handleClick(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y), bx, by, 18.0f, bs, row, col)) {
                const Board& board = gameState.getBoard();
                if (board.isValidPosition(row, col)) {
                    int cellValue = board.getCellValue(row, col);
                    if (cellValue == static_cast<int>(CellType::EMPTY) || cellValue == static_cast<int>(CellType::STONE)) {
                        int myId = gameState.getSelectedColor();
                        if (cellValue == static_cast<int>(CellType::EMPTY)) {
                            Tile tile = Tile::getTile(Tile::fromInt(static_cast<int>(TileId::TILE_0)));
                            if (tile.isValid()) {
                                bool firstTurn = !PlacementRules::playerHasCells(board, myId);
                                if (!PlacementRules::canPlaceTile(board, tile, row, col, myId, firstTurn)) {
                                    return false;
                                }
                            }
                        }
                        if (g_client) {
                            g_client->sendCellClick(gameState.getCurrentLobby(), row, col, 0, false, false, true);
                        }
                    }
                }
            }
            return false;
        }
        
        if (gameState.isGameOver()) return false;
        
        int turnId = gameState.getCurrentTurnColorId();
        int myId = gameState.getSelectedColor();
        if (turnId != myId) return false;
        
        // vérifie si le bouton d'abandon de tuile est cliqué
        if (s_discardButton && s_discardButton->getBounds().contains(static_cast<float>(event.mouseButton.x), 
                                                                      static_cast<float>(event.mouseButton.y))) {
            if (g_client && !gameState.canPlaceTile() && gameState.getCurrentPlayerTileId() >= 0 &&
                !gameState.hasPendingStoneBonus() && !gameState.hasPendingRobberyBonus()) {
                g_client->sendDiscardTile(gameState.getCurrentLobby());
            }
            return false;
        }
        
        // vérifie si le joueur a un bonus en attente
        if (gameState.hasPendingStoneBonus()) {
            // bonus de pierre : clic sur une case vide pour placer la pierre
            int row, col;
            if (BoardRenderer::handleClick(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y), bx, by, 18.0f, bs, row, col)) {
                const Board& board = gameState.getBoard();
                if (board.isValidPosition(row, col) && board.isEmpty(row, col)) {
                    if (g_client) {
                        g_client->sendPlaceStone(gameState.getCurrentLobby(), row, col);
                    }
                }
            }
            return false;
        }
        
        if (gameState.hasPendingRobberyBonus()) {
            // bonus de vol : clic sur une case d'un autre joueur pour voler sa tuile
            int row, col;
            if (BoardRenderer::handleClick(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y), bx, by, 18.0f, bs, row, col)) {
                const Board& board = gameState.getBoard();
                if (board.isValidPosition(row, col)) {
                    int cellValue = board.getCellValue(row, col);
                    // vérifie que c'est une case d'un autre joueur
                    if (cellValue >= 0 && cellValue < 9 && cellValue != myId) {
                        if (g_client) {
                            g_client->sendRobTile(gameState.getCurrentLobby(), cellValue);
                        }
                    }
                }
            }
            return false;
        }
        
        // placement normal de tuile
        int row, col;
        if (BoardRenderer::handleClick(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y), bx, by, 18.0f, bs, row, col)) {
            // vérifie si la tuile couvrirait un bonus
            const Board& board = gameState.getBoard();
            int tileId = gameState.getCurrentPlayerTileId();
            bool wouldCoverBonus = false;
            std::string bonusTypeName = "";
            
            if (tileId >= 0 && tileId < static_cast<int>(TileId::TOTAL_TILES)) {
                const Tile& baseTile = Tile::getTile(Tile::fromInt(tileId));
                if (baseTile.isValid()) {
                    Tile tile = applyTileTransform(baseTile, gameState.getTileRotation(), 
                                                   gameState.getTileFlippedH(), gameState.getTileFlippedV());
                    
                    for (const auto& [blockRow, blockCol] : tile.blocks) {
                        int actualRow = row + blockRow;
                        int actualCol = col + blockCol;
                        if (board.isValidPosition(actualRow, actualCol)) {
                            int cellValue = board.getCellValue(actualRow, actualCol);
                            if (cellValue == static_cast<int>(CellType::BONUS_EXCHANGE)) {
                                wouldCoverBonus = true;
                                bonusTypeName = "Tile exchange bonus";
                                break;
                            } else if (cellValue == static_cast<int>(CellType::BONUS_STONE)) {
                                wouldCoverBonus = true;
                                bonusTypeName = "Stone bonus";
                                break;
                            } else if (cellValue == static_cast<int>(CellType::BONUS_ROBBERY)) {
                                wouldCoverBonus = true;
                                bonusTypeName = "Robbery bonus";
                                break;
                            }
                        }
                    }
                }
            }
            
            if (wouldCoverBonus) {
                // affiche la popup de confirmation
                std::string message = "This tile will cover a " + bonusTypeName + "\nand it will be lost!\n\nContinue?";
                ConfirmDialog::show(message);
                // sauvegarde les paramètres du placement
                s_pendingRow = row;
                s_pendingCol = col;
                s_pendingRotation = gameState.getTileRotation();
                s_pendingFlippedH = gameState.getTileFlippedH();
                s_pendingFlippedV = gameState.getTileFlippedV();
            } else {
                // placement normal, pas de bonus à couvrir
                if (g_client) {
                    g_client->sendCellClick(gameState.getCurrentLobby(), row, col, 
                        gameState.getTileRotation(), gameState.getTileFlippedH(), gameState.getTileFlippedV(), false);
                    // réinitialise la prévisualisation après le clic
                    g_client->sendTilePreview(gameState.getCurrentLobby(), -1, -1, 
                                             gameState.getTileRotation(), 
                                             gameState.getTileFlippedH(), 
                                             gameState.getTileFlippedV(), 
                                             myId);
                }
            }
        }
    }
    return false;
}
