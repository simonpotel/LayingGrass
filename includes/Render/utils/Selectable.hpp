#pragma once
#include <SFML/Graphics.hpp>
#include "Render/utils/Element.hpp"

class Selectable : public Element {
public:
    Selectable(float x, float y, float width, float height); // constructeur de l'élément sélectionnable
    
    void setSelected(bool selected); // définit si l'élément est sélectionné
    
    sf::FloatRect getBounds() const override; // retourne la zone de l'élément sélectionnable
    void draw(sf::RenderWindow& window) override; // affiche l'élément sélectionnable

protected:
    void onPositionChanged() override;
    void onSizeChanged() override;

private:
    void updateRectangle();
    
    sf::RectangleShape rectangle;
    bool selected;
};

