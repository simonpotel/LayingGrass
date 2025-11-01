#include "Render/utils/Selectable.hpp"

Selectable::Selectable(float x, float y, float width, float height)
    : Element(x, y, width, height), selected(false) {
    
    rectangle.setPosition(posX, posY);
    rectangle.setSize(sf::Vector2f(this->width, this->height));
    rectangle.setFillColor(sf::Color(60, 60, 60)); // définit la couleur de remplissage du rectangle
    rectangle.setOutlineColor(sf::Color::White); // définit la couleur de l'outline du rectangle
    rectangle.setOutlineThickness(2); // définit l'épaisseur de l'outline du rectangle
}

void Selectable::setSelected(bool selected) {
    this->selected = selected;
    if (selected) {
        rectangle.setFillColor(sf::Color(50, 100, 200));
    } else {
        rectangle.setFillColor(sf::Color(60, 60, 60));
    }
}

void Selectable::onPositionChanged() {
    updateRectangle();
}

void Selectable::onSizeChanged() {
    updateRectangle();
}

void Selectable::updateRectangle() {
    rectangle.setPosition(posX, posY);
    rectangle.setSize(sf::Vector2f(this->width, this->height));
}

sf::FloatRect Selectable::getBounds() const {
    return rectangle.getGlobalBounds();
}

void Selectable::draw(sf::RenderWindow& window) {
    window.draw(rectangle);
}
