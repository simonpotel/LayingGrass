#include "Render.hpp"
#include "GameState.hpp"
#include "Render/screens/WaitingForLobbies.hpp"
#include "Render/screens/SelectingLobby.hpp"
#include "Render/screens/EnteringUsername.hpp"
#include "Render/screens/WaitingForResponse.hpp"
#include "Render/screens/InLobby.hpp"
#include "Render/screens/InGame.hpp"
#include "Render/screens/GameEnd.hpp"
#include "Render/screens/TileViewer.hpp"
#include "Render/utils/Theme.hpp"
#include "Render/utils/Text.hpp"
#include "Render/utils/Element.hpp"

std::unique_ptr<sf::RenderWindow> Render::createWindow() {
    return std::make_unique<sf::RenderWindow>(sf::VideoMode(1280, 720), "LayingGrass Client", sf::Style::Titlebar | sf::Style::Close);
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
        case ClientState::IN_GAME:
            return std::make_unique<InGame>();
        case ClientState::GAME_END:
            return std::make_unique<GameEnd>();
        case ClientState::VIEWING_TILES:
            return std::make_unique<TileViewer>();
        default:
            return std::make_unique<WaitingForLobbies>();
    }
}

void Render::render(sf::RenderWindow& window, GameState& gameState) {
    window.clear(sf::Color::Black);
    
    // titre (affiché sur tous les écrans)
    sf::Vector2u ws = window.getSize();
    sf::Text title = Text::createText("LayingGrass", 60.0f);
    Element::centerH(title, static_cast<float>(ws.x), 40.0f); // centrage horizontal dans la fenêtre
    title.setFillColor(sf::Color::White);
    window.draw(title);
    
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
