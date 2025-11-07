#pragma once

enum class CellType {
    EMPTY = -1,            // Cellule vide
    GRASS = 0,             // Herbe (territoire d'un joueur)
    STONE = 99,            // Pierre (obstacle)
    BONUS_EXCHANGE = 100,  // Bonus d'échange de tuile
    BONUS_STONE = 101,     // Bonus de pierre
    BONUS_ROBBERY = 102    // Bonus de vol
};

// Représente une cellule du plateau (vide, herbe d'un joueur, obstacle ou bonus)
class Cell {
public:
    Cell();
    Cell(int playerId); // cellule avec un joueur (GRASS)
    Cell(CellType type); // cellule de type spécial (STONE, BONUS, etc.)
    
    bool isEmpty() const { return type == CellType::EMPTY; }
    bool isGrass() const { return type == CellType::GRASS; }
    bool isStone() const { return type == CellType::STONE; }
    bool isBonus() const { return type == CellType::BONUS_EXCHANGE || 
                                  type == CellType::BONUS_STONE || 
                                  type == CellType::BONUS_ROBBERY; }
    
    int getPlayerId() const { return playerId; } // retourne le joueur propriétaire (-1 si vide)
    CellType getType() const { return type; }
    
    void setPlayer(int playerId); // définit un joueur propriétaire (passe en GRASS)
    void setType(CellType newType); // change le type de la cellule
    void clear(); // remet la cellule à vide
    
    // conversion pour compatibilité réseau (int)
    int toInt() const; // convertit en int pour les packets réseau
    static Cell fromInt(int value); // crée une Cell depuis un int du réseau
    
    // opérateurs de comparaison
    bool operator==(const Cell& other) const; // égalité structurelle
    bool operator!=(const Cell& other) const; // inégalité structurelle

private:
    CellType type; 
    int playerId;  // ID du joueur propriétaire (-1 si aucun joueur)
};

