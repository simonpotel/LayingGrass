#include "Render/screens/InGame.hpp"
#include "Render/utils/Text.hpp"
#include "Render/utils/Board.hpp"
#include "Client.hpp"
#include <sstream>
#include <iostream>

extern Client* g_client;

void InGame::draw(sf::RenderWindow& window, GameState& gameState) {
    int currentLobbyId = gameState.getCurrentLobby();
    const auto& board = gameState.getBoard(); // récupère la grille du jeu
    int boardSize = gameState.getBoardSize(); // récupère la taille de la grille
    
    float boardX = 50.0f; // définit la position x du board
    float boardY = 50.0f; // définit la position y du board
    float cellSize = 15.0f; // définit la taille des cellules
    
    Board::draw(window, board, boardSize, boardX, boardY, cellSize); // dessine la grille
    
    std::stringstream ss;
    ss << "Lobby #" << currentLobbyId; // définit le nom du lobby
    Text::draw(window, ss.str(), 50, 20, 24);
    
    ss.str("");
    ss << "Tour: " << gameState.getTurnCount() << "/9"; // définit le nombre de tours
    Text::draw(window, ss.str(), 400, 20, 24);
    
    if (gameState.isGameOver()) {
        ss.str(""); // vide le stream
        if (gameState.getWinnerId() >= 0) {
            ss << "Gagnant: Couleur " << gameState.getWinnerId(); // définit le gagnant
        } else {
            ss << "Partie terminee"; // définit la fin de la partie
        }
        Text::draw(window, ss.str(), 200, 400, 32);
    } else {
        int currentTurnColorId = gameState.getCurrentTurnColorId(); // récupère la couleur du joueur dont c'est le tour
        int myColorId = gameState.getSelectedColor(); // récupère la couleur du joueur
        if (currentTurnColorId == myColorId) {
            Text::draw(window, "Votre tour!", 400, 50, 24); // définit le tour du joueur    
        } else {
            Text::draw(window, "Tour de l'adversaire", 400, 50, 24);
        }
    }
}

bool InGame::handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (gameState.isGameOver()) { // si la partie est terminée
            return false; // on ne fait rien quand un joueur clique sur le board 
        }
        
        int currentTurnColorId = gameState.getCurrentTurnColorId(); // récupère la couleur du joueur dont c'est le tour
        int myColorId = gameState.getSelectedColor(); // récupère la couleur du joueur
        
        if (currentTurnColorId != myColorId) { // si le joueur n'est pas le joueur dont c'est le tour
            return false;
        }
        
        float boardX = 50.0f; // définit la position x du board
        float boardY = 50.0f; // définit la position y du board
        float cellSize = 15.0f; // définit la taille des cellules
        int boardSize = gameState.getBoardSize(); // récupère la taille de la grille
        
        int row, col;
        if (Board::handleClick(event.mouseButton.x, event.mouseButton.y, boardX, boardY, cellSize, boardSize, row, col)) { // si le joueur clique sur le board
            if (g_client) {
                g_client->sendCellClick(gameState.getCurrentLobby(), row, col); // envoie le clic au serveur
            }
        }
    }
    return false;
}

