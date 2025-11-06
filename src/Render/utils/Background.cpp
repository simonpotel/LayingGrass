#include "Render/utils/Background.hpp"
#include <iostream>
#include <algorithm>

std::unique_ptr<sf::Texture> Background::load(const std::string& filename) {
    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(filename)) {
        std::cerr << "Error: Cannot load background " << filename << std::endl;
        return nullptr;
    }
    return texture;
}

void Background::draw(sf::RenderWindow& window, sf::Texture& texture) {
    sf::Sprite sprite(texture); // crée un sprite à partir de la texture 
    window.draw(sprite); // affiche le sprite
}

void Background::drawFullscreen(sf::RenderWindow& window, sf::Texture& texture) {
    sf::Sprite sprite(texture); // crée un sprite à partir de la texture
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = texture.getSize(); // récupère la taille de la texture
    
    if (textureSize.x > 0 && textureSize.y > 0) {
        float scaleX = static_cast<float>(windowSize.x) / static_cast<float>(textureSize.x); // calcule la taille de l'échelle en x
        float scaleY = static_cast<float>(windowSize.y) / static_cast<float>(textureSize.y); // calcule la taille de l'échelle en y
        float scale = std::max(scaleX, scaleY); // calcule la taille de l'échelle
        
        sprite.setScale(scale, scale); // applique la taille de l'échelle au sprite
        
        float scaledWidth = textureSize.x * scale; // calcule la largeur de l'image échelonnée
        float scaledHeight = textureSize.y * scale; // calcule la hauteur de l'image échelonnée
        sprite.setPosition((windowSize.x - scaledWidth) / 2.0f, (windowSize.y - scaledHeight) / 2.0f); // applique la position au sprite
    }
    
    window.draw(sprite); // affiche le sprite
}

void Background::drawScaled(sf::RenderWindow& window, sf::Texture& texture, float scaleX, float scaleY) {
    sf::Sprite sprite(texture); // crée un sprite à partir de la texture
    sprite.setScale(scaleX, scaleY);
    window.draw(sprite); // affiche le sprite
}

