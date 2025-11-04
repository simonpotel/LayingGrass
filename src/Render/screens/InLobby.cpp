#include "Render/screens/InLobby.hpp"
#include "Render/utils/Text.hpp"
#include "GameState.hpp"
#include <SFML/Graphics.hpp>
#include <sstream>

void InLobby::draw(sf::RenderWindow& window, GameState& gameState) {
    int currentLobbyId = gameState.getCurrentLobby();

    std::stringstream ss;
    ss << "Connected to Lobby #" << currentLobbyId;
    Text::draw(window, ss.str(), 250, 100, 32);

    Text::draw(window, "Players:", 250, 180, 28);

    const auto& lobbies = gameState.getLobbies(); // récupère la liste des lobbies
    for (const auto& lobby : lobbies) {
        if (lobby.lobbyId == currentLobbyId) {
            float y = 220.0f;
            for (int i = 0; i < lobby.playerCount; i++) {
                if (lobby.players[i].colorId >= 0 && lobby.players[i].colorId < 9) { //vérifie si la couleur est valide
                    sf::Color playerColor = GameState::PLAYERS_COLORS[lobby.players[i].colorId]; //récupère la couleur du joueur
                    Text::drawBox(window, 250, y, 35, 35, playerColor, sf::Color::White);
                    Text::draw(window, lobby.players[i].playerName, 295, y + 5, 22); //affiche le nom du joueur
                }
                y += 50.0f; //augmente la position y pour le prochain joueur
            }
            break;
        }
    }

    Text::draw(window, "Waiting for game to start...", 250, 450, 24);
}

bool InLobby::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    return false;
}
