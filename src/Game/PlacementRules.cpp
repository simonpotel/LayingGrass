#include "Game/PlacementRules.hpp"

namespace PlacementRules {
    bool playerHasCells(const Board& board, int colorId) {
        if (colorId < 0) {
            return false; // aucun territoire si la couleur est invalide
        }

        int size = board.getSize();
        for (int row = 0; row < size; ++row) {
            for (int col = 0; col < size; ++col) {
                if (board.getCellValue(row, col) == colorId) {
                    return true; // cellule du joueur trouvée
                }
            }
        }
        return false; // aucune cellule trouvée
    }

    bool canPlaceTile(const Board& board, const Tile& tile, int anchorRow, int anchorCol, int colorId, bool isFirstTurn) {
        if (!tile.isValid() || colorId < 0) {
            return false; // impossible si la tuile ou la couleur est invalide
        }

        bool touchesOwnTerritory = false;
        int size = board.getSize();

        for (const auto& block : tile.blocks) {
            int row = anchorRow + block.first;
            int col = anchorCol + block.second;

            if (row < 0 || row >= size || col < 0 || col >= size) {
                return false; // bloc hors plateau
            }

            if (!board.isEmpty(row, col) && !board.isBonus(row, col)) {
                return false; // cellule occupée
            }

            if (board.touchesOtherPlayerTerritory(row, col, colorId)) {
                return false; // touche un adversaire
            }

            if (!isFirstTurn && board.hasAdjacentPlayerCell(row, col, colorId)) {
                touchesOwnTerritory = true; // touche notre territoire
            }
        }

        if (!isFirstTurn && !touchesOwnTerritory) {
            return false; // aucun bloc adjacent à notre territoire
        }

        return true; // placement valide
    }
}


