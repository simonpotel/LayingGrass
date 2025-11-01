#ifndef RENDER_HPP
#define RENDER_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include "GameState.hpp"

class Render {
public:
    virtual ~Render() = default;
    
    virtual void draw(sf::RenderWindow& window, GameState& gameState) = 0;
    virtual bool handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) = 0;
    
    static std::unique_ptr<sf::RenderWindow> createWindow();
    static std::unique_ptr<Render> createScreen(ClientState state);
    static void render(sf::RenderWindow& window, GameState& gameState);
    static bool handleInput(sf::RenderWindow& window, GameState& gameState);
};

#endif
