#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

namespace Background {
    std::unique_ptr<sf::Texture> load(const std::string& filename); // charge le fond d'écran depuis le fichier
    void draw(sf::RenderWindow& window, sf::Texture& texture); // affiche le fond d'écran
    void drawFullscreen(sf::RenderWindow& window, sf::Texture& texture); // affiche le fond d'écran en entier
    void drawScaled(sf::RenderWindow& window, sf::Texture& texture, float scaleX, float scaleY); // affiche le fond d'écran à une taille spécifique
}

