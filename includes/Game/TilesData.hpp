#pragma once
#include "Game/Tile.hpp"
#include <vector>

// Contient les 96 tuiles prédéfinies du jeu
namespace TilesData {
    // Initialise toutes les tuiles (à appeler une fois au démarrage)
    void initialize();
    
    // Retourne toutes les 96 tuiles
    const std::vector<Tile>& getAllTiles();
    
    // Retourne une tuile par son index (0-95)
    const Tile& getTile(int index);
    
    // Nombre total de tuiles
    constexpr int TOTAL_TILES = 96;
}

