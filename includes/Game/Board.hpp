#pragma once
#include "Game/Cell.hpp"
#include <vector>

class Board {
public:
    Board(int size = 20); // constructeur de la grille avec une taille par défaut de 20
    
    int getSize() const { return size; }
    
    // Méthodes utilisant Cell (recommandées)
    const Cell& getCell(int row, int col) const;
    void setCell(int row, int col, const Cell& cell);
    
    // Méthodes de compatibilité (pour migration progressive)
    int getCellValue(int row, int col) const; // Retourne la valeur int pour compatibilité
    void setCellValue(int row, int col, int value); // Définit depuis une valeur int
    
    void reset();
    
    // Méthodes utilitaires
    bool isEmpty(int row, int col) const;
    bool isPlayerCell(int row, int col, int playerId) const;
    int getPlayerId(int row, int col) const;
    
    // Vérifications pour le placement de tuiles
    bool isValidPosition(int row, int col) const; // Vérifie si la position est dans les limites
    bool hasAdjacentPlayerCell(int row, int col, int playerId) const; // Vérifie si une cellule a un voisin du joueur
    bool touchesOtherPlayerTerritory(int row, int col, int playerId) const; // Vérifie si une cellule touche le territoire d'un autre joueur
    
    std::vector<std::vector<Cell>> grid; // grille de cellules
    int size; // taille de la grille
};

