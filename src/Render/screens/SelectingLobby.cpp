#include "Render/screens/SelectingLobby.hpp"
#include "Render/utils/Text.hpp"
#include "Render/utils/Selectable.hpp"
#include "Render/utils/Theme.hpp"
#include "Render/utils/Element.hpp"
#include <sstream>

static const int MAX_PLAYERS = 9; // nombre maximum de joueurs par lobby

void SelectingLobby::draw(sf::RenderWindow& window, GameState& gameState) {
    sf::Vector2u ws = window.getSize();
    
    // conteneur
    float cx = (ws.x - 800.0f) / 2.0f; // position x centrée: (largeur fenêtre - largeur conteneur) / 2
    float cy = (ws.y - 400.0f) / 2.0f; // position y centrée: (hauteur fenêtre - hauteur conteneur) / 2
    
    sf::RectangleShape container(sf::Vector2f(800.0f, 400.0f));
    container.setPosition(cx, cy);
    container.setFillColor(sf::Color(30, 30, 30));
    container.setOutlineColor(sf::Color::White);
    container.setOutlineThickness(2);
    window.draw(container);
    
    // texte "Select a Lobby:"
    sf::Text select = Text::createText("Select a Lobby:", 30);
    Element::centerH(select, static_cast<float>(ws.x), cy - select.getLocalBounds().height - 20.0f); // centré horizontalement, positionné au-dessus du conteneur avec espacement
    select.setFillColor(sf::Color::White);
    window.draw(select);
    
    static std::vector<std::unique_ptr<Selectable>> selectables;
    const auto& lobbies = gameState.getLobbies();
    float lx = cx + 20.0f;
    float ly = cy + 20.0f;
    
    if (selectables.size() != lobbies.size()) {
        selectables.clear();
        for (size_t i = 0; i < lobbies.size(); i++) {
            selectables.push_back(std::make_unique<Selectable>(lx, ly + i * 90.0f, 760.0f, 70.0f)); // position y = position de départ + index * espacement vertical
        }
    }
    
    // affichage des lobbies
    for (size_t i = 0; i < selectables.size() && i < lobbies.size(); i++) {
        float y = ly + i * 90.0f; // calcul de la position y de chaque lobby avec espacement
        selectables[i]->setSelected(gameState.getSelectedLobby() == lobbies[i].lobbyId);
        selectables[i]->draw(window);
        
        std::stringstream ss;
        ss << "Lobby #" << lobbies[i].lobbyId;
        if (lobbies[i].gameStarted) {
            ss << " (Started)";
        }
        sf::Text text = Text::createText(ss.str(), 22);
        text.setPosition(lx + 10, y + 20);
        text.setFillColor(sf::Color::White);
        window.draw(text);
        
        std::stringstream ps;
        ps << "Players " << lobbies[i].playerCount << " / " << MAX_PLAYERS;
        sf::Text players = Text::createText(ps.str(), 22);
        sf::FloatRect bounds = players.getLocalBounds();
        float px = cx + 800.0f - 20.0f - bounds.width - 10; // position x à droite: début conteneur + largeur - padding - largeur texte - marge
        players.setPosition(px, y + 20);
        players.setFillColor(sf::Color::White);
        window.draw(players);
    }
    
    // bouton Join
    float bx = (ws.x - 200.0f) / 2.0f;
    float by = cy + 400.0f + 30.0f;
    bool enabled = gameState.getSelectedLobby() != -1;
    
    sf::RectangleShape btn(sf::Vector2f(200.0f, 60.0f));
    btn.setPosition(bx, by);
    btn.setFillColor(enabled ? Theme::FOREST_GREEN : Theme::NEUTRAL_GRAY);
    btn.setOutlineColor(sf::Color::White);
    btn.setOutlineThickness(2);
    window.draw(btn);
    
    sf::Text btnText = Text::createText("Join", 28);
    Element::centerInContainer(btnText, bx, by, 200.0f, 60.0f); // centrage dans le bouton
    btnText.setFillColor(enabled ? sf::Color::White : sf::Color(128, 128, 128));
    window.draw(btnText);
}

bool SelectingLobby::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    if (event.type != sf::Event::MouseButtonPressed || event.mouseButton.button != sf::Mouse::Left) {
        return false;
    }
    
    static std::vector<std::unique_ptr<Selectable>> selectables;
    sf::Vector2f mouse(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
    sf::Vector2u ws = window.getSize();
    const auto& lobbies = gameState.getLobbies();
    
    float cx = (ws.x - 800.0f) / 2.0f;
    float cy = (ws.y - 400.0f) / 2.0f;
    float lx = cx + 20.0f;
    float ly = cy + 20.0f;
    
    if (selectables.size() != lobbies.size()) {
        selectables.clear();
        for (size_t i = 0; i < lobbies.size(); i++) {
            selectables.push_back(std::make_unique<Selectable>(lx, ly + i * 90.0f, 760.0f, 70.0f));
        }
    }
    
    for (size_t i = 0; i < selectables.size() && i < lobbies.size(); i++) {
        if (selectables[i]->contains(mouse.x, mouse.y)) {
            gameState.setSelectedLobby(lobbies[i].lobbyId);
            return false;
        }
    }
    
    float bx = (ws.x - 200.0f) / 2.0f;
    float by = cy + 400.0f + 30.0f;
    sf::FloatRect btnBounds(bx, by, 200.0f, 60.0f);
    
    if (btnBounds.contains(mouse.x, mouse.y) && gameState.getSelectedLobby() != -1) {
        gameState.setState(ClientState::ENTERING_USERNAME);
        return false;
    }
    
    return false;
}
