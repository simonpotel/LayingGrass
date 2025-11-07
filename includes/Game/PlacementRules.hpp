#pragma once
#include "Game/Board.hpp"
#include "Game/Tile.hpp"

namespace PlacementRules {
    bool playerHasCells(const Board& board, int colorId);
    bool canPlaceTile(const Board& board, const Tile& tile, int anchorRow, int anchorCol, int colorId, bool isFirstTurn);
}


