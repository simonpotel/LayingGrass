#pragma once
#include <SFML/Graphics.hpp>
#include "Render/utils/Element.hpp"
#include <string>

class Button : public Element {
public:
    Button(float x, float y, float width, float height, const std::string& text, unsigned int textSize = 24);
    
    void setText(const std::string& text); // définit le texte du bouton
    void setEnabled(bool enabled); // active ou désactive le bouton
    
    sf::FloatRect getBounds() const override; // retourne la zone cliquable du bouton
    void draw(sf::RenderWindow& window) override; // affiche le bouton

protected:
    void onPositionChanged() override;
    void onSizeChanged() override;

private:
    void updateTextPosition(); // met à jour la position du texte du bouton
    void updateSpriteSize(); // met à jour la taille du sprite du bouton
    
    sf::Texture texture; // texture du bouton
    sf::Sprite sprite; // sprite du bouton
    sf::Text text; // texte du bouton
    bool enabled; // true si le bouton est actif, false sinon
};

