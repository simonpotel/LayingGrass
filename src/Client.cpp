#include "Client.hpp"

std::unique_ptr<sf::Window> createWindow() {
    return std::make_unique<sf::Window>(sf::VideoMode(800, 600), "LayingGrass Client");
}

void run(sf::Window& window) {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
    }
}

