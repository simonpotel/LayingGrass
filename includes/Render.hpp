#ifndef RENDER_HPP
#define RENDER_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

class GameState;

// classe pour la gestion de la fenêtre de rendu avec SFML pour le client
class Render {
public:
    static std::unique_ptr<sf::RenderWindow> createWindow(); // crée une fenêtre de rendu avec SFML
    static void render(sf::RenderWindow& window, GameState& gameState); // effectue le rendu en fonction de l'état du jeu
    static bool handleInput(sf::RenderWindow& window, GameState& gameState); // gère les inputs utilisateur (retourne true si doit quitter)

private:
    // fonctions de rendu pour chaque état
    static void renderWaitingForLobbies(sf::RenderWindow& window);
    static void renderSelectingLobby(sf::RenderWindow& window, GameState& gameState);
    static void renderEnteringUsername(sf::RenderWindow& window, GameState& gameState);
    static void renderWaitingForResponse(sf::RenderWindow& window);
    static void renderInLobby(sf::RenderWindow& window, GameState& gameState);

    // fonction utilitaire pour afficher du texte
    static void drawText(sf::RenderWindow& window, const std::string& text, float x, float y, unsigned int size = 24);
};

#endif

