#include "Game/Tile.hpp"
#include <algorithm>

Tile Tile::rotate90() const {
    std::vector<std::pair<int, int>> rotated;
    for (const auto& [row, col] : blocks) {
        // Rotation 90° horaire: (row, col) -> (col, -row)
        rotated.push_back({col, -row});
    }
    
    // Normaliser pour avoir des coordonnées positives
    Tile result(rotated);
    return result.normalized();
}

Tile Tile::flipHorizontal() const {
    if (blocks.empty()) return *this;
    
    int maxCol = blocks[0].second;
    for (const auto& [r, c] : blocks) {
        maxCol = std::max(maxCol, c);
    }
    
    std::vector<std::pair<int, int>> flipped;
    for (const auto& [row, col] : blocks) {
        flipped.push_back({row, maxCol - col});
    }
    return Tile(flipped).normalized();
}

Tile Tile::flipVertical() const {
    if (blocks.empty()) return *this;
    
    int maxRow = blocks[0].first;
    for (const auto& [r, c] : blocks) {
        maxRow = std::max(maxRow, r);
    }
    
    std::vector<std::pair<int, int>> flipped;
    for (const auto& [row, col] : blocks) {
        flipped.push_back({maxRow - row, col});
    }
    return Tile(flipped).normalized();
}

int Tile::getWidth() const {
    if (blocks.empty()) return 0;
    
    int minCol = blocks[0].second;
    int maxCol = blocks[0].second;
    for (const auto& [r, c] : blocks) {
        minCol = std::min(minCol, c);
        maxCol = std::max(maxCol, c);
    }
    return maxCol - minCol + 1;
}

int Tile::getHeight() const {
    if (blocks.empty()) return 0;
    
    int minRow = blocks[0].first;
    int maxRow = blocks[0].first;
    for (const auto& [r, c] : blocks) {
        minRow = std::min(minRow, r);
        maxRow = std::max(maxRow, r);
    }
    return maxRow - minRow + 1;
}

Tile Tile::normalized() const {
    if (blocks.empty()) return *this;
    
    int minRow = blocks[0].first;
    int minCol = blocks[0].second;
    
    for (const auto& [r, c] : blocks) {
        minRow = std::min(minRow, r);
        minCol = std::min(minCol, c);
    }
    
    std::vector<std::pair<int, int>> normalized;
    for (const auto& [r, c] : blocks) {
        normalized.push_back({r - minRow, c - minCol});
    }
    
    return Tile(normalized);
}

