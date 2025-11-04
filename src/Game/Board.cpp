#include "Game/Board.hpp"

Board::Board(int size) : size(size) { 
    // initialise la grille avec -1 (cellule vide)
    grid.resize(size);
    for (int i = 0; i < size; ++i) {
        grid[i].resize(size, -1);
    }
}

int Board::getCell(int row, int col) const {
    // retourne la valeur d'une cellule (valeur -1 si la cellule est vide)
    if (row < 0 || row >= size || col < 0 || col >= size) {
        return -1;
    }
    return grid[row][col];
}

void Board::setCell(int row, int col, int value) {
    // définit la valeur d'une cellule (valeur -1 si la cellule est vide)
    if (row >= 0 && row < size && col >= 0 && col < size) {
        grid[row][col] = value;
    }
}

void Board::reset() {
    // réinitialise la grille avec -1 (cellule vide)
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            grid[i][j] = -1;
        }
    }
}

