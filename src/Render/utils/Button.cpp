#include "Render/utils/Button.hpp"
#include "Render/utils/Text.hpp"
#include <iostream>
#include <algorithm>

static const float BUTTON_SCALE_MULTIPLIER = 1.2f;

Button::Button(float x, float y, float width, float height, const std::string& text, unsigned int textSize)
    : Element(x, y, width, height), enabled(true) {
    
    this->text = Text::createText(text, textSize); // crée un texte à partir du texte et de la taille de texte
    
    if (!texture.loadFromFile("button.png") && !texture.loadFromFile("../resources/button.png")) {
        std::cerr << "Error: Cannot load button.png" << std::endl; // affiche un message d'erreur si la texture n'est pas chargée   
    }
     
    sprite.setTexture(texture); // applique la texture au sprite
    updateSpriteSize();
    updateTextPosition();
}

void Button::setText(const std::string& text) {
    unsigned int size = this->text.getCharacterSize();
    this->text = Text::createText(text, size);
    updateTextPosition();
}

void Button::onPositionChanged() {
    updateSpriteSize();
    updateTextPosition();
}

void Button::onSizeChanged() {
    updateSpriteSize();
    updateTextPosition();
}

void Button::setEnabled(bool enabled) {
    this->enabled = enabled;
    if (enabled) {
        text.setFillColor(sf::Color::White);
        sprite.setColor(sf::Color::White);
    } else {
        text.setFillColor(sf::Color(128, 128, 128));
        sprite.setColor(sf::Color(128, 128, 128));
    }
}

sf::FloatRect Button::getBounds() const {
    return sprite.getGlobalBounds();
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(sprite);
    window.draw(text);
}

void Button::updateSpriteSize() {
    if (texture.getSize().x == 0 || texture.getSize().y == 0) {
        sprite.setPosition(posX, posY);
        return;
    }
    
    float originalWidth = static_cast<float>(texture.getSize().x);
    float originalHeight = static_cast<float>(texture.getSize().y);
    
    float scaleX = width / originalWidth;
    float scaleY = height / originalHeight;
    float scale = std::min(scaleX, scaleY) * BUTTON_SCALE_MULTIPLIER;
    
    sprite.setScale(scale, scale);
    
    float scaledWidth = originalWidth * scale;
    float scaledHeight = originalHeight * scale;
    float offsetX = (width - scaledWidth) / 2.0f;
    float offsetY = (height - scaledHeight) / 2.0f;
    
    sprite.setPosition(posX + offsetX, posY + offsetY);
}

void Button::updateTextPosition() {
    sf::FloatRect textBounds = text.getLocalBounds();
    
    float textX = posX + (width - textBounds.width) / 2.0f - textBounds.left;
    float textY = posY + (height - textBounds.height) / 2.0f - textBounds.top;
    
    text.setPosition(textX, textY);
}

