#pragma once
#include "Cell.hpp"
#include <vector>

class Board {
public:
    Board(int size = 20); // construit un plateau carré de taille « size »
    
    int getSize() const { return size; } // retourne la taille du plateau
    

    const Cell& getCell(int row, int col) const; // lit la cellule aux coordonnées (row, col)
    void setCell(int row, int col, const Cell& cell); // écrit une cellule aux coordonnées (row, col)
    
    int getCellValue(int row, int col) const; // retourne la valeur réseau/int de la cellule
    void setCellValue(int row, int col, int value); // définit la cellule depuis une valeur int
    
    void reset(); // remet toutes les cellules à vide
    
    bool isEmpty(int row, int col) const; // true si la cellule est vide
    bool isPlayerCell(int row, int col, int playerId) const; // true si la cellule appartient au joueur
    int getPlayerId(int row, int col) const; // id du joueur propriétaire (-1 si aucun)
    
    bool isValidPosition(int row, int col) const; // coordonnées dans les bornes du plateau
    bool hasAdjacentPlayerCell(int row, int col, int playerId) const; // adjacence orthogonale au territoire du joueur
    bool touchesOtherPlayerTerritory(int row, int col, int playerId) const; // touche le territoire d'un autre joueur
    bool isBonus(int row, int col) const; // true si la cellule contient un bonus
    bool isValidBonusPosition(int row, int col) const; // true si la position est valide pour placer un bonus (pas sur bord, pas adjacent à un autre bonus)
    bool isBonusCaptured(int row, int col, int playerId) const; // true si un bonus est capturé par un joueur (4 directions cardinales)
    
    std::vector<std::vector<Cell>> grid; // grille 2D de cellules
    int size; // dimension du plateau (size x size)
};
 
 