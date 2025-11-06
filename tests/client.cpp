#include <gtest/gtest.h>
#include "Render.hpp"
#include "Client.hpp"

Client* g_client = nullptr;

// test de la création de la fenêtre de rendu avec SFML

TEST(Render, WindowCreation) {
    auto window = Render::createWindow(); // crée une fenêtre de rendu avec SFML
    
    ASSERT_NE(window, nullptr); // vérifie que la fenêtre n'est pas nulle
    EXPECT_TRUE(window->isOpen()); // vérifie que la fenêtre est ouverte
    
    //sf::Vector2u size = window->getSize(); // récupère la taille de la fenêtre
    //EXPECT_EQ(size.x, 800u); // vérifie que la largeur de la fenêtre est de 800 pixels
    //EXPECT_EQ(size.y, 600u); // vérifie que la hauteur de la fenêtre est de 600 pixels
    
    window->close(); // ferme la fenêtre
    EXPECT_FALSE(window->isOpen()); // vérifie que la fenêtre est fermée
}
