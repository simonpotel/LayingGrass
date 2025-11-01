#include <SFML/Window.hpp>
#include <memory>

std::unique_ptr<sf::Window> createWindow();
void run(sf::Window& window);

