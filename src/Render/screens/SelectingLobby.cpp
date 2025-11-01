#include "Render/screens/SelectingLobby.hpp"
#include "Render/utils/Text.hpp"
#include "Render/utils/Button.hpp"
#include "Render/utils/Selectable.hpp"
#include "Render/utils/Background.hpp"
#include <sstream>

static const float LOBBY_X = 50.0f;
static const float LOBBY_Y_START = 120.0f;
static const float LOBBY_WIDTH = 700.0f;
static const float LOBBY_HEIGHT = 70.0f;
static const float LOBBY_SPACING = 90.0f;
static const float BUTTON_X = 300.0f;
static const float BUTTON_Y = 520.0f;
static const float BUTTON_WIDTH = 200.0f;
static const float BUTTON_HEIGHT = 60.0f;

void SelectingLobby::draw(sf::RenderWindow& window, GameState& gameState) {
    static std::unique_ptr<sf::Texture> backgroundTexture; // texture du fond d'écran
    static std::vector<std::unique_ptr<Selectable>> lobbySelectables; // liste des éléments sélectionnables
    static std::unique_ptr<Button> joinButton; // bouton de joindre un lobby
    
    if (!backgroundTexture) {
        backgroundTexture = Background::load("menu.png"); // charge la texture du fond d'écran
    }
    if (backgroundTexture) {
        Background::drawFullscreen(window, *backgroundTexture); // affiche le fond d'écran
    }
    
    Text::draw(window, "Select a Lobby:", 50, 30, 30); // affiche le texte "Select a Lobby:"

    const auto& lobbies = gameState.getLobbies();
    
    if (lobbySelectables.size() != lobbies.size()) {
        lobbySelectables.clear(); // vide la liste des éléments sélectionnables
        for (size_t i = 0; i < lobbies.size(); i++) {
            float y = LOBBY_Y_START + i * LOBBY_SPACING; // calcule la position y de l'élément sélectionnable
            lobbySelectables.push_back(std::make_unique<Selectable>(LOBBY_X, y, LOBBY_WIDTH, LOBBY_HEIGHT)); // crée un élément sélectionnable à la position x, y, avec la largeur et la hauteur spécifiées
        }
    }
    
    if (!joinButton) {
        joinButton = std::make_unique<Button>(BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, "Join", 28); // crée un bouton de joindre un lobby à la position x, y, avec la largeur et la hauteur spécifiées
    }
    
    for (size_t i = 0; i < lobbySelectables.size() && i < lobbies.size(); i++) {
        lobbySelectables[i]->setSelected(gameState.getSelectedLobby() == lobbies[i].lobbyId); // définit si l'élément sélectionnable est sélectionné
        lobbySelectables[i]->draw(window); // affiche l'élément sélectionnable
        
        std::stringstream ss; // crée un stream de chaîne de caractères
        ss << "Lobby #" << lobbies[i].lobbyId << " - Players: " << lobbies[i].playerCount; // ajoute le nom du lobby et le nombre de joueurs
        if (lobbies[i].gameStarted) {
            ss << " (Started)"; // ajoute le statut de la partie
        }
        
        float y = LOBBY_Y_START + i * LOBBY_SPACING; // calcule la position y de l'élément sélectionnable
        Text::draw(window, ss.str(), 70, y + 20, 22); // affiche le texte
    }
    
    joinButton->setEnabled(gameState.getSelectedLobby() != -1); // définit si le bouton de joindre un lobby est actif
    joinButton->draw(window); // affiche le bouton de joindre un lobby
}

bool SelectingLobby::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    static std::vector<std::unique_ptr<Selectable>> lobbySelectables; // liste des éléments sélectionnables
    static std::unique_ptr<Button> joinButton; // bouton de joindre un lobby

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y)); // récupère la position de la souris

        const auto& lobbies = gameState.getLobbies();
        
        if (lobbySelectables.size() != lobbies.size()) {
            lobbySelectables.clear(); // vide la liste des éléments sélectionnables
            for (size_t i = 0; i < lobbies.size(); i++) {
                float y = LOBBY_Y_START + i * LOBBY_SPACING; // calcule la position y de l'élément sélectionnable
                lobbySelectables.push_back(std::make_unique<Selectable>(LOBBY_X, y, LOBBY_WIDTH, LOBBY_HEIGHT));
            }
        }
        
        if (!joinButton) {
            joinButton = std::make_unique<Button>(BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, "Join", 28);
        }

        for (size_t i = 0; i < lobbySelectables.size() && i < lobbies.size(); i++) {
            if (lobbySelectables[i]->contains(mousePos.x, mousePos.y)) {
                gameState.setSelectedLobby(lobbies[i].lobbyId); // définit l'identifiant du lobby sélectionné
                return false;
            }
        }

        if (joinButton && joinButton->contains(mousePos.x, mousePos.y) && gameState.getSelectedLobby() != -1) { // vérifie si le bouton de joindre un lobby est cliqué et si un lobby est sélectionné
            gameState.setState(ClientState::ENTERING_USERNAME); // passe à l'état d'entrée du nom d'utilisateur
            return false;
        }
    }

    return false;
}
