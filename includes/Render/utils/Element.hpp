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
    
    // méthodes utilitaires statiques pour centrer des éléments SFML
    template<typename T>
    static void centerH(T& drawable, float containerWidth, float y) {
        sf::FloatRect bounds = drawable.getLocalBounds();
        drawable.setPosition((containerWidth - bounds.width) / 2.0f - bounds.left, y);
    }
    
    template<typename T>
    static void centerV(T& drawable, float x, float containerHeight) {
        sf::FloatRect bounds = drawable.getLocalBounds();
        drawable.setPosition(x, (containerHeight - bounds.height) / 2.0f - bounds.top);
    }
    
    template<typename T>
    static void center(T& drawable, float containerWidth, float containerHeight) {
        sf::FloatRect bounds = drawable.getLocalBounds();
        drawable.setPosition((containerWidth - bounds.width) / 2.0f - bounds.left,
                           (containerHeight - bounds.height) / 2.0f - bounds.top);
    }
    
    // centre dans un conteneur positionné (position + taille)
    template<typename T>
    static void centerInContainer(T& drawable, float containerX, float containerY, float containerWidth, float containerHeight) {
        sf::FloatRect bounds = drawable.getLocalBounds();
        drawable.setPosition(containerX + (containerWidth - bounds.width) / 2.0f - bounds.left,
                           containerY + (containerHeight - bounds.height) / 2.0f - bounds.top);
    }

protected:
    virtual void onPositionChanged() {}
    virtual void onSizeChanged() {}
    
    float posX, posY;
    float width, height;
};

