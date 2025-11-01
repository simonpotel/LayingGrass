#include "Render.hpp"
#include "GameState.hpp"
#include "Render/screens/WaitingForLobbies.hpp"
#include "Render/screens/SelectingLobby.hpp"
#include "Render/screens/EnteringUsername.hpp"
#include "Render/screens/WaitingForResponse.hpp"
#include "Render/screens/InLobby.hpp"

std::unique_ptr<sf::RenderWindow> Render::createWindow() {
    return std::make_unique<sf::RenderWindow>(sf::VideoMode(800, 600), "LayingGrass Client");
}

std::unique_ptr<Render> Render::createScreen(ClientState state) {
    switch (state) {
        case ClientState::WAITING_FOR_LOBBIES:
            return std::make_unique<WaitingForLobbies>();
        case ClientState::SELECTING_LOBBY:
            return std::make_unique<SelectingLobby>();
        case ClientState::ENTERING_USERNAME:
            return std::make_unique<EnteringUsername>();
        case ClientState::WAITING_FOR_RESPONSE:
            return std::make_unique<WaitingForResponse>();
        case ClientState::IN_LOBBY:
            return std::make_unique<InLobby>();
        default:
            return std::make_unique<WaitingForLobbies>();
    }
}

void Render::render(sf::RenderWindow& window, GameState& gameState) {
    window.clear(sf::Color::Black);
    
    auto screen = createScreen(gameState.getState());
    screen->draw(window, gameState);
    
    window.display();
}

bool Render::handleInput(sf::RenderWindow& window, GameState& gameState) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            return true;
        }
        
        auto screen = createScreen(gameState.getState());
        bool shouldQuit = screen->handleInput(window, gameState, event);
        
        if (shouldQuit) return true;
    }
    
    return false;
}
