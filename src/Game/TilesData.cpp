#include "Game/TilesData.hpp"
#include <vector>

namespace TilesData {
    static std::vector<Tile> allTiles;
    static bool initialized = false;
    
    void initialize() {
        if (initialized) return;
        
        allTiles.clear();
        allTiles.reserve(96);
        
        // TODO: Compléter avec les 96 tuiles de l'image
        // Format: Tile({{row1, col1}, {row2, col2}, ...})
        // Exemple pour une tuile 1x1 (monomino):
        
        // Tuile 0: Monomino (1 bloc)
        allTiles.push_back(Tile({{0, 0}}));
        
        // Tuile 1: L couché gauche haut
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {-1,0}}));
        
        // Tuile 2: T à l'envers
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {-1, 1}, {-2,1}}));
        
        // Tuile 3: +
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {-1, 1}, {1, 1}}));
        
        // Tuile 4: Z couché 
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {-1, 2}, {1, 0}}));
        
        // Tuile 5: mini T à l'envers
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {-1, 1}}));
        
        // Tuile 6: carré 2x2
        allTiles.push_back(Tile({{0, 1}, {-1, 0}, {-1, 1}, {0, 0}}));
        
        // Tuile 7: crochet haut
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {0, 1}, {0, 2},{-1, 2}}));
        
        // Tuile 8: i couché
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}}));
        
        // Tuile 9: éclair horizontal
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 0}, {-1, 1}}));
        
        // Tuile 10: angle droit
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 0}}));

        // escaliers
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 1}, {-1, 2}, {-2, 2}}));
        
        // 2
        allTiles.push_back(Tile({{0, 0}, {0, 1}}));
        
        // crochet a l'envers 
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 0}, {-2, 0}, {-3, 0}, {-3, 1}, {-4, 1}}));
        
        // L à l'envers
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-3, 0}, {-4, 0}, {-4, 1}, {-4, 2}}));
        
        // 
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 1}, {-1, 2}, {-1, 3}, {-2, 3}, {-2, 4}, {-3, 3}, {-4, 3}}));
        
        
        //
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-2, 1}, {-2, 2}, {-3, 2}}));
        
        // 
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 0}, {-1, 1}, {-2, 1}, {-1, 2}}));
        
        // 3
        allTiles.push_back(Tile({{0, 1}, {1, 0}, {1, 1}, {1, 2}, {2, 1}}));
        
        // Pentomino Y
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {2, 0}, {2, 1}, {3, 0}}));
        
        // Pentomino Z
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {2, 1}, {2, 2}}));
        
        // Pentomino F
        allTiles.push_back(Tile({{0, 1}, {0, 2}, {1, 0}, {1, 1}, {2, 1}}));
        
        // Hexomino I (ligne de 6)
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}}));
        
        // Hexomino rectangle 2x3
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}}));
        
        // Hexomino rectangle 3x2
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 0}, {2, 1}}));
        
        // Tuile 25: Pentomino N (première forme)
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {1, 1}, {2, 1}, {2, 2}}));
        
        // Tuile 26: Pentomino N (forme miroir)
        allTiles.push_back(Tile({{0, 1}, {0, 2}, {1, 0}, {1, 1}, {2, 0}}));
        
        // Hexominoes (6 blocs) - formes variées
        // Tuile 27: Hexomino ligne verticale
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}}));
        
        // Tuile 28: Hexomino rectangle 2x3
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}}));
        
        // Tuile 29: Hexomino rectangle 3x2
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 0}, {2, 1}}));
        
        // Tuile 30: Hexomino L (grand)
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {4, 1}}));
        
        // Tuile 31: Hexomino T (grand)
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 1}, {2, 1}, {3, 1}}));
        
        // Tuile 32: Hexomino croix
        allTiles.push_back(Tile({{0, 1}, {1, 0}, {1, 1}, {1, 2}, {2, 1}, {3, 1}}));
        
        // Tuile 33: Hexomino escalier
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 3}}));
        
        // Tuile 34: Hexomino zigzag
        allTiles.push_back(Tile({{0, 1}, {0, 2}, {1, 0}, {1, 1}, {2, 0}, {3, 0}}));
        
        // Tuile 35: Hexomino U (grand)
        allTiles.push_back(Tile({{0, 0}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 2}}));
        
        // Tuile 36: Hexomino rectangle avec extension
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {2, 1}}));
        
        // Tuile 37: Hexomino L avec extension
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {2, 0}, {2, 1}, {3, 1}, {3, 2}}));
        
        // Tuile 38: Hexomino double escalier
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {2, 1}, {2, 2}, {3, 2}}));
        
        // Tuile 39: Hexomino forme complexe
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 0}, {2, 1}}));
        
        // Tuile 40: Hexomino carré avec deux extensions
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 0}, {2, 2}}));
        
        // Heptominoes (7 blocs)
        // Tuile 41: Heptomino ligne
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}}));
        
        // Tuile 42: Heptomino rectangle 2x4 (avec un manquant)
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 1}, {1, 3}}));
        
        // Tuile 43: Heptomino L (grand)
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {5, 1}}));
        
        // Tuile 44: Heptomino T (grand)
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 1}, {2, 1}, {3, 1}, {4, 1}}));
        
        // Tuile 45: Heptomino croix
        allTiles.push_back(Tile({{0, 1}, {1, 0}, {1, 1}, {1, 2}, {2, 1}, {3, 1}, {4, 1}}));
        
        // Tuile 46: Heptomino U (grand)
        allTiles.push_back(Tile({{0, 0}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 2}}));
        
        // Tuile 47: Heptomino escalier
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 3}, {3, 3}}));
        
        // Tuile 48: Heptomino rectangle 3x3 (avec 2 manquants)
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 2}, {2, 0}, {2, 1}}));
        
        // Tuile 49: Heptomino forme complexe
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2}}));
        
        // Tuile 50: Heptomino L avec double extension
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {2, 0}, {2, 1}, {3, 1}, {3, 2}, {4, 2}}));
        
        // Tuiles de 8 blocs (Octominoes)
        // Tuile 51: Octomino ligne
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}}));
        
        // Tuile 52: Octomino rectangle 2x4
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 1}, {1, 2}, {1, 3}}));
        
        // Tuile 53: Octomino rectangle 4x2
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 0}, {2, 1}, {3, 0}, {3, 1}}));
        
        // Tuile 54: Octomino L (très grand)
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {6, 1}}));
        
        // Tuile 55: Octomino T (grand)
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}}));
        
        // Tuile 56: Octomino carré 3x3 (avec 1 manquant)
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 1}}));
        
        // Tuile 57: Octomino U (grand)
        allTiles.push_back(Tile({{0, 0}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 2}, {3, 0}}));
        
        // Tuile 58: Octomino forme complexe
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2}, {3, 1}}));
        
        // Tuile 59: Octomino escalier
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 3}, {3, 3}, {3, 4}}));
        
        // Tuile 60: Octomino zigzag
        allTiles.push_back(Tile({{0, 1}, {0, 2}, {1, 0}, {1, 1}, {2, 1}, {2, 2}, {3, 0}, {3, 1}}));
        
        // Tuiles de 9 blocs (Enneominoes)
        // Tuile 61: Enneomino ligne
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}}));
        
        // Tuile 62: Enneomino rectangle 3x3
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2}}));
        
        // Tuile 63: Enneomino L (très grand)
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {7, 1}}));
        
        // Tuile 64: Enneomino T (grand)
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}}));
        
        // Tuile 65: Enneomino rectangle 3x3 (avec extensions)
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {3, 0}}));
        
        // Tuile 66: Enneomino forme complexe
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2}, {3, 1}, {3, 2}}));
        
        // Tuiles de 10+ blocs (variantes)
        // Tuile 67: Décomino ligne (10 blocs)
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 9}}));
        
        // Tuile 68: Rectangle 2x5
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}}));
        
        // Tuile 69: Rectangle 5x2
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 0}, {2, 1}, {3, 0}, {3, 1}, {4, 0}, {4, 1}}));
        
        // Tuile 70: L avec extensions multiples
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {5, 1}, {5, 2}, {6, 2}, {7, 2}}));
        
        // Tuile 71: T avec extensions
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}}));
        
        // Tuile 72: Forme complexe avec creux
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 2}, {2, 0}, {2, 1}, {2, 2}, {3, 0}, {3, 1}}));
        
        // Tuile 73: Escalier long
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 3}, {3, 3}, {3, 4}, {4, 4}, {4, 5}}));
        
        // Tuile 74: Zigzag long
        allTiles.push_back(Tile({{0, 1}, {0, 2}, {1, 0}, {1, 1}, {2, 1}, {2, 2}, {3, 0}, {3, 1}, {4, 1}, {4, 2}}));
        
        // Tuile 75: Rectangle 3x4 (avec 2 manquants)
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 1}, {1, 3}, {2, 0}, {2, 1}, {2, 2}}));
        
        // Tuile 76: Forme en spirale
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2}, {3, 0}, {3, 1}, {4, 1}}));
        
        // Tuile 77: Croix avec extensions
        allTiles.push_back(Tile({{0, 1}, {1, 0}, {1, 1}, {1, 2}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 0}, {6, 2}}));
        
        // Tuile 78: U avec extensions
        allTiles.push_back(Tile({{0, 0}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 2}, {3, 0}, {3, 1}, {4, 0}}));
        
        // Tuile 79: Forme complexe mixte
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2}, {3, 1}, {3, 2}, {4, 2}}));
        
        // Tuile 80: Rectangle avec protubérances
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {3, 1}, {4, 1}}));
        
        // Tuile 81: L avec double branche
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {2, 0}, {3, 0}, {3, 1}, {4, 1}, {4, 2}, {5, 2}, {5, 3}, {6, 3}}));
        
        // Tuile 82: T avec double branche
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {4, 0}, {4, 2}, {5, 1}}));
        
        // Tuile 83: Forme en H
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {2, 0}, {1, 1}, {2, 1}, {3, 1}, {0, 2}, {1, 2}, {2, 2}, {3, 2}}));
        
        // Tuile 84: Forme en X (grande)
        allTiles.push_back(Tile({{0, 1}, {1, 0}, {1, 1}, {1, 2}, {2, 1}, {3, 0}, {3, 1}, {3, 2}, {4, 1}, {5, 1}}));
        
        // Tuile 85: Rectangle 4x3 (avec 2 manquants)
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 2}, {3, 0}, {3, 1}}));
        
        // Tuile 86: Forme complexe avec angles
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2}, {3, 1}, {3, 2}, {4, 0}, {4, 1}}));
        
        // Tuile 87: Zigzag complexe
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {3, 1}, {3, 2}, {4, 0}, {4, 1}, {5, 1}}));
        
        // Tuile 88: L avec triple extension
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {4, 1}, {5, 1}, {5, 2}, {6, 2}, {6, 3}, {7, 3}}));
        
        // Tuile 89: Rectangle presque complet
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 1}, {1, 3}, {2, 0}, {2, 1}, {2, 2}, {2, 3}}));
        
        // Tuile 90: Forme en étoile
        allTiles.push_back(Tile({{0, 1}, {1, 0}, {1, 1}, {1, 2}, {2, 1}, {3, 0}, {3, 1}, {3, 2}, {4, 1}, {5, 0}, {5, 2}}));
        
        // Tuile 91: Spiral complexe
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2}, {3, 0}, {3, 1}, {4, 1}, {4, 2}, {5, 2}}));
        
        // Tuile 92: Rectangle 3x4 (avec 1 manquant)
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {2, 0}, {2, 1}, {2, 3}}));
        
        // Tuile 93: Forme complexe avec plusieurs angles
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2}, {3, 1}, {3, 2}, {4, 0}, {4, 1}, {5, 1}}));
        
        // Tuile 94: Grande forme L
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {6, 1}, {6, 2}, {7, 2}, {7, 3}, {8, 3}}));
        
        // Tuile 95: Dernière tuile - forme complexe finale
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 2}, {2, 0}, {2, 1}, {2, 2}, {3, 1}, {4, 0}, {4, 1}, {5, 1}}));
        
        initialized = true;
    }
    
    const std::vector<Tile>& getAllTiles() {
        if (!initialized) {
            initialize();
        }
        return allTiles;
    }
    
    const Tile& getTile(int index) {
        if (!initialized) {
            initialize();
        }
        if (index < 0 || index >= static_cast<int>(allTiles.size())) {
            static Tile emptyTile;
            return emptyTile;
        }
        return allTiles[index];
    }
}

