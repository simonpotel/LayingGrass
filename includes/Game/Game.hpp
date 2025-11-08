#pragma once
#include "Game/Board.hpp"
#include "Game/Tile.hpp"
#include "Lobby.hpp"
#include <vector>
#include <memory>
#include <random>
#include <unordered_map>

class Game {
public:
    Game(int lobbyId, Lobby* lobby); // constructeur du jeu avec l'identifiant du lobby et le lobby associé
    ~Game();
    
    void update(); // met à jour le jeu
    void handleCellClick(int connection, int row, int col, int rotation, bool flippedH, bool flippedV, bool useCoupon); // gère le clic sur une cellule ou l'utilisation d'un coupon
    bool isGameOver() const { return turnCount >= 9; } // retourne true si la partie est terminée
    int getWinner() const { return winnerId; } // retourne l'identifiant de la couleur du gagnant
    int getCurrentPlayerConnection() const; // retourne le descripteur de socket du joueur dont c'est le tour
    Board* getBoard() { return &board; } // retourne la grille du jeu
    
    void broadcastBoardUpdate(); // envoie le paquet de mise à jour de la grille à tous les joueurs du lobby
    
    // Gestion des tuiles
    int getCurrentPlayerTileId(int connection) const; // retourne l'ID de la tuile actuelle du joueur (-1 si aucune)
    bool isFirstTurnForPlayer(int connection) const; // vérifie si c'est le premier tour du joueur

    void addExchangeCoupon(int connection, int count = 1); // ajoute des coupons d'échange au joueur
    int getExchangeCouponCount(int connection) const; // retourne le nombre de coupons d'échange du joueur
    bool useExchangeCoupon(int connection, int row, int col); // tente d'utiliser un coupon
    bool hasRemainingCoupons() const; // indique si des coupons restent à utiliser
    
    // Méthodes utilitaires pour le calcul du gagnant
    int getPlayerTerritoryCount(int playerId) const; // compte le nombre de cellules d'un joueur
    int getPlayerLargestSquare(int playerId) const; // trouve le plus grand carré d'un joueur
    
private:
    void initializePlayers(); // initialise les joueurs du jeu
    void nextTurn(); // passe au joueur suivant et distribue une nouvelle tuile
    void endGame(); // termine la partie
    int getPlayerColorId(int connection) const; // retourne l'identifiant de la couleur du joueur
    void giveTileToPlayer(int connection, bool forceRandom = false); // distribue une tuile au joueur (1x1 au premier tour, aléatoire sinon)
    bool canPlaceTile(int connection, int tileId, int anchorRow, int anchorCol) const; // Vérifie si une tuile peut être placée
    bool placeTile(int connection, int tileId, int anchorRow, int anchorCol); // Place une tuile sur le plateau
    void placeExchangeCoupons(); // positionne des cases coupon sur le plateau
    void computeWinner(); // calcule le gagnant en fonction du territoire
    void finalizeWinnerIfReady(); // termine la partie si tous les coupons sont utilisés
    
    int lobbyId; // identifiant du lobby
    Lobby* lobby; // lobby associé au jeu
    Board board; // grille du jeu
    std::vector<int> playerConnections; // liste des descripteurs de socket des joueurs
    std::vector<int> turnOrder; // liste des identifiants des joueurs dans l'ordre du tour
    int currentTurnIndex; // index du joueur dont c'est le tour
    int turnCount; // nombre de tours effectués
    int winnerId; // identifiant de la couleur du gagnant
    
    // Gestion des tuiles : map connection -> tileId (-1 si aucune tuile)
    std::unordered_map<int, int> playerTiles; // tuile actuelle de chaque joueur
    std::unordered_map<int, int> playerTurnsPlayed; // nombre de tours joués par chaque joueur
    std::unordered_map<int, int> playerExchangeCoupons; // coupons d'échange disponibles par joueur
    
    std::mt19937 rng; // générateur de nombres aléatoires
    bool awaitingFinalCoupons; // true si la partie attend que les joueurs utilisent/écartent leurs coupons
};

