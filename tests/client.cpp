#include <gtest/gtest.h>
#include "Client.hpp"

TEST(Client, WindowCreation) {
    auto window = createWindow();
    
    ASSERT_NE(window, nullptr);
    EXPECT_TRUE(window->isOpen());
    
    sf::Vector2u size = window->getSize();
    EXPECT_EQ(size.x, 800u);
    EXPECT_EQ(size.y, 600u);
    
    window->close();
    EXPECT_FALSE(window->isOpen());
}
