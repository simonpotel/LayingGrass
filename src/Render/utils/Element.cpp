#include "Render/utils/Element.hpp"

Element::Element(float x, float y, float width, float height)
    : posX(x), posY(y), width(width), height(height) {
}

void Element::setPosition(float x, float y) {
    posX = x;
    posY = y;
    onPositionChanged();
}

void Element::setSize(float width, float height) {
    this->width = width;
    this->height = height;
    onSizeChanged();
}

bool Element::contains(float x, float y) const {
    return getBounds().contains(x, y);
}

