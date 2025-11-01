#include "Render/screens/SelectingLobby.hpp"
#include "Render/utils/Text.hpp"
#include <sstream>

void SelectingLobby::draw(sf::RenderWindow& window, GameState& gameState) {
    static std::vector<sf::FloatRect> lobbyBoxes;
    lobbyBoxes.clear();

    Text::draw(window, "Select a Lobby:", 50, 30, 30);

    const auto& lobbies = gameState.getLobbies();
    float y = 100;

    for (size_t i = 0; i < lobbies.size(); i++) {
        const auto& lobby = lobbies[i];

        // couleur de la box (bleu si sélectionné, gris sinon)
        sf::Color boxColor = (gameState.getSelectedLobby() == lobby.lobbyId)
            ? sf::Color(50, 100, 200)
            : sf::Color(60, 60, 60);

        Text::drawBox(window, 50, y, 700, 80, boxColor, sf::Color::White);

        // sauvegarde la zone cliquable
        lobbyBoxes.push_back(sf::FloatRect(50, y, 700, 80));

        // affiche les infos du lobby
        std::stringstream ss;
        ss << "Lobby #" << lobby.lobbyId;
        Text::draw(window, ss.str(), 70, y + 10, 24);

        ss.str("");
        ss << "Players: " << lobby.playerCount;
        if (lobby.gameStarted) {
            ss << " (Game started)";
        }
        Text::draw(window, ss.str(), 70, y + 45, 18);

        y += 100;
    }

    // bouton join (vert si lobby sélectionné, gris sinon)
    sf::Color joinColor = (gameState.getSelectedLobby() != -1)
        ? sf::Color(50, 150, 50)
        : sf::Color(80, 80, 80);

    Text::drawBox(window, 300, 500, 200, 60, joinColor, sf::Color::White);
    Text::draw(window, "Join", 370, 515, 28);
}

bool SelectingLobby::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    static std::vector<sf::FloatRect> lobbyBoxes;

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

        // vérifie si on a cliqué sur un lobby
        const auto& lobbies = gameState.getLobbies();
        float y = 100;

        for (size_t i = 0; i < lobbies.size(); i++) {
            sf::FloatRect box(50, y, 700, 80);
            if (box.contains(mousePos)) {
                gameState.setSelectedLobby(lobbies[i].lobbyId);
                return false;
            }
            y += 100;
        }

        // vérifie si on a cliqué sur le bouton join
        sf::FloatRect joinButton(300, 500, 200, 60);
        if (joinButton.contains(mousePos) && gameState.getSelectedLobby() != -1) {
            gameState.setState(ClientState::ENTERING_USERNAME);
            return false;
        }
    }

    return false;
}
