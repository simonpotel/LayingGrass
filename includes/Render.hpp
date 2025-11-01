#ifndef RENDER_HPP
#define RENDER_HPP

#include <SFML/Window.hpp>
#include <memory>

// classe pour la gestion de la fenêtre de rendu avec SFML pour le client 
class Render {
public:
    static std::unique_ptr<sf::Window> createWindow(); // crée une fenêtre de rendu avec SFML
    static void run(sf::Window& window); // exécute la boucle de rendu avec SFML
};

#endif

