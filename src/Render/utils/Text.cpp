#include "Render/utils/Text.hpp"

namespace Text {

void draw(sf::RenderWindow& window, const std::string& text, float x, float y, unsigned int size) {
    static sf::Font font;
    static bool loaded = false;

    if (!loaded) {
        loaded = font.loadFromFile("BoldPixels.ttf");
    }

    // affiche le texte ou un carré rouge si la police n'est pas chargée
    if (!loaded) {
        sf::RectangleShape error(sf::Vector2f(text.length() * size * 0.6f, size));
        error.setPosition(x, y);
        error.setFillColor(sf::Color::Red);
        window.draw(error);
        return;
    }

    sf::Text sfText;
    sfText.setFont(font);
    sfText.setString(text);
    sfText.setCharacterSize(size);
    sfText.setFillColor(sf::Color::White);
    sfText.setPosition(x, y);
    window.draw(sfText);
}

void drawBox(sf::RenderWindow& window, float x, float y, float width, float height, sf::Color fillColor, sf::Color outlineColor) {
    sf::RectangleShape box(sf::Vector2f(width, height));
    box.setPosition(x, y);
    box.setFillColor(fillColor);
    box.setOutlineColor(outlineColor);
    box.setOutlineThickness(2);
    window.draw(box);
}

sf::Text createText(const std::string& text, unsigned int size) {
    static sf::Font font;
    static bool loaded = false;
    
    if (!loaded) {
        loaded = font.loadFromFile("BoldPixels.ttf");
    }
    
    sf::Text sfText;
    if (loaded) {
        sfText.setFont(font);
    }
    sfText.setString(text);
    sfText.setCharacterSize(size);
    sfText.setFillColor(sf::Color::White);
    
    return sfText;
}

}
