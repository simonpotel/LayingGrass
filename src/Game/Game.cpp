#include "Game/Game.hpp"
#include "Game/Cell.hpp"
#include "Game/Tile.hpp"
#include "Packet.hpp"
#include <algorithm>
#include <random>
#include <thread>
#include <chrono>
#include <cstring>

Game::Game(int lobbyId, Lobby* lobby)
    : lobbyId(lobbyId), lobby(lobby), board(20), currentTurnIndex(0), turnCount(0), winnerId(-1), rng(std::random_device{}()) {
    initializePlayers();
    
    int firstPlayer = getCurrentPlayerConnection();
    if (firstPlayer != -1) {
        giveTileToPlayer(firstPlayer);
    }
}

Game::~Game() {
}

void Game::initializePlayers() {
    for (int conn : lobby->connections) { // parcourt les connexions des joueurs du lobby
        auto colorIt = lobby->playerColors.find(conn); // trouve la couleur du joueur
        if (colorIt != lobby->playerColors.end()) { // si la couleur du joueur est trouvée
            playerConnections.push_back(conn); // ajoute la connexion au joueur
            playerTurnsPlayed[conn] = 0; // initialise le nombre de tours joués à 0
            playerTiles[conn] = -1; // aucune tuile pour l'instant
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
    
    // Vérifie que le joueur a une tuile
    if (playerTiles.find(connection) == playerTiles.end() || playerTiles[connection] == -1) {
        return; // pas de tuile à placer
    }
    
    int tileId = playerTiles[connection];
    int colorId = getPlayerColorId(connection); // obtient l'identifiant de la couleur du joueur
    if (colorId == -1) { // si la couleur du joueur n'est pas trouvée
        return;
    }
    
    // Le clic (row, col) représente où le joueur veut placer la tuile
    // On considère que c'est le point d'ancrage (premier bloc de la tuile)
    // Essaye de placer la tuile à cette position
    if (placeTile(connection, tileId, row, col)) {
        // Placement réussi
        playerTiles[connection] = -1; // Retire la tuile du joueur après placement
        playerTurnsPlayed[connection]++;
        turnCount++; // incrémente le nombre de tours
        
        if (isGameOver()) { // si la partie est terminée
            endGame(); // termine la partie
        } else { 
            nextTurn(); // passe au joueur suivant et distribue une nouvelle tuile
        }
        
        broadcastBoardUpdate(); // envoie le paquet de mise à jour de la grille à tous les joueurs du lobby
    }
    // Si le placement échoue, on ne fait rien (le joueur peut réessayer)
}

void Game::nextTurn() {
    currentTurnIndex = (currentTurnIndex + 1) % turnOrder.size(); // passe au joueur suivant
    
    // Distribue une nouvelle tuile au joueur dont c'est le tour
    int nextPlayer = getCurrentPlayerConnection();
    if (nextPlayer != -1) {
        giveTileToPlayer(nextPlayer);
    }
}

void Game::giveTileToPlayer(int connection) {
    if (isFirstTurnForPlayer(connection)) {
        playerTiles[connection] = static_cast<int>(TileId::TILE_0);
    } else {
        std::uniform_int_distribution<int> tileDist(0, static_cast<int>(TileId::TOTAL_TILES) - 1);
        playerTiles[connection] = tileDist(rng);
    }
}

int Game::getCurrentPlayerTileId(int connection) const {
    auto it = playerTiles.find(connection);
    if (it != playerTiles.end()) {
        return it->second;
    }
    return -1;
}

bool Game::isFirstTurnForPlayer(int connection) const {
    auto it = playerTurnsPlayed.find(connection);
    if (it != playerTurnsPlayed.end()) {
        return it->second == 0; // Premier tour si aucun tour joué
    }
    return true; // Si pas trouvé, on considère que c'est le premier tour
}

bool Game::canPlaceTile(int connection, int tileId, int anchorRow, int anchorCol) const {
    const Tile& tile = Tile::getTile(Tile::fromInt(tileId));
    if (!tile.isValid()) {
        return false;
    }
    
    int colorId = getPlayerColorId(connection);
    if (colorId == -1) {
        return false;
    }
    
    bool isFirstTurn = isFirstTurnForPlayer(connection);
    bool touchesOwnTerritory = false;
    
    // Vérifie chaque bloc de la tuile
    for (const auto& [blockRow, blockCol] : tile.blocks) {
        int actualRow = anchorRow + blockRow;
        int actualCol = anchorCol + blockCol;
        
        // Vérifie que la position est dans les limites
        if (!board.isValidPosition(actualRow, actualCol)) {
            return false;
        }
        
        // Vérifie que la cellule est vide
        if (!board.isEmpty(actualRow, actualCol)) {
            return false;
        }
        
        // Vérifie qu'on ne touche pas le territoire d'un autre joueur
        if (board.touchesOtherPlayerTerritory(actualRow, actualCol, colorId)) {
            return false;
        }
        
        // Pour les tours suivants (pas le premier), vérifie qu'au moins un bloc touche le territoire du joueur
        if (!isFirstTurn && board.hasAdjacentPlayerCell(actualRow, actualCol, colorId)) {
            touchesOwnTerritory = true;
        }
    }
    
    // Pour les tours suivants, au moins un bloc doit toucher le territoire du joueur
    if (!isFirstTurn && !touchesOwnTerritory) {
        return false;
    }
    
    return true;
}

bool Game::placeTile(int connection, int tileId, int anchorRow, int anchorCol) {
    if (!canPlaceTile(connection, tileId, anchorRow, anchorCol)) {
        return false;
    }
    
    const Tile& tile = Tile::getTile(Tile::fromInt(tileId));
    int colorId = getPlayerColorId(connection);
    
    if (colorId == -1) {
        return false;
    }
    
    // Place tous les blocs de la tuile
    for (const auto& [blockRow, blockCol] : tile.blocks) {
        int actualRow = anchorRow + blockRow;
        int actualCol = anchorCol + blockCol;
        
        // Place la cellule avec la couleur du joueur
        board.setCell(actualRow, actualCol, Cell(colorId));
    }
    
    return true;
}

void Game::endGame() {
    if (playerConnections.empty()) { // si il n'y a pas de joueurs
        winnerId = -1; // définit le gagnant à -1 (pas de gagnant)
        return;
    }
    
    // TODO: Implémenter la vraie logique de victoire selon les règles du jeu
    // Pour l'instant, on garde le système aléatoire temporaire
    std::uniform_int_distribution<int> dist(0, playerConnections.size() - 1); // génère un nombre aléatoire entre 0 et le nombre de joueurs
    int winnerIndex = dist(rng); // obtient l'index du joueur gagnant
    winnerId = getPlayerColorId(playerConnections[winnerIndex]); // obtient l'identifiant de la couleur du joueur gagnant
    
    // Exemple d'utilisation de la classe Cell pour trouver le gagnant :
    // int bestPlayer = -1;
    // int bestSquare = 0;
    // int bestTerritory = 0;
    // 
    // for (int conn : playerConnections) {
    //     int colorId = getPlayerColorId(conn);
    //     int square = getPlayerLargestSquare(colorId);
    //     int territory = getPlayerTerritoryCount(colorId);
    //     
    //     if (square > bestSquare || (square == bestSquare && territory > bestTerritory)) {
    //         bestSquare = square;
    //         bestTerritory = territory;
    //         bestPlayer = colorId;
    //     }
    // }
    // winnerId = bestPlayer;
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
    memset(&packet, 0, sizeof(packet)); // Initialise le packet à 0
    packet.lobbyId = lobbyId; // définit l'identifiant du lobby
    packet.size = board.getSize(); // définit la taille de la grille
    
    int currentPlayerConn = getCurrentPlayerConnection(); // obtient le descripteur de socket du joueur dont c'est le tour
    packet.currentTurnColorId = getPlayerColorId(currentPlayerConn); // obtient l'identifiant de la couleur du joueur dont c'est le tour
    
    // Récupère la tuile du joueur actif
    packet.currentPlayerTileId = getCurrentPlayerTileId(currentPlayerConn);
    
    packet.turnCount = turnCount; // définit le nombre de tours
    packet.gameOver = isGameOver(); // définit si la partie est terminée
    packet.winnerId = winnerId; // définit l'identifiant de la couleur du gagnant
    
    for (int i = 0; i < board.getSize() && i < 30; ++i) { // parcourt la grille
        for (int j = 0; j < board.getSize() && j < 30; ++j) {
            packet.grid[i * 30 + j] = board.getCell(i, j).toInt(); // convertit la cellule en int pour le réseau
        }
    }
    
    lobby->broadcast(PacketType::BOARD_UPDATE, &packet, sizeof(BoardUpdatePacket)); // envoie le paquet de mise à jour de la grille à tous les joueurs du lobby
}

int Game::getPlayerTerritoryCount(int playerId) const {
    int count = 0;
    for (int i = 0; i < board.getSize(); ++i) {
        for (int j = 0; j < board.getSize(); ++j) {
            if (board.isPlayerCell(i, j, playerId)) {
                count++;
            }
        }
    }
    return count;
}

int Game::getPlayerLargestSquare(int playerId) const {
    // TODO: Implémenter l'algorithme pour trouver le plus grand carré
    // Pour l'instant, on retourne 0
    // Cette méthode devra parcourir toutes les cellules et trouver le plus grand carré
    // formé uniquement par les cellules du joueur
    
    int maxSquare = 0;
    int size = board.getSize();
    
    // Algorithme simple : pour chaque position, vérifier si on peut former un carré
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (!board.isPlayerCell(i, j, playerId)) {
                continue;
            }
            
            // Essayer de former un carré de plus en plus grand
            for (int squareSize = 1; squareSize <= size - std::max(i, j); ++squareSize) {
                bool isValidSquare = true;
                
                // Vérifier si toutes les cellules du carré appartiennent au joueur
                for (int di = 0; di < squareSize && isValidSquare; ++di) {
                    for (int dj = 0; dj < squareSize && isValidSquare; ++dj) {
                        if (!board.isPlayerCell(i + di, j + dj, playerId)) {
                            isValidSquare = false;
                        }
                    }
                }
                
                if (isValidSquare) {
                    maxSquare = std::max(maxSquare, squareSize);
                } else {
                    break; // Pas besoin de continuer avec des carrés plus grands
                }
            }
        }
    }
    
    return maxSquare;
}

