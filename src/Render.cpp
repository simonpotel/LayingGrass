#include "Render.hpp"

std::unique_ptr<sf::Window> Render::createWindow() {
    // crée une fenêtre de rendu avec SFML
    return std::make_unique<sf::Window>(sf::VideoMode(800, 600), "LayingGrass Client");
}

void Render::run(sf::Window& window) {
    // exécute la boucle de rendu avec SFML
    while (window.isOpen()) {
        sf::Event event; // événement de la fenêtre
        while (window.pollEvent(event)) { // 
            if (event.type == sf::Event::Closed) { // event fermeture de la fenêtre
                window.close(); // ferme la fenêtre
            }
        }
    }
}

