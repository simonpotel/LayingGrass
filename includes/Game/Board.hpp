#pragma once
#include <vector>

class Board {
public:
    Board(int size = 20); // constructeur de la grille avec une taille par défaut de 20
    
    int getSize() const { return size; }
    int getCell(int row, int col) const;
    void setCell(int row, int col, int value);
    void reset();
    
    std::vector<std::vector<int>> grid; // grille de la grille
    int size; // taille de la grille
};

