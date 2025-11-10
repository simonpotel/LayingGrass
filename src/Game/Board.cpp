#include "Game/Board.hpp"
#include <iostream>

Board::Board(int size) : size(size) { 
    // initialise la grille avec des cellules vides
    grid.resize(size);
    for (int i = 0; i < size; ++i) {
        grid[i].resize(size, Cell(CellType::EMPTY));
    }
}

const Cell& Board::getCell(int row, int col) const {
    static const Cell emptyCell = Cell(CellType::EMPTY);
    
    if (row < 0 || row >= size || col < 0 || col >= size) {
        return emptyCell;
    }
    return grid[row][col];
}

void Board::setCell(int row, int col, const Cell& cell) {
    if (row >= 0 && row < size && col >= 0 && col < size) {
        grid[row][col] = cell;
    }
}

int Board::getCellValue(int row, int col) const {
    return getCell(row, col).toInt();
}

void Board::setCellValue(int row, int col, int value) {
    setCell(row, col, Cell::fromInt(value));
}

void Board::reset() {
    // réinitialise la grille avec des cellules vides
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            grid[i][j] = Cell(CellType::EMPTY);
        }
    }
}

bool Board::isEmpty(int row, int col) const {
    return getCell(row, col).isEmpty();
}

bool Board::isPlayerCell(int row, int col, int playerId) const {
    const Cell& cell = getCell(row, col);
    return cell.isGrass() && cell.getPlayerId() == playerId;
}

int Board::getPlayerId(int row, int col) const {
    return getCell(row, col).getPlayerId();
}

bool Board::isValidPosition(int row, int col) const {
    return row >= 0 && row < size && col >= 0 && col < size;
}

bool Board::hasAdjacentPlayerCell(int row, int col, int playerId) const {
    // Vérifie les 4 directions cardinales (haut, bas, gauche, droite)
    const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    
    for (int i = 0; i < 4; ++i) {
        int newRow = row + directions[i][0];
        int newCol = col + directions[i][1];
        
        if (isValidPosition(newRow, newCol) && isPlayerCell(newRow, newCol, playerId)) {
            return true;
        }
    }
    
    return false;
}

bool Board::touchesOtherPlayerTerritory(int row, int col, int playerId) const {
    // Vérifie les 4 directions cardinales
    const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    
    for (int i = 0; i < 4; ++i) {
        int newRow = row + directions[i][0];
        int newCol = col + directions[i][1];
        
        if (isValidPosition(newRow, newCol)) {
            const Cell& cell = getCell(newRow, newCol);
            // Si c'est une cellule d'un autre joueur
            if (cell.isGrass() && cell.getPlayerId() != playerId) {
                return true;
            }
        }
    }
    
    return false;
}

bool Board::isBonus(int row, int col) const {
    return getCell(row, col).isBonus();
}

bool Board::isValidBonusPosition(int row, int col) const {
    // vérifie que la position est valide et pas sur un bord
    if (row <= 0 || row >= size - 1 || col <= 0 || col >= size - 1) {
        return false; // sur un bord
    }
    
    // vérifie que la cellule est vide
    if (!isEmpty(row, col)) {
        return false;
    }
    
    // vérifie qu'il n'y a pas de bonus adjacent (4 directions cardinales)
    const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (int i = 0; i < 4; ++i) {
        int newRow = row + directions[i][0];
        int newCol = col + directions[i][1];
        if (isValidPosition(newRow, newCol) && isBonus(newRow, newCol)) {
            return false; // adjacent à un autre bonus
        }
    }
    
    return true; // position valide
}

bool Board::isBonusCaptured(int row, int col, int playerId) const {
    // vérifie que c'est bien un bonus
    if (!isBonus(row, col)) {
        return false;
    }
    
    // vérifie que les 4 directions cardinales sont occupées par le même joueur
    const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    
    for (int i = 0; i < 4; ++i) {
        int newRow = row + directions[i][0];
        int newCol = col + directions[i][1];
        
        if (!isValidPosition(newRow, newCol)) {
            return false;
        }
        
        if (!isPlayerCell(newRow, newCol, playerId)) {
            return false;
        }
    }
    
    return true; // toutes les 4 directions sont occupées par ce joueur
}


