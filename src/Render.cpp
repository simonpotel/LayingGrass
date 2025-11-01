#include "Render.hpp"
#include "GameState.hpp"
#include <sstream>

std::unique_ptr<sf::RenderWindow> Render::createWindow() {
    return std::make_unique<sf::RenderWindow>(sf::VideoMode(800, 600), "LayingGrass Client");
}

void Render::render(sf::RenderWindow& window, GameState& gameState) {
    window.clear(sf::Color::Black);

    switch (gameState.getState()) {
        case ClientState::WAITING_FOR_LOBBIES:
            renderWaitingForLobbies(window);
            break;
        case ClientState::SELECTING_LOBBY:
            renderSelectingLobby(window, gameState);
            break;
        case ClientState::ENTERING_USERNAME:
            renderEnteringUsername(window, gameState);
            break;
        case ClientState::WAITING_FOR_RESPONSE:
            renderWaitingForResponse(window);
            break;
        case ClientState::IN_LOBBY:
            renderInLobby(window, gameState);
            break;
    }

    window.display();
}

bool Render::handleInput(sf::RenderWindow& window, GameState& gameState) {
    static std::vector<sf::FloatRect> lobbyRects;
    static sf::FloatRect joinButtonRect;

    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            return true;
        }

        // sélection de lobby
        if (gameState.getState() == ClientState::SELECTING_LOBBY &&
            event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {

            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            const auto& lobbies = gameState.getLobbies();

            // clic sur un lobby
            for (size_t i = 0; i < lobbyRects.size() && i < lobbies.size(); ++i) {
                if (lobbyRects[i].contains(mousePos)) {
                    gameState.setSelectedLobby(lobbies[i].lobbyId);
                    break;
                }
            }

            // clic sur le bouton join
            if (gameState.getSelectedLobby() != -1 && joinButtonRect.contains(mousePos)) {
                gameState.setState(ClientState::ENTERING_USERNAME);
            }
        }

        // saisie du nom
        if (gameState.getState() == ClientState::ENTERING_USERNAME) {
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
                if (!gameState.getUsername().empty()) {
                    gameState.setState(ClientState::WAITING_FOR_RESPONSE);
                }
            } else if (event.type == sf::Event::TextEntered) {
                std::string current = gameState.getUsername();
                if (event.text.unicode == 8 && !current.empty()) {
                    current.pop_back();
                    gameState.setUsername(current);
                } else if (event.text.unicode >= 32 && event.text.unicode < 128 && current.size() < 20) {
                    current += static_cast<char>(event.text.unicode);
                    gameState.setUsername(current);
                }
            }
        }
    }

    // mise à jour des zones cliquables
    lobbyRects.clear();
    float y = 100;
    for (size_t i = 0; i < gameState.getLobbies().size() && i < 9; ++i) {
        lobbyRects.push_back(sf::FloatRect(50, y, 700, 35));
        y += 40;
    }
    joinButtonRect = sf::FloatRect(300, 520, 200, 50);

    return false;
}

void Render::renderWaitingForLobbies(sf::RenderWindow& window) {
    drawText(window, "Waiting for lobby list...", 50, 250);
}

void Render::renderSelectingLobby(sf::RenderWindow& window, GameState& gameState) {
    drawText(window, "Select a lobby:", 50, 50);

    const auto& lobbies = gameState.getLobbies();
    int selectedLobby = gameState.getSelectedLobby();
    float y = 100;

    // affichage des lobbies
    for (size_t i = 0; i < lobbies.size() && i < 9; ++i) {
        sf::RectangleShape box(sf::Vector2f(700, 35));
        box.setPosition(50, y);
        box.setFillColor(lobbies[i].lobbyId == selectedLobby ?
            sf::Color(70, 130, 180) : sf::Color(60, 60, 60));
        box.setOutlineColor(sf::Color(100, 100, 100));
        box.setOutlineThickness(2);
        window.draw(box);

        std::ostringstream oss;
        oss << "Lobby " << lobbies[i].lobbyId << " - "
            << lobbies[i].playerCount << "/2 players";
        if (lobbies[i].gameStarted) oss << " (In game)";

        drawText(window, oss.str(), 60, y + 5, 20);
        y += 40;
    }

    // bouton join
    sf::RectangleShape joinButton(sf::Vector2f(200, 50));
    joinButton.setPosition(300, 520);
    joinButton.setFillColor(selectedLobby != -1 ?
        sf::Color(34, 139, 34) : sf::Color(80, 80, 80));
    joinButton.setOutlineColor(sf::Color(150, 150, 150));
    joinButton.setOutlineThickness(2);
    window.draw(joinButton);

    drawText(window, "Join", 365, 535, 24);
    drawText(window, "Click on a lobby to select it", 250, 480, 16);
}

void Render::renderEnteringUsername(sf::RenderWindow& window, GameState& gameState) {
    drawText(window, "Enter your username:", 50, 200);
    drawText(window, "> " + gameState.getUsername() + "_", 80, 250, 20);
    drawText(window, "Press Enter to confirm", 50, 350, 16);
}

void Render::renderWaitingForResponse(sf::RenderWindow& window) {
    drawText(window, "Connecting to lobby...", 50, 250);
}

void Render::renderInLobby(sf::RenderWindow& window, GameState& gameState) {
    std::ostringstream oss;
    oss << "You are in lobby " << gameState.getCurrentLobby();
    drawText(window, oss.str(), 50, 200);
    drawText(window, "Waiting for other players...", 50, 250);
}

void Render::drawText(sf::RenderWindow& window, const std::string& text, float x, float y, unsigned int size) {
    static sf::Font font;
    static bool loaded = false;

    // charge la police une seule fois
    if (!loaded && font.loadFromFile("../resources/BoldPixels.ttf")) {
        loaded = true;
    }

    // affiche le texte ou un carré rouge si la police n'est pas chargée
    if (!loaded) {
        sf::RectangleShape error(sf::Vector2f(text.length() * size * 0.6f, size));
        error.setPosition(x, y);
        error.setFillColor(sf::Color::Red);
        window.draw(error);
        return;
    }

    sf::Text sfText;
    sfText.setFont(font);
    sfText.setString(text);
    sfText.setCharacterSize(size);
    sfText.setFillColor(sf::Color::White);
    sfText.setPosition(x, y);
    window.draw(sfText);
}
