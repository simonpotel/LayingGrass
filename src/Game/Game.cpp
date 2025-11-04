#include "Game/Game.hpp"
#include "Packet.hpp"
#include <algorithm>
#include <random>
#include <thread>
#include <chrono>

Game::Game(int lobbyId, Lobby* lobby)
    : lobbyId(lobbyId), lobby(lobby), board(20), currentTurnIndex(0), turnCount(0), winnerId(-1), rng(std::random_device{}()) {
    initializePlayers();
}

Game::~Game() {
}

void Game::initializePlayers() {
    for (int conn : lobby->connections) { // parcourt les connexions des joueurs du lobby
        auto colorIt = lobby->playerColors.find(conn); // trouve la couleur du joueur
        if (colorIt != lobby->playerColors.end()) { // si la couleur du joueur est trouvée
            playerConnections.push_back(conn); // ajoute la connexion au joueur
        }
    }
    
    turnOrder.resize(playerConnections.size()); // redimensionne la liste des identifiants des joueurs dans l'ordre du tour
    for (size_t i = 0; i < playerConnections.size(); ++i) {
        turnOrder[i] = i; // définit l'identifiant du joueur à l'index i
    }
    
    std::shuffle(turnOrder.begin(), turnOrder.end(), rng); // mélange la liste des identifiants des joueurs dans l'ordre du tour avec le générateur de nombres aléatoires
    currentTurnIndex = 0; // définit l'index du joueur dont c'est le tour
}

int Game::getPlayerColorId(int connection) const {
    auto it = lobby->playerColors.find(connection); // trouve la couleur du joueur
    if (it != lobby->playerColors.end()) {
        return it->second; // retourne la couleur du joueur
    }
    return -1; // retourne -1 si la couleur du joueur n'est pas trouvée
}

void Game::update() {
    broadcastBoardUpdate(); // envoie le paquet de mise à jour de la grille à tous les joueurs du lobby
}

void Game::handleCellClick(int connection, int row, int col) {
    if (isGameOver()) { // si la partie est terminée
        return; // on ne fait rien
    }
    
    int currentPlayerConn = getCurrentPlayerConnection(); // obtient le descripteur de socket du joueur dont c'est le tour
    if (connection != currentPlayerConn) { // si le joueur n'est pas le joueur dont c'est le tour
        return;
    }
    
    if (board.getCell(row, col) != -1) { // si la cellule est déjà remplie
        return;
    }
    
    int colorId = getPlayerColorId(connection); // obtient l'identifiant de la couleur du joueur
    if (colorId == -1) { // si la couleur du joueur n'est pas trouvée
        return;
    }
    
    board.setCell(row, col, colorId); // définit la valeur de la cellule
    turnCount++; // incrémente le nombre de tours
    
    if (isGameOver()) { // si la partie est terminée
        endGame(); // termine la partie
    } else { 
        nextTurn(); // passe au joueur suivant
    }
    
    broadcastBoardUpdate(); // envoie le paquet de mise à jour de la grille à tous les joueurs du lobby
}

void Game::nextTurn() {
    currentTurnIndex = (currentTurnIndex + 1) % turnOrder.size(); // passe au joueur suivant
}

void Game::endGame() {
    if (playerConnections.empty()) { // si il n'y a pas de joueurs
        winnerId = -1; // définit le gagnant à -1 (pas de gagnant)
        return;
    }
    
    std::uniform_int_distribution<int> dist(0, playerConnections.size() - 1); // génère un nombre aléatoire entre 0 et le nombre de joueurs
    int winnerIndex = dist(rng); // obtient l'index du joueur gagnant
    winnerId = getPlayerColorId(playerConnections[winnerIndex]); // obtient l'identifiant de la couleur du joueur gagnant
}

int Game::getCurrentPlayerConnection() const {
    if (turnOrder.empty() || currentTurnIndex >= static_cast<int>(turnOrder.size())) { // si il n'y a pas de joueurs ou si l'index du joueur est hors limites
        return -1;
    }
    int playerIndex = turnOrder[currentTurnIndex]; // obtient l'index du joueur dont c'est le tour
    return playerConnections[playerIndex]; // retourne le descripteur de socket du joueur dont c'est le tour
}

void Game::broadcastBoardUpdate() {
    BoardUpdatePacket packet; // paquet de mise à jour de la grille
    packet.lobbyId = lobbyId; // définit l'identifiant du lobby
    packet.size = board.getSize(); // définit la taille de la grille
    
    int currentPlayerConn = getCurrentPlayerConnection(); // obtient le descripteur de socket du joueur dont c'est le tour
    packet.currentTurnColorId = getPlayerColorId(currentPlayerConn); // obtient l'identifiant de la couleur du joueur dont c'est le tour
    
    packet.turnCount = turnCount; // définit le nombre de tours
    packet.gameOver = isGameOver(); // définit si la partie est terminée
    packet.winnerId = winnerId; // définit l'identifiant de la couleur du gagnant
    
    for (int i = 0; i < board.getSize() && i < 30; ++i) { // parcourt la grille
        for (int j = 0; j < board.getSize() && j < 30; ++j) {
            packet.grid[i * 30 + j] = board.getCell(i, j); // définit la valeur de la cellule
        }
    }
    
    lobby->broadcast(PacketType::BOARD_UPDATE, &packet, sizeof(BoardUpdatePacket)); // envoie le paquet de mise à jour de la grille à tous les joueurs du lobby
}

