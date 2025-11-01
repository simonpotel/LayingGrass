#include "Render/utils/Selectable.hpp"

Selectable::Selectable(float x, float y, float width, float height)
    : Element(x, y, width, height), selected(false) {
    
    rectangle.setPosition(posX, posY);
    rectangle.setSize(sf::Vector2f(this->width, this->height));
    rectangle.setFillColor(sf::Color(60, 60, 60, 80));
    rectangle.setOutlineColor(sf::Color(255, 255, 255, 120));
    rectangle.setOutlineThickness(2);
}

void Selectable::setSelected(bool selected) {
    this->selected = selected;
    if (selected) {
        rectangle.setFillColor(sf::Color(50, 100, 200, 100));
    } else {
        rectangle.setFillColor(sf::Color(60, 60, 60, 80));
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
