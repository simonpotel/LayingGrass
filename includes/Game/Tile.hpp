#pragma once
#include <vector>
#include <utility>

// Représente une tuile (polymino) avec ses blocs
class Tile {
public:
    // Coordonnées relatives des blocs qui composent la tuile
    // Format: (row, col) où (0,0) est le point de référence
    std::vector<std::pair<int, int>> blocks;
    
    Tile() = default;
    Tile(const std::vector<std::pair<int, int>>& coords) : blocks(coords) {}
    
    // Rotation de 90° dans le sens horaire
    Tile rotate90() const;
    
    // Retournement horizontal (miroir)
    Tile flipHorizontal() const;
    
    // Retournement vertical (miroir)
    Tile flipVertical() const;
    
    // Dimensions de la tuile (hauteur et largeur)
    int getWidth() const;
    int getHeight() const;
    
    // Normalise la tuile (déplace pour que les coordonnées commencent à (0,0))
    Tile normalized() const;
    
    // Vérifie si la tuile est valide (au moins un bloc)
    bool isValid() const { return !blocks.empty(); }
    
    // Nombre de blocs dans la tuile
    size_t getBlockCount() const { return blocks.size(); }
};

