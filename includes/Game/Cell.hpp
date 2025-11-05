#pragma once

// Types de cellules possibles
enum class CellType {
    EMPTY = -1,      // Cellule vide
    GRASS = 0,       // Herbe (territoire d'un joueur)
    STONE = 99,      // Pierre (obstacle)
    BONUS_EXCHANGE = 100,  // Bonus d'échange de tuile
    BONUS_STONE = 101,     // Bonus de pierre
    BONUS_ROBBERY = 102    // Bonus de vol
};

class Cell {
public:
    Cell();
    Cell(int playerId); // Cellule avec un joueur (GRASS)
    Cell(CellType type); // Cellule de type spécial (STONE, BONUS, etc.)
    
    // Getters
    bool isEmpty() const { return type == CellType::EMPTY; }
    bool isGrass() const { return type == CellType::GRASS; }
    bool isStone() const { return type == CellType::STONE; }
    bool isBonus() const { return type == CellType::BONUS_EXCHANGE || 
                                  type == CellType::BONUS_STONE || 
                                  type == CellType::BONUS_ROBBERY; }
    
    int getPlayerId() const { return playerId; } // Retourne le joueur propriétaire (-1 si vide)
    CellType getType() const { return type; }
    
    // Setters
    void setPlayer(int playerId); // Définit un joueur propriétaire (passe en GRASS)
    void setType(CellType newType);
    void clear(); // Remet la cellule à vide
    
    // Conversion pour compatibilité réseau (int)
    int toInt() const; // Convertit en int pour les packets réseau
    static Cell fromInt(int value); // Crée une Cell depuis un int du réseau
    
    // Opérateurs de comparaison
    bool operator==(const Cell& other) const;
    bool operator!=(const Cell& other) const;

private:
    CellType type; // Type de la cellule
    int playerId;  // ID du joueur propriétaire (-1 si aucun joueur)
};

