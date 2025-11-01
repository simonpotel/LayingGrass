#pragma once
#include <SFML/Graphics.hpp>

class Element {
public:
    Element(float x, float y, float width, float height); // constructeur de l'élément
    virtual ~Element() = default;
    
    void setPosition(float x, float y);
    void setSize(float width, float height);
    
    float getX() const { return posX; }
    float getY() const { return posY; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    
    bool contains(float x, float y) const; // vérifie si un point est dans l'élément
    virtual sf::FloatRect getBounds() const = 0; // retourne la zone de l'élément
    virtual void draw(sf::RenderWindow& window) = 0; // affiche l'élément

protected:
    virtual void onPositionChanged() {}
    virtual void onSizeChanged() {}
    
    float posX, posY;
    float width, height;
};

