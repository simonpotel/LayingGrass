#include <gtest/gtest.h>
#include "Game/Game.hpp"
#include "Game/Board.hpp"
#include "Game/Tile.hpp"
#include "Game/PlacementRules.hpp"
#include "Lobby.hpp"

// test que la première tuile 1x1 peut être placée n'importe où sur une cellule vide

TEST(TilePlacement, FirstTile1x1AnywhereEmpty) {
    Board board(20);
    const Tile& tile = Tile::getTile(TileId::TILE_0); // tuile 1x1
    
    // premier tour : peut placer n'importe où sur le plateau
    EXPECT_TRUE(PlacementRules::canPlaceTile(board, tile, 0, 0, 0, true));
    EXPECT_TRUE(PlacementRules::canPlaceTile(board, tile, 10, 10, 0, true));
    EXPECT_TRUE(PlacementRules::canPlaceTile(board, tile, 19, 19, 0, true));
    
    // ne peut pas placer sur une cellule occupée
    board.setCell(5, 5, Cell(1)); // joueur 1
    EXPECT_FALSE(PlacementRules::canPlaceTile(board, tile, 5, 5, 0, true));
}

// test que les tuiles suivantes doivent toucher le territoire du joueur sur au moins un côté

TEST(TilePlacement, SubsequentMustTouchTerritory) {
    Board board(20);
    
    // place le territoire initial du joueur 0
    board.setCell(10, 10, Cell(0));
    
    const Tile& tile = Tile::getTile(TileId::TILE_0); // tuile 1x1
    
    // doit toucher le territoire sur au moins un côté (pas le premier tour)
    EXPECT_TRUE(PlacementRules::canPlaceTile(board, tile, 9, 10, 0, false)); // nord
    EXPECT_TRUE(PlacementRules::canPlaceTile(board, tile, 11, 10, 0, false)); // sud
    EXPECT_TRUE(PlacementRules::canPlaceTile(board, tile, 10, 9, 0, false)); // ouest
    EXPECT_TRUE(PlacementRules::canPlaceTile(board, tile, 10, 11, 0, false)); // est
    
    // ne peut pas placer loin du territoire
    EXPECT_FALSE(PlacementRules::canPlaceTile(board, tile, 0, 0, 0, false));
    
    // ne peut pas placer en diagonale (doit toucher un côté)
    EXPECT_FALSE(PlacementRules::canPlaceTile(board, tile, 9, 9, 0, false));
}

// test que les tuiles ne peuvent pas toucher le territoire adverse

TEST(TilePlacement, CannotTouchOpponentTerritory) {
    Board board(20);
    
    // place le territoire du joueur 0 et du joueur 1
    board.setCell(10, 10, Cell(0)); // joueur 0
    board.setCell(10, 12, Cell(1)); // joueur 1
    
    const Tile& tile = Tile::getTile(TileId::TILE_0);
    
    // le joueur 0 ne peut pas placer adjacent au territoire du joueur 1
    EXPECT_FALSE(PlacementRules::canPlaceTile(board, tile, 10, 11, 0, false));
    
    // le joueur 0 peut placer adjacent à son propre territoire (pas au joueur 1)
    EXPECT_TRUE(PlacementRules::canPlaceTile(board, tile, 10, 9, 0, false));
}

// test que les tuiles ne peuvent pas se chevaucher

TEST(TilePlacement, CannotOverlap) {
    Board board(20);
    
    // place une cellule du joueur 0
    board.setCell(10, 10, Cell(0));
    
    const Tile& tile = Tile::getTile(TileId::TILE_0);
    
    // ne peut pas placer sur une cellule occupée par le joueur
    EXPECT_FALSE(PlacementRules::canPlaceTile(board, tile, 10, 10, 0, true));
    
    // place une pierre
    board.setCell(5, 5, Cell(CellType::STONE));
    
    // ne peut pas placer sur une pierre
    EXPECT_FALSE(PlacementRules::canPlaceTile(board, tile, 5, 5, 0, true));
}

// test que les tuiles doivent rester dans les limites du plateau

TEST(TilePlacement, RespectsBoardBoundaries) {
    Board board(20);
    const Tile& tile = Tile::getTile(TileId::TILE_0);
    
    // peut placer sur les bords
    EXPECT_TRUE(PlacementRules::canPlaceTile(board, tile, 0, 0, 0, true));
    EXPECT_TRUE(PlacementRules::canPlaceTile(board, tile, 19, 19, 0, true));
    
    // ne peut pas placer hors du plateau
    EXPECT_FALSE(PlacementRules::canPlaceTile(board, tile, 20, 0, 0, true));
    EXPECT_FALSE(PlacementRules::canPlaceTile(board, tile, 0, 20, 0, true));
    EXPECT_FALSE(PlacementRules::canPlaceTile(board, tile, -1, 0, 0, true));
}

// test de la rotation des tuiles (90°, 180°, 270°)

TEST(TilePlacement, TileRotation) {
    // utilise une tuile en L pour tester la rotation
    const Tile& baseTile = Tile::getTile(TileId::TILE_10); // tuile en L
    
    // rotation de 90°
    Tile rotated90 = baseTile.rotate90();
    EXPECT_TRUE(rotated90.isValid());
    EXPECT_EQ(rotated90.getBlockCount(), baseTile.getBlockCount());
    
    // rotation de 180° (2x 90°)
    Tile rotated180 = rotated90.rotate90();
    EXPECT_TRUE(rotated180.isValid());
    
    // rotation de 270° (3x 90°)
    Tile rotated270 = rotated180.rotate90();
    EXPECT_TRUE(rotated270.isValid());
    
    // rotation de 360° (4x 90°) revient à l'original (même forme normalisée)
    Tile rotated360 = rotated270.rotate90();
    EXPECT_TRUE(rotated360.isValid());
    EXPECT_EQ(rotated360.getBlockCount(), baseTile.getBlockCount());
}

// test du retournement des tuiles (horizontal, vertical)

TEST(TilePlacement, TileFlipping) {
    const Tile& baseTile = Tile::getTile(TileId::TILE_10);
    
    // flip horizontal
    Tile flippedH = baseTile.flipHorizontal();
    EXPECT_TRUE(flippedH.isValid());
    EXPECT_EQ(flippedH.getBlockCount(), baseTile.getBlockCount());
    
    // flip vertical
    Tile flippedV = baseTile.flipVertical();
    EXPECT_TRUE(flippedV.isValid());
    EXPECT_EQ(flippedV.getBlockCount(), baseTile.getBlockCount());
    
    // flip horizontal puis vertical
    Tile flippedBoth = flippedH.flipVertical();
    EXPECT_TRUE(flippedBoth.isValid());
    EXPECT_EQ(flippedBoth.getBlockCount(), baseTile.getBlockCount());
}
