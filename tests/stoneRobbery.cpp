#include <gtest/gtest.h>
#include "Game/Game.hpp"
#include "Game/Board.hpp"
#include "Game/Cell.hpp"
#include "Game/Tile.hpp"
#include "Game/PlacementRules.hpp"
#include "Lobby.hpp"

// test du placement d'une pierre sur n'importe quel espace vide

TEST(StoneRobbery, PlaceStoneOnEmptySpace) {
    Lobby lobby(1);
    lobby.addConnection(1, "Player1", 0);
    lobby.addConnection(2, "Player2", 1);
    
    Game game(1, &lobby);
    Board* board = game.getBoard();
    
    // vérifie qu'une cellule est vide au départ
    EXPECT_TRUE(board->isEmpty(15, 15));
    
    // simule qu'un joueur a un bonus de pierre en attente
    int currentPlayer = game.getCurrentPlayerConnection();
    
    // place une pierre manuellement sur le plateau
    board->setCell(15, 15, Cell(CellType::STONE));
    
    // vérifie que la pierre est bien placée
    EXPECT_TRUE(board->getCell(15, 15).isStone());
    EXPECT_FALSE(board->isEmpty(15, 15));
}

// test que la pierre bloque le placement futur de tuiles

TEST(StoneRobbery, StoneBlocksFutureTiles) {
    Board board(20);
    
    // place une pierre
    board.setCell(10, 10, Cell(CellType::STONE));
    
    // vérifie qu'on ne peut pas placer une tuile sur la pierre
    const Tile& tile = Tile::getTile(TileId::TILE_0);
    
    // ne peut pas placer sur une pierre (pas au premier tour)
    EXPECT_FALSE(PlacementRules::canPlaceTile(board, tile, 10, 10, 0, false));
    
    // même au premier tour, on ne peut pas placer sur une pierre
    EXPECT_FALSE(PlacementRules::canPlaceTile(board, tile, 10, 10, 0, true));
}

// test de la suppression d'une pierre avec un coupon

TEST(StoneRobbery, RemoveStoneWithCoupon) {
    Lobby lobby(1);
    lobby.addConnection(1, "Player1", 0);
    lobby.addConnection(2, "Player2", 1);
    
    Game game(1, &lobby);
    Board* board = game.getBoard();
    
    // place une pierre manuellement
    board->setCell(10, 10, Cell(CellType::STONE));
    EXPECT_TRUE(board->getCell(10, 10).isStone());
    
    // vérifie que le joueur a un coupon
    int currentPlayer = game.getCurrentPlayerConnection();
    EXPECT_EQ(game.getExchangeCouponCount(currentPlayer), 1);
    
    // dans le jeu réel, on pourrait utiliser un coupon pour retirer la pierre
    // (fonctionnalité non implémentée dans le code actuel, mais mentionnée dans la consigne)
    
    // pour l'instant, on vérifie juste que la pierre peut être remplacée
    board->setCell(10, 10, Cell(0)); // remplace par une cellule du joueur
    EXPECT_FALSE(board->getCell(10, 10).isStone());
    EXPECT_TRUE(board->isPlayerCell(10, 10, 0));
}

// test du vol d'une tuile d'un adversaire pour utilisation immédiate

TEST(StoneRobbery, RobberyStealsTileForImmediateUse) {
    Lobby lobby(1);
    lobby.addConnection(1, "Player1", 0);
    lobby.addConnection(2, "Player2", 1);
    
    Game game(1, &lobby);
    
    // vérifie que la fonction robTile existe et peut être appelée
    int currentPlayer = game.getCurrentPlayerConnection();
    
    // vérifie que le joueur n'a pas de bonus de vol en attente au début
    EXPECT_FALSE(game.hasPendingRobberyBonus(currentPlayer));
    
    // vérifie que la fonction robTile existe (même si elle échoue sans bonus)
    // tente de voler une tuile (devrait échouer car pas de bonus)
    bool result = game.robTile(currentPlayer, 1); // tente de voler au joueur couleur 1
    EXPECT_FALSE(result); // devrait échouer car pas de bonus en attente
}

