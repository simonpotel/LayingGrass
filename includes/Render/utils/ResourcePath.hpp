#pragma once
#include <string>

namespace ResourcePath {
    // Trouve le chemin correct pour une ressource en essayant plusieurs emplacements
    std::string find(const std::string& filename);
}

