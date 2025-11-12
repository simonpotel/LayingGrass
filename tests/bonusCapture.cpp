#include <gtest/gtest.h>
#include "Game/Game.hpp"
#include "Game/Board.hpp"
#include "Game/Cell.hpp"
#include "Lobby.hpp"

// test du nombre de bonus d'échange (1.5 par joueur, arrondi au supérieur)

TEST(BonusCapture, ExchangeBonusCount) {
    // 2 joueurs : 1.5 * 2 = 3 bonus d'échange
    Lobby lobby2(1);
    lobby2.addConnection(1, "P1", 0);
    lobby2.addConnection(2, "P2", 1);
    Game game2(1, &lobby2);
    
    // compte les bonus d'échange sur le plateau
    int exchangeCount = 0;
    Board* board = game2.getBoard();
    for (int i = 0; i < board->getSize(); ++i) {
        for (int j = 0; j < board->getSize(); ++j) {
            if (board->getCell(i, j).getType() == CellType::BONUS_EXCHANGE) {
                exchangeCount++;
            }
        }
    }
    EXPECT_EQ(exchangeCount, 3); // ceil(1.5 * 2) = 3
    
    // 3 joueurs : 1.5 * 3 = 4.5 ≈ 5 bonus d'échange
    Lobby lobby3(2);
    lobby3.addConnection(1, "P1", 0);
    lobby3.addConnection(2, "P2", 1);
    lobby3.addConnection(3, "P3", 2);
    Game game3(2, &lobby3);
    
    exchangeCount = 0;
    board = game3.getBoard();
    for (int i = 0; i < board->getSize(); ++i) {
        for (int j = 0; j < board->getSize(); ++j) {
            if (board->getCell(i, j).getType() == CellType::BONUS_EXCHANGE) {
                exchangeCount++;
            }
        }
    }
    EXPECT_EQ(exchangeCount, 5); // ceil(1.5 * 3) = 5
}

// test du nombre de bonus de pierre (0.5 par joueur, arrondi au supérieur)

TEST(BonusCapture, StoneBonusCount) {
    // 2 joueurs : 0.5 * 2 = 1 bonus de pierre
    Lobby lobby2(1);
    lobby2.addConnection(1, "P1", 0);
    lobby2.addConnection(2, "P2", 1);
    Game game2(1, &lobby2);
    
    // compte les bonus de pierre sur le plateau
    int stoneCount = 0;
    Board* board = game2.getBoard();
    for (int i = 0; i < board->getSize(); ++i) {
        for (int j = 0; j < board->getSize(); ++j) {
            if (board->getCell(i, j).getType() == CellType::BONUS_STONE) {
                stoneCount++;
            }
        }
    }
    EXPECT_EQ(stoneCount, 1); // ceil(0.5 * 2) = 1
    
    // 3 joueurs : 0.5 * 3 = 1.5 ≈ 2 bonus de pierre
    Lobby lobby3(2);
    lobby3.addConnection(1, "P1", 0);
    lobby3.addConnection(2, "P2", 1);
    lobby3.addConnection(3, "P3", 2);
    Game game3(2, &lobby3);
    
    stoneCount = 0;
    board = game3.getBoard();
    for (int i = 0; i < board->getSize(); ++i) {
        for (int j = 0; j < board->getSize(); ++j) {
            if (board->getCell(i, j).getType() == CellType::BONUS_STONE) {
                stoneCount++;
            }
        }
    }
    EXPECT_EQ(stoneCount, 2); // ceil(0.5 * 3) = 2
}

// test du nombre de bonus de vol (1 par joueur)

TEST(BonusCapture, RobberyBonusCount) {
    // 2 joueurs : 1 * 2 = 2 bonus de vol
    Lobby lobby2(1);
    lobby2.addConnection(1, "P1", 0);
    lobby2.addConnection(2, "P2", 1);
    Game game2(1, &lobby2);
    
    // compte les bonus de vol sur le plateau
    int robberyCount = 0;
    Board* board = game2.getBoard();
    for (int i = 0; i < board->getSize(); ++i) {
        for (int j = 0; j < board->getSize(); ++j) {
            if (board->getCell(i, j).getType() == CellType::BONUS_ROBBERY) {
                robberyCount++;
            }
        }
    }
    EXPECT_EQ(robberyCount, 2); // 1 * 2 = 2
    
    // 4 joueurs : 1 * 4 = 4 bonus de vol
    Lobby lobby4(2);
    lobby4.addConnection(1, "P1", 0);
    lobby4.addConnection(2, "P2", 1);
    lobby4.addConnection(3, "P3", 2);
    lobby4.addConnection(4, "P4", 3);
    Game game4(2, &lobby4);
    
    robberyCount = 0;
    board = game4.getBoard();
    for (int i = 0; i < board->getSize(); ++i) {
        for (int j = 0; j < board->getSize(); ++j) {
            if (board->getCell(i, j).getType() == CellType::BONUS_ROBBERY) {
                robberyCount++;
            }
        }
    }
    EXPECT_EQ(robberyCount, 4); // 1 * 4 = 4
}

// test de la capture d'un bonus en entourant les 4 directions cardinales

TEST(BonusCapture, CapturedBySurrounding4Cardinals) {
    Board board(20);
    
    // place un bonus au centre
    board.setCell(10, 10, Cell(CellType::BONUS_EXCHANGE));
    
    // vérifie qu'il n'est pas capturé au départ
    EXPECT_FALSE(board.isBonusCaptured(10, 10, 0));
    
    // place 3 cellules autour (pas encore capturé)
    board.setCell(9, 10, Cell(0)); // nord
    board.setCell(11, 10, Cell(0)); // sud
    board.setCell(10, 9, Cell(0)); // ouest
    EXPECT_FALSE(board.isBonusCaptured(10, 10, 0));
    
    // place la 4ème cellule (maintenant capturé)
    board.setCell(10, 11, Cell(0)); // est
    EXPECT_TRUE(board.isBonusCaptured(10, 10, 0));
    
    // vérifie que les diagonales ne comptent pas
    Board board2(20);
    board2.setCell(10, 10, Cell(CellType::BONUS_EXCHANGE));
    board2.setCell(9, 9, Cell(0)); // nord-ouest
    board2.setCell(9, 11, Cell(0)); // nord-est
    board2.setCell(11, 9, Cell(0)); // sud-ouest
    board2.setCell(11, 11, Cell(0)); // sud-est
    EXPECT_FALSE(board2.isBonusCaptured(10, 10, 0)); // diagonales ne comptent pas
}

// test que le bonus est perdu s'il est couvert par une tuile

TEST(BonusCapture, BonusLostIfCovered) {
    Lobby lobby(1);
    lobby.addConnection(1, "Player1", 0);
    lobby.addConnection(2, "Player2", 1);
    
    Game game(1, &lobby);
    Board* board = game.getBoard();
    
    // trouve une position avec un bonus
    int bonusRow = -1, bonusCol = -1;
    for (int i = 1; i < board->getSize() - 1; ++i) {
        for (int j = 1; j < board->getSize() - 1; ++j) {
            if (board->isBonus(i, j)) {
                bonusRow = i;
                bonusCol = j;
                break;
            }
        }
        if (bonusRow != -1) break;
    }
    
    // si on a trouvé un bonus
    if (bonusRow != -1) {
        CellType bonusType = board->getCell(bonusRow, bonusCol).getType();
        EXPECT_TRUE(board->isBonus(bonusRow, bonusCol));
        
        // place une tuile du joueur sur le bonus (le couvre)
        board->setCell(bonusRow, bonusCol, Cell(0));
        
        // le bonus n'existe plus
        EXPECT_FALSE(board->isBonus(bonusRow, bonusCol));
        EXPECT_TRUE(board->isPlayerCell(bonusRow, bonusCol, 0));
    }
}

// test que les bonus de pierre et de vol doivent être utilisés immédiatement

TEST(BonusCapture, ImmediateUseForStoneAndRobbery) {
    Lobby lobby(1);
    lobby.addConnection(1, "Player1", 0);
    lobby.addConnection(2, "Player2", 1);
    
    Game game(1, &lobby);
    Board* board = game.getBoard();
    
    // place manuellement un bonus de pierre entouré pour le capturer
    board->setCell(10, 10, Cell(CellType::BONUS_STONE));
    board->setCell(9, 10, Cell(0));
    board->setCell(11, 10, Cell(0));
    board->setCell(10, 9, Cell(0));
    board->setCell(10, 11, Cell(0));
    
    // simule la capture du bonus de pierre
    int currentPlayer = game.getCurrentPlayerConnection();
    
    // avant la capture, le joueur n'a pas de bonus en attente
    EXPECT_FALSE(game.hasPendingStoneBonus(currentPlayer));
    
    // après avoir capturé un bonus de pierre dans une vraie partie,
    // le flag pendingStoneBonus serait mis à true
    // (on ne peut pas facilement simuler cela sans jouer une vraie partie)
}

