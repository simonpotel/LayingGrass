#include <gtest/gtest.h>
#include "Game/PlacementRules.hpp"
#include "Game/Board.hpp"
#include "Game/Tile.hpp"

// test du rejet d'un placement en dehors de la grille

TEST(GamePlacement, RejectsOutOfBoundsPlacement) {
    Board board(20); // crée un plateau de 20x20 (cas 2-4 joueurs)
    const Tile& tile = Tile::getTile(TileId::TILE_0); // récupère une tuile 1x1 pour le test

    // vérifie qu'une tuile placée hors des limites (ligne) est refusée
    EXPECT_FALSE(PlacementRules::canPlaceTile(board, tile, 20, 0, 0, true));

    // vérifie qu'une tuile placée hors des limites (colonne) est refusée
    EXPECT_FALSE(PlacementRules::canPlaceTile(board, tile, 0, 20, 0, true));

    // vérifie qu'une tuile correctement placée est acceptée
    EXPECT_TRUE(PlacementRules::canPlaceTile(board, tile, 0, 0, 0, true));
}


