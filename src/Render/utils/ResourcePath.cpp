#include "Render/utils/ResourcePath.hpp"
#include <fstream>
#include <vector>

namespace ResourcePath {
    std::string find(const std::string& filename) {
        // Liste des chemins à essayer dans l'ordre de priorité
        std::vector<std::string> paths = {
            filename,                              // Répertoire courant
            "./" + filename,                       // Répertoire courant explicite
            "resources/" + filename,               // Sous-dossier resources
            "./resources/" + filename,             // Sous-dossier resources explicite
            "../resources/" + filename,            // Resources un niveau au-dessus
            "../../resources/" + filename,         // Resources deux niveaux au-dessus
            "../../../resources/" + filename       // Resources trois niveaux au-dessus
        };
        
        for (const auto& path : paths) {
            std::ifstream file(path);
            if (file.good()) {
                file.close();
                return path;
            }
        }
        
        // Si aucun chemin ne fonctionne, retourner le nom du fichier tel quel
        // (SFML essaiera quand même de le charger)
        return filename;
    }
}

