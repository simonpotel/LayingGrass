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

        // Tuile 11: escaliers
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 1}, {-1, 2}, {-2, 2}}));
        
        // Tuile 12: 2
        allTiles.push_back(Tile({{0, 0}, {0, 1}}));
        
        // Tuile 13: crochet a l'envers 
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 0}, {-2, 0}, {-3, 0}, {-3, 1}, {-4, 1}}));
        
        // Tuile 14: L à l'envers
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-3, 0}, {-4, 0}, {-4, 1}, {-4, 2}}));
        
        // Tuile 15: 
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 1}, {-1, 2}, {-1, 3}, {-2, 3}, {-2, 4}, {-3, 3}, {-4, 3}}));
        
        
        // Tuile 16: 
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-2, 1}, {-2, 2}, {-3, 2}}));
        
        // Tuile 17: 
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 0}, {-1, 1}, {-2, 1}, {-1, 2}}));
        
        // Tuile 18: 
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-2, 1}, {-3, 0}}));
        
        // Tuile 19: 
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {0, 1}, {-1, 1}, {-2, 1}, {-2, 2}}));
        
        // Tuile 20: 
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {-1, 0}, {-1, 1}, {-2, 1}}));
        
        // Tuile 21: 
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-1, 2}, {-2, 1}, {-3, 1}}));
        
        // Tuile 22: 
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 0}, {-1, 1}, {-2, 0}, {-2, 1}}));
        
        // Tuile 23: T
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-3, 0}, {-3, -1}, {-3, 1}}));
        
        // Tuile 24: escaliers descendants droite
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {1, 1}, {2, 1}, {2, 2}, {3, 2}, {3, 3}}));
        
        // Tuile 25: 5
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}}));
        
        // Tuile 26:
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {1, 1}, {1, 2}, {2, 2}, {3, 2}}));
        
        // Tuile 27:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-2, 0}, {-3, 0}, {-3, 1}, {-4, 0}}));
        
        // Tuile 28
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-2, 0}, {-2, 1}, {-3, 0}}));
        
        // Tuile 29:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-1, 2}, {0, 2}, {-2, 1}, {-3, 1}, {-4, 1}}));
        
        // Tuile 30:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-1, 1}, {-1, 2}, {0, 2}, {0, 3}}));
        
        // Tuile 31:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-2, 1}, {-3, 1}, {-4, 1}}));
        
        // Tuile 32:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-2, 1}, {-2, 2}, {-2, 3}, {-3, 1}, {-4, 1}, {-4, 0}}));
    
        // Tuile 33: 
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-2, 0}, {-3, 0}}));
        
        // Tuile 34:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {0, 1}, {0, 2}, {0, 3}, {-1, 3}}));
        
        // Tuile 35:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {-1, 4}, {-2, 4}, {-2, 3}}));
        
        // Tuile 36:
        allTiles.push_back(Tile({{0, 0}, {1, 0}, {2, 0}, {2, 1}, {3, 1}, {3, 2}, {3, 3}}));
        
        // Tuile 37: 
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-2, 0}, {-2, -1}, {-3, 0}, {-3, 1}, {-4, 0}, {-4, -1}}));
          
        // Tuile 38: 
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 0}, {-1, 1}, {-2, 0}}));
        
        // Tuile 39:
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {0, 3}, {-1, 1}, {-2, 1}}));
        
        // Tuile 40:carré 
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 0}, {-1, 1}}));
        
        // Tuile 41: 
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-2, 0}, {-2, 1}, {-3, 1}}));
        
        // Tuile 42: J
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 1}, {-2, 0}, {-2, 1}, {-2, 2}}));
        
        // Tuile 43: 4
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {0, 3}}));
        
        // Tuile 44:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {0, 1}, {0, 2}, {0, 3}}));
        
        // Tuile 45:
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {-1, 0}, {-1, -1}, {-2, 0}, {-2, 1}}));
        
        // Tuile 46:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-2, 1}, {-3, 1}}));
        
        // Tuile 47:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {0, 1}, {0, 2}}));
        
        // Tuile 48:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-3, 0}, {-3, 1}, {-4, 0}}));
        
        // Tuile 49:
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {-1, 2}, {-2, 2}, {-3, 2}, {-4, 2}, {-4, 3}, {-4, 4}}));
        
        // Tuile 50:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-2, 0}, {-3, 0}, {-3, 1}, {-4, 1}, {-5, 1}}));
        
        // Tuile 51:
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {1, 1}, {2, 1}, {2, 2}, {2, 3}, {3, 3}}));
        
        // Tuile 52: 
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-2, -1}, {-2, 1}, {-3, 0}, {-4, 0}}));
        
        // Tuile 53:
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {-1, 2}, {-2, 2}}));
        
        // Tuile 54: I en 6 
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-3, 0}, {-4, 0}, {-5, 0}}));
        
        // Tuile 55: Y
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 1}, {-2, 1}, {-2, 0}, {-2, 2}, {-3, 0}, {-3, 2}}));
        
        // Tuile 56: 
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-1, 2}, {-2, 0}, {-2, -1}, {-2, 1}, {-3, 1}}));
        
        // Tuile 57:
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {-1, 1}, {-2, 1}, {-3, 1}, {-3, 2}}));
        
        // Tuile 58:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-2, 1}, {-3, 0}, {-4, 0}}));
        
        // Tuile 59: Octomino escalier
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 1}, {-2, 1}, {-3, 1}, {-2, 0}, {0, 2}, {0, 3}, {-1, 3}}));
        
        // Tuile 60: 
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 0}, {-2, 0}, {-2, 1}, {-3, 1}, {-3, 2}, {-4, 2}}));
        
        // Tuile 61:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-2, 1}, {-3, 1}, {-4, 1}, {-5, 1}, {-5, 2}}));
        
        // Tuile 62:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {0, 1}, {0, 2}, {-1, 2}, {0, 3}}));
        
        // Tuile 63:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-3, 0}, {-3, -1}, {0, 1}, {0, 2}, {-1, 2}, {-2, 2}}));
        
        // Tuile 64:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-2, -1}, {-3, -1}, {-4, -1}, {-2, 1}, {-3, 1}, {-2, 2}}));
        
        // Tuile 65:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-3, 0}, {-4, 0}, {-5, 0}, {-6, 0}, {-6, 1}}));
        
        // Tuile 66: 
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-2, 1}, {-3, 1}, {-2, 2}}));

        // Tuile 67: 
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {0, 1}, {0, 2}, {-1, 2}, {-2, 2}}));
        
        // Tuile 68:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-2, 1}, {-1, 2}, {0, 2}}));
        
        // Tuile 69: +
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-2, -1}, {-2, -2}, {-2, 1}, {-2, 2}, {-3, 0}, {-4, 0}}));
        
        // Tuile 70:
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 1}, {-1, 2}, {-2, 2}, {-3, 2}, {-4, 2}, {-4, 3}, {-5, 3}}));
        
        // Tuile 71: E
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 0}, {-2, 0}, {-2, 1}, {-3, 0}, {-4, 0}, {-4, 1}}));
        
        // Tuile 72:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, -1}, {-1, 1}, {-1, 2}, {-2, 0}, {-3, 0}, {-4, 0}}));
        
        // Tuile 73:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-3, 0}, {-3, 1}, {-4, 1}}));
        
        // Tuile 74:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-2, 1}, {-3, 1}, {-3, 2}}));
        
        // Tuile 75: 
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-2, 1}, {-3, 0}, {-3, -1}, {-4, 0}, {-5, 0}, {-5, 1}}));
        
        // Tuile 76:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-3, 0}, {-3, 1}, {-4, 0}, {-4, -1}, {-5, 0}, {-5, -1}}));
        
        // Tuile 77:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, -1}, {-2, 0}, {-2, 1}, {-3, 0}}));
        
        // Tuile 78:
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 0}, {-1, 1}, {0, 2}, {0, 3}}));
        
        // Tuile 79:
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 0}, {-2, 0}, {-2, 1}, {-2, 2}, {-3, 2}, {-3, 3}}));
        
        // Tuile 80:
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 1}, {-2, 1}, {-3, 1}, {-3, 2}}));
        
        // Tuile 81:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-3, 0}, {-1, 1}, {-1, -1}}));
        
        // Tuile 82:
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 0}, {-2, 0}, {-3, 0}, {-3, 1}, {-3, 2}, {-2, 2}}));
        
        // Tuile 83:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-2, 1}, {-2, 2}, {-3, 1}}));
        
        // Tuile 84: escaliers pleins
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {0, 2}, {-1, 0}, {-1, 1}, {-2, 0}}));
        
        // Tuile 85: 
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 0}, {-2, 0}, {-3, 0}}));
        
        // Tuile 86:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-1, 1}, {-1, 2}, {-2, 1}, {-1, 3}}));
        
        // Tuile 87:
        allTiles.push_back(Tile({{0, 0}, {-1, 0}, {-2, 0}, {-2, 1}, {-3, 1}, {-3, 2}}));
        
        // Tuile 88:
        allTiles.push_back(Tile({{0, 0}, {0, 1}, {-1, 0}, {-2, 0}, {-2, 1}, {-3, 1}, {-3, 2}}));
        
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

