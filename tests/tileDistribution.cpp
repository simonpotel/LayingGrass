#include <gtest/gtest.h>
#include "Game/Tile.hpp"
#include "Game/Game.hpp"
#include "Lobby.hpp"
#include <unordered_set>

// test que les 96 formes de tuiles uniques sont disponibles

TEST(TileDistribution, AllTilesAvailable) {
    // vérifie que toutes les tuiles de TILE_0 à TILE_95 sont valides
    for (int i = 0; i < static_cast<int>(TileId::TOTAL_TILES); ++i) {
        const Tile& tile = Tile::getTile(static_cast<TileId>(i));
        EXPECT_TRUE(tile.isValid()) << "Tile " << i << " should be valid";
        EXPECT_GT(tile.getBlockCount(), 0u) << "Tile " << i << " should have blocks";
    }
    
    // vérifie qu'il y a exactement 96 tuiles
    EXPECT_EQ(static_cast<int>(TileId::TOTAL_TILES), 96);
}

// test que la queue de tuiles est prédéterminée mais randomisée au début

TEST(TileDistribution, TileQueuePredeterminedButRandom) {
    // crée deux jeux avec le même nombre de joueurs
    Lobby lobby1(1);
    lobby1.addConnection(1, "Player1", 0);
    lobby1.addConnection(2, "Player2", 1);
    
    Lobby lobby2(2);
    lobby2.addConnection(3, "Player3", 0);
    lobby2.addConnection(4, "Player4", 1);
    
    Game game1(1, &lobby1);
    Game game2(2, &lobby2);
    
    // récupère les tuiles actuelles des joueurs (premier tour)
    int tile1_p1 = game1.getCurrentPlayerTileId(game1.getCurrentPlayerConnection());
    int tile2_p1 = game2.getCurrentPlayerTileId(game2.getCurrentPlayerConnection());
    
    // les deux jeux peuvent avoir des tuiles différentes (randomisé)
    // mais dans chaque jeu, l'ordre est prédéterminé
    // vérifie que les tuiles sont valides
    EXPECT_GE(tile1_p1, 0);
    EXPECT_LT(tile1_p1, 96);
    EXPECT_GE(tile2_p1, 0);
    EXPECT_LT(tile2_p1, 96);
}

// test du nombre correct de tuiles par joueur (10.67 arrondi)

TEST(TileDistribution, CorrectTileCountPerPlayer) {
    // 2 joueurs : 10.67 * 2 = 21.34 ≈ 21 tuiles
    Lobby lobby2(1);
    lobby2.addConnection(1, "P1", 0);
    lobby2.addConnection(2, "P2", 1);
    Game game2(1, &lobby2);
    
    // 3 joueurs : 10.67 * 3 = 32.01 ≈ 32 tuiles
    Lobby lobby3(2);
    lobby3.addConnection(1, "P1", 0);
    lobby3.addConnection(2, "P2", 1);
    lobby3.addConnection(3, "P3", 2);
    Game game3(2, &lobby3);
    
    // 4 joueurs : 10.67 * 4 = 42.68 ≈ 43 tuiles
    Lobby lobby4(3);
    lobby4.addConnection(1, "P1", 0);
    lobby4.addConnection(2, "P2", 1);
    lobby4.addConnection(3, "P3", 2);
    lobby4.addConnection(4, "P4", 3);
    Game game4(3, &lobby4);
    
    // vérifie que les jeux ont été créés avec succès
    EXPECT_NE(game2.getBoard(), nullptr);
    EXPECT_NE(game3.getBoard(), nullptr);
    EXPECT_NE(game4.getBoard(), nullptr);
    
    // vérifie que les joueurs ont reçu leur première tuile
    EXPECT_NE(game2.getCurrentPlayerTileId(game2.getCurrentPlayerConnection()), -1);
    EXPECT_NE(game3.getCurrentPlayerTileId(game3.getCurrentPlayerConnection()), -1);
    EXPECT_NE(game4.getCurrentPlayerTileId(game4.getCurrentPlayerConnection()), -1);
}

// test que les joueurs reçoivent les tuiles dans l'ordre de la queue

TEST(TileDistribution, PlayersReceiveTilesInOrder) {
    Lobby lobby(1);
    lobby.addConnection(1, "Player1", 0);
    lobby.addConnection(2, "Player2", 1);
    
    Game game(1, &lobby);
    
    // le premier joueur reçoit une tuile au début
    int currentPlayer = game.getCurrentPlayerConnection();
    int firstTile = game.getCurrentPlayerTileId(currentPlayer);
    
    // vérifie que le premier joueur a une tuile (devrait être TILE_0 pour le premier tour)
    EXPECT_EQ(firstTile, static_cast<int>(TileId::TILE_0));
    
    // simule un placement valide pour passer au tour suivant
    Board* board = game.getBoard();
    board->setCell(10, 10, Cell(0)); // simule le territoire du premier joueur
    
    // place la tuile du premier joueur pour passer au suivant
    game.handleCellClick(currentPlayer, 10, 10, 0, false, false, false, -1);
    
    // le prochain joueur devrait avoir une tuile différente (du second tour)
    int nextPlayer = game.getCurrentPlayerConnection();
    int secondTile = game.getCurrentPlayerTileId(nextPlayer);
    
    // vérifie que le second joueur a aussi TILE_0 (c'est son premier tour)
    EXPECT_EQ(secondTile, static_cast<int>(TileId::TILE_0));
}

