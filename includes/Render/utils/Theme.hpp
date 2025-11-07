#pragma once

#include <SFML/Graphics.hpp>

namespace Theme {
    // Primary Colors
    inline const sf::Color FOREST_GREEN  = sf::Color(34, 139, 34);   // #228B22
    inline const sf::Color DEEP_FOREST   = sf::Color(21, 71, 21);    // #154715
    inline const sf::Color EARTH_BROWN   = sf::Color(101, 67, 33);   // #654321
    inline const sf::Color WARM_BEIGE    = sf::Color(245, 245, 220); // #F5F5DC
    inline const sf::Color CREAM         = sf::Color(255, 253, 208); // #FFFDD0

    // Semantic Colors
    inline const sf::Color VALID_PLACEMENT   = sf::Color(144, 238, 144); // #90EE90
    inline const sf::Color INVALID_PLACEMENT = sf::Color(220, 20, 60);   // #DC143C
    inline const sf::Color CURRENT_PLAYER    = sf::Color(255, 215, 0);   // #FFD700
    inline const sf::Color NEUTRAL_GRAY      = sf::Color(169, 169, 169); // #A9A9A9

    // Bonus Colors
    inline const sf::Color BONUS_EXCHANGE = sf::Color(100, 149, 237); // #6495ED
    inline const sf::Color BONUS_STONE    = sf::Color(105, 105, 105); // #696969
    inline const sf::Color BONUS_ROBBERY  = sf::Color(178, 34, 34);   // #B22222

    // Utility helpers
    inline sf::Color withAlpha(sf::Color color, sf::Uint8 alpha) {
        color.a = alpha;
        return color;
    }
}


