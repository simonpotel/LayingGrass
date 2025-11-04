#pragma once
#include "Game/Board.hpp"
#include "Lobby.hpp"
#include <vector>
#include <memory>
#include <random>

class Game {
public:
    Game(int lobbyId, Lobby* lobby); // constructeur du jeu avec l'identifiant du lobby et le lobby associé
    ~Game();
    
    void update(); // met à jour le jeu
    void handleCellClick(int connection, int row, int col); // gère le clic sur une cellule (paquet de type CELL_CLICK)
    bool isGameOver() const { return turnCount >= 9; } // retourne true si la partie est terminée
    int getWinner() const { return winnerId; } // retourne l'identifiant de la couleur du gagnant
    int getCurrentPlayerConnection() const; // retourne le descripteur de socket du joueur dont c'est le tour
    Board* getBoard() { return &board; } // retourne la grille du jeu
    
    void broadcastBoardUpdate(); // envoie le paquet de mise à jour de la grille à tous les joueurs du lobby
    
private:
    void initializePlayers(); // initialise les joueurs du jeu
    void nextTurn(); // passe au joueur suivant
    void endGame(); // termine la partie
    int getPlayerColorId(int connection) const; // retourne l'identifiant de la couleur du joueur
    
    int lobbyId; // identifiant du lobby
    Lobby* lobby; // lobby associé au jeu
    Board board; // grille du jeu
    std::vector<int> playerConnections; // liste des descripteurs de socket des joueurs
    std::vector<int> turnOrder; // liste des identifiants des joueurs dans l'ordre du tour
    int currentTurnIndex; // index du joueur dont c'est le tour
    int turnCount; // nombre de tours effectués
    int winnerId; // identifiant de la couleur du gagnant

    //temp 
    std::mt19937 rng; // générateur de nombres aléatoires (temporaire à retirer quand on aura un système de victoire définir d'un joueur )
};

