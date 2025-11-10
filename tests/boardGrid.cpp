#include <gtest/gtest.h>
#include "Game/Board.hpp"
#include "Game/Cell.hpp"
#include "Game/Game.hpp"
#include "Lobby.hpp"

// test de la taille du plateau pour 2-4 joueurs (20x20)

TEST(BoardGrid, BoardSize20For2to4Players) {
    // crée un lobby avec 2 joueurs
    Lobby lobby(1);
    lobby.addConnection(1, "Player1", 0);
    lobby.addConnection(2, "Player2", 1);
    
    // crée un jeu avec 2 joueurs
    Game game(1, &lobby);
    
    // vérifie que le plateau est de taille 20x20
    EXPECT_EQ(game.getBoard()->getSize(), 20);
    
    // test avec 4 joueurs
    Lobby lobby4(2);
    lobby4.addConnection(1, "P1", 0);
    lobby4.addConnection(2, "P2", 1);
    lobby4.addConnection(3, "P3", 2);
    lobby4.addConnection(4, "P4", 3);
    
    Game game4(2, &lobby4);
    EXPECT_EQ(game4.getBoard()->getSize(), 20);
}

// test de la taille du plateau pour 5-9 joueurs (30x30)

TEST(BoardGrid, BoardSize30For5to9Players) {
    // crée un lobby avec 5 joueurs
    Lobby lobby(1);
    lobby.addConnection(1, "Player1", 0);
    lobby.addConnection(2, "Player2", 1);
    lobby.addConnection(3, "Player3", 2);
    lobby.addConnection(4, "Player4", 3);
    lobby.addConnection(5, "Player5", 4);
    
    // crée un jeu avec 5 joueurs
    Game game(1, &lobby);
    
    // vérifie que le plateau est de taille 30x30
    EXPECT_EQ(game.getBoard()->getSize(), 30);
    
    // test avec 9 joueurs
    Lobby lobby9(2);
    for (int i = 0; i < 9; ++i) {
        lobby9.addConnection(i + 1, "Player" + std::to_string(i), i);
    }
    
    Game game9(2, &lobby9);
    EXPECT_EQ(game9.getBoard()->getSize(), 30);
}

// test des types de cellules (vide, herbe de joueur, pierre, bonus)

TEST(BoardGrid, CellTypes) {
    Board board(20);
    
    // vérifie qu'une cellule vide est bien vide
    EXPECT_TRUE(board.isEmpty(0, 0));
    
    // place une cellule de joueur (herbe)
    board.setCell(0, 0, Cell(0)); // joueur 0
    EXPECT_TRUE(board.isPlayerCell(0, 0, 0));
    EXPECT_FALSE(board.isEmpty(0, 0));
    
    // place une pierre
    board.setCell(1, 1, Cell(CellType::STONE));
    EXPECT_TRUE(board.getCell(1, 1).isStone());
    
    // place un bonus d'échange
    board.setCell(2, 2, Cell(CellType::BONUS_EXCHANGE));
    EXPECT_TRUE(board.isBonus(2, 2));
    
    // place un bonus de pierre
    board.setCell(3, 3, Cell(CellType::BONUS_STONE));
    EXPECT_TRUE(board.isBonus(3, 3));
    
    // place un bonus de vol
    board.setCell(4, 4, Cell(CellType::BONUS_ROBBERY));
    EXPECT_TRUE(board.isBonus(4, 4));
}

// test de la propriété du territoire par couleur de joueur

TEST(BoardGrid, TerritoryOwnershipByColor) {
    Board board(20);
    
    // place des cellules pour différents joueurs
    board.setCell(0, 0, Cell(0)); // joueur 0 (rouge)
    board.setCell(1, 1, Cell(1)); // joueur 1 (bleu)
    board.setCell(2, 2, Cell(2)); // joueur 2 (vert)
    
    // vérifie que chaque cellule appartient au bon joueur
    EXPECT_TRUE(board.isPlayerCell(0, 0, 0));
    EXPECT_FALSE(board.isPlayerCell(0, 0, 1));
    
    EXPECT_TRUE(board.isPlayerCell(1, 1, 1));
    EXPECT_FALSE(board.isPlayerCell(1, 1, 0));
    
    EXPECT_TRUE(board.isPlayerCell(2, 2, 2));
    EXPECT_FALSE(board.isPlayerCell(2, 2, 1));
    
    // vérifie getPlayerId
    EXPECT_EQ(board.getPlayerId(0, 0), 0);
    EXPECT_EQ(board.getPlayerId(1, 1), 1);
    EXPECT_EQ(board.getPlayerId(2, 2), 2);
    EXPECT_EQ(board.getPlayerId(5, 5), -1); // cellule vide
}

// test des règles de placement des bonus (pas sur les bords, pas adjacents)

TEST(BoardGrid, BonusPlacementRules) {
    Board board(20);
    
    // vérifie qu'on ne peut pas placer un bonus sur un bord
    EXPECT_FALSE(board.isValidBonusPosition(0, 5)); // bord haut
    EXPECT_FALSE(board.isValidBonusPosition(19, 5)); // bord bas
    EXPECT_FALSE(board.isValidBonusPosition(5, 0)); // bord gauche
    EXPECT_FALSE(board.isValidBonusPosition(5, 19)); // bord droit
    
    // vérifie qu'on peut placer un bonus au centre
    EXPECT_TRUE(board.isValidBonusPosition(10, 10));
    
    // place un bonus
    board.setCell(10, 10, Cell(CellType::BONUS_EXCHANGE));
    
    // vérifie qu'on ne peut pas placer un bonus adjacent (4 directions cardinales)
    EXPECT_FALSE(board.isValidBonusPosition(9, 10)); // nord
    EXPECT_FALSE(board.isValidBonusPosition(11, 10)); // sud
    EXPECT_FALSE(board.isValidBonusPosition(10, 9)); // ouest
    EXPECT_FALSE(board.isValidBonusPosition(10, 11)); // est
    
    // vérifie qu'on peut placer un bonus en diagonale (pas adjacent au sens cardinal)
    EXPECT_TRUE(board.isValidBonusPosition(9, 9));
    EXPECT_TRUE(board.isValidBonusPosition(11, 11));
}

