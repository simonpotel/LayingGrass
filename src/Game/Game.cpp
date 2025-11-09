#include "Game/Game.hpp"
#include "Game/Cell.hpp"
#include "Game/Tile.hpp"
#include "Game/PlacementRules.hpp"
#include "Packet.hpp"
#include <algorithm>
#include <random>
#include <cmath>
#include <numeric>
#include <thread>
#include <chrono>
#include <cstring>
#include <iostream>

Game::Game(int lobbyId, Lobby* lobby)
    : lobbyId(lobbyId), lobby(lobby), currentTurnIndex(0), turnCount(0), winnerId(-1), rng(std::random_device{}()), awaitingFinalCoupons(false), tileQueueIndex(0) {
    // détermine la taille de la grille selon le nombre de joueurs
    // 2-4 joueurs : grille 20x20, 5-9 joueurs : grille 30x30
    int playerCount = lobby->getPlayerCount();
    int boardSize = (playerCount >= 2 && playerCount <= 4) ? 20 : 30;
    board = Board(boardSize); // initialise le board avec la bonne taille
    
    initializePlayers();
    initializeTileQueue(); // initialise la queue de tuiles prédéfinie mais aléatoire
    placeExchangeCoupons();
    
    int firstPlayer = getCurrentPlayerConnection();
    if (firstPlayer != -1) {
        giveTileToPlayer(firstPlayer);
    }
    
    // broadcast initial pour que tous les clients reçoivent l'état de départ
    broadcastBoardUpdate();
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
            playerExchangeCoupons[conn] = 0; // aucun coupon au départ
            playerPendingStoneBonus[conn] = false; // aucun bonus de pierre en attente
            playerPendingRobberyBonus[conn] = false; // aucun bonus de vol en attente
        }
    }
    
    turnOrder.resize(playerConnections.size()); // redimensionne la liste des identifiants des joueurs dans l'ordre du tour
    for (size_t i = 0; i < playerConnections.size(); ++i) {
        turnOrder[i] = i; // définit l'identifiant du joueur à l'index i
    }
    
    std::shuffle(turnOrder.begin(), turnOrder.end(), rng); // mélange la liste des identifiants des joueurs dans l'ordre du tour avec le générateur de nombres aléatoires
    currentTurnIndex = 0; // définit l'index du joueur dont c'est le tour
}

void Game::initializeTileQueue() {
    int playerCount = lobby->getPlayerCount();
    // calcule le nombre de tuiles nécessaires : 10.67 par joueur, arrondi
    // cela inclut la tuile de départ (1x1) + 9 tuiles pour les tours + quelques tuiles en plus pour les coupons
    int totalTilesNeeded = static_cast<int>(std::round(playerCount * 10.67));
    
    // crée une liste de toutes les 96 tuiles possibles
    tileQueue.clear();
    tileQueue.reserve(96);
    for (int i = 0; i < static_cast<int>(TileId::TOTAL_TILES); ++i) {
        tileQueue.push_back(i);
    }
    
    // mélange la queue de manière aléatoire (ordre prédéterminé mais aléatoire)
    std::shuffle(tileQueue.begin(), tileQueue.end(), rng);
    
    // ne garde que le nombre de tuiles nécessaires
    // note: la tuile de départ (TILE_0) n'est pas dans cette queue, elle est donnée séparément
    if (totalTilesNeeded < static_cast<int>(tileQueue.size())) {
        tileQueue.resize(totalTilesNeeded);
    }
    
    tileQueueIndex = 0; // réinitialise l'index
}

int Game::getPlayerColorId(int connection) const {
    auto it = lobby->playerColors.find(connection); // trouve la couleur du joueur
    if (it != lobby->playerColors.end()) {
        return it->second; // retourne la couleur du joueur
    }
    return -1; // retourne -1 si la couleur du joueur n'est pas trouvée
}

void Game::update() {
    // ne broadcast que si nécessaire (évite d'écraser les updates importants)
    // les broadcasts sont déjà gérés par les méthodes spécifiques (handleCellClick, robTile, etc.)
    // cette méthode est appelée en continu par la boucle du serveur, donc on évite de spammer
}

void Game::handleCellClick(int connection, int row, int col, int rotation, bool flippedH, bool flippedV, bool useCoupon) {
    if (useCoupon) {
        int currentPlayerConn = getCurrentPlayerConnection();
        if (!isGameOver()) {
            if (connection != currentPlayerConn) {
                return;
            }
            if (getExchangeCouponCount(connection) <= 0) {
                return;
            }

            playerExchangeCoupons[connection] = std::max(0, playerExchangeCoupons[connection] - 1);
            giveTileToPlayer(connection, true);
            broadcastBoardUpdate();
            return;
        }

        if (useExchangeCoupon(connection, row, col)) {
            finalizeWinnerIfReady();
        }
        return;
    }

    if (isGameOver()) { // si la partie est terminée
        return; // on ne fait rien
    }
    
    int currentPlayerConn = getCurrentPlayerConnection(); // obtient le descripteur de socket du joueur dont c'est le tour
    if (connection != currentPlayerConn) { // si le joueur n'est pas le joueur dont c'est le tour
        return;
    }
    
    // vérifie que le joueur a une tuile
    if (playerTiles.find(connection) == playerTiles.end() || playerTiles[connection] == -1) {
        return; // pas de tuile à placer
    }
    
    int tileId = playerTiles[connection];
    int colorId = getPlayerColorId(connection); // obtient l'identifiant de la couleur du joueur
    if (colorId == -1) { // si la couleur du joueur n'est pas trouvée
        return;
    }
    
    // applique les transformations à la tuile
    const Tile& baseTile = Tile::getTile(Tile::fromInt(tileId));
    Tile transformedTile = baseTile;
    
    // applique les rotations (0-3 fois 90°)
    for (int i = 0; i < rotation; ++i) {
        transformedTile = transformedTile.rotate90();
    }
    
    // applique le flip horizontal si nécessaire
    if (flippedH) {
        transformedTile = transformedTile.flipHorizontal();
    }
    
    // applique le flip vertical si nécessaire
    if (flippedV) {
        transformedTile = transformedTile.flipVertical();
    }
    
    // vérifie si la tuile transformée peut être placée
    bool isFirstTurn = isFirstTurnForPlayer(connection);
    if (!PlacementRules::canPlaceTile(board, transformedTile, row, col, colorId, isFirstTurn)) {
        return; // placement invalide
    }
    
    // place la tuile transformée
    // note: si on place sur un bonus, il est perdu (écrasé)
    for (const auto& [blockRow, blockCol] : transformedTile.blocks) {
        int actualRow = row + blockRow;
        int actualCol = col + blockCol;
        
        // si c'est un bonus, on l'écrase (perdu selon la consigne)
        // sinon, on place la cellule avec la couleur du joueur
        board.setCell(actualRow, actualCol, Cell(colorId));
    }
    
    // vérifie les bonus capturés après le placement (4 directions cardinales)
    // un bonus est capturé si les 4 directions cardinales sont occupées par le joueur
    int size = board.getSize();
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            if (!board.isBonusCaptured(r, c, colorId)) {
                continue;
            }
            
            // bonus capturé !
            const Cell& bonusCell = board.getCell(r, c);
            CellType bonusType = bonusCell.getType();
            
            // applique l'effet du bonus
            if (bonusType == CellType::BONUS_EXCHANGE) {
                addExchangeCoupon(connection);
            } else if (bonusType == CellType::BONUS_STONE) {
                playerPendingStoneBonus[connection] = true;
            } else if (bonusType == CellType::BONUS_ROBBERY) {
                playerPendingRobberyBonus[connection] = true;
            }
            
            // le bonus devient partie du territoire du joueur
            board.setCell(r, c, Cell(colorId));
        }
    }
    
    // placement réussi
    playerTiles[connection] = -1; // retire la tuile du joueur après placement
    playerTurnsPlayed[connection]++;
    turnCount++; // incrémente le nombre d'actions individuelles
    
    // vérifie si le joueur a un bonus en attente qui doit être utilisé immédiatement
    bool hasPendingBonus = playerPendingStoneBonus[connection] || playerPendingRobberyBonus[connection];
    
    if (isGameOver()) { // si la partie est terminée
        endGame(); // termine la partie
    } else if (!hasPendingBonus) { 
        // ne passe au joueur suivant que s'il n'a pas de bonus en attente
        nextTurn(); // passe au joueur suivant et distribue une nouvelle tuile
    }
    // si le joueur a un bonus en attente, le tour reste au même joueur jusqu'à ce qu'il utilise son bonus
    
    broadcastBoardUpdate(); // envoie le paquet de mise à jour de la grille à tous les joueurs du lobby
}

void Game::nextTurn() {
    // si on a fait un tour complet (tous les joueurs ont joué), on passe au tour suivant
    // sinon, on passe juste au joueur suivant
    currentTurnIndex = (currentTurnIndex + 1) % turnOrder.size(); // passe au joueur suivant
    
    // distribue une nouvelle tuile au joueur dont c'est le tour
    int nextPlayer = getCurrentPlayerConnection();
    if (nextPlayer != -1) {
        // vérifie si le joueur a déjà une tuile (par exemple s'il a été volé)
        // si c'est son premier tour, on lui donne toujours une tuile
        // sinon, s'il n'a pas de tuile, c'est qu'il a été volé, donc on passe son tour
        bool hasTile = (playerTiles.find(nextPlayer) != playerTiles.end() && playerTiles[nextPlayer] != -1);
        bool isFirstTurn = isFirstTurnForPlayer(nextPlayer);
        
        if (isFirstTurn || hasTile) {
            // premier tour ou a déjà une tuile : on lui donne une tuile normalement
            giveTileToPlayer(nextPlayer);
        } else {
            // le joueur n'a pas de tuile et ce n'est pas son premier tour : il a été volé
            // on passe son tour sans lui donner de tuile
            std::cout << "[GAME] [NEXT_TURN] Player " << nextPlayer << " has no tile (stolen), skipping turn" << std::endl;
            playerTurnsPlayed[nextPlayer]++; // compte quand même le tour comme joué
            turnCount++; // incrémente le nombre d'actions
            // passe au joueur suivant
            nextTurn(); // récursion pour passer au joueur suivant
        }
    }
}

void Game::giveTileToPlayer(int connection, bool forceRandom) {
    if (isFirstTurnForPlayer(connection) && !forceRandom) {
        // premier tour : tuile 1x1 (TILE_0)
        playerTiles[connection] = static_cast<int>(TileId::TILE_0);
    } else {
        // distribue depuis la queue de tuiles prédéfinie
        if (tileQueueIndex < tileQueue.size()) {
            playerTiles[connection] = tileQueue[tileQueueIndex];
            tileQueueIndex++;
        } else {
            // si la queue est épuisée, génère aléatoirement (ne devrait pas arriver)
            std::uniform_int_distribution<int> tileDist(0, static_cast<int>(TileId::TOTAL_TILES) - 1);
            playerTiles[connection] = tileDist(rng);
        }
    }
}

void Game::placeExchangeCoupons() {
    int size = board.getSize();
    size_t playerCount = std::max<size_t>(1, playerConnections.size());
    
    // calcule le nombre de chaque type de bonus selon la consigne
    int exchangeToPlace = static_cast<int>(std::ceil(playerCount * 1.5)); // 1.5 par joueur, arrondi au supérieur
    int stoneToPlace = static_cast<int>(std::ceil(playerCount * 0.5)); // 0.5 par joueur, arrondi au supérieur
    int robberyToPlace = static_cast<int>(playerCount); // 1 par joueur
    
    // fonction helper pour placer un type de bonus
    auto placeBonusType = [this, size](CellType bonusType, int count) {
        // crée une liste de toutes les positions valides
        std::vector<std::pair<int, int>> validPositions;
        for (int row = 1; row < size - 1; ++row) {
            for (int col = 1; col < size - 1; ++col) {
                if (board.isValidBonusPosition(row, col)) {
                    validPositions.push_back({row, col});
                }
            }
        }
        
        // mélange les positions valides
        std::shuffle(validPositions.begin(), validPositions.end(), rng);
        
        // place les bonus
        const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        int positionIndex = 0;
        for (int i = 0; i < count && positionIndex < static_cast<int>(validPositions.size()); ++i) {
            auto [row, col] = validPositions[positionIndex++];
            board.setCell(row, col, Cell(bonusType));
            
            // retire les positions adjacentes qui ne sont plus valides
            for (int d = 0; d < 4; ++d) {
                int newRow = row + directions[d][0];
                int newCol = col + directions[d][1];
                validPositions.erase(
                    std::remove_if(validPositions.begin() + positionIndex, validPositions.end(),
                        [newRow, newCol](const std::pair<int, int>& p) {
                            return p.first == newRow && p.second == newCol;
                        }),
                    validPositions.end());
            }
        }
    };
    
    // place chaque type de bonus
    placeBonusType(CellType::BONUS_EXCHANGE, exchangeToPlace);
    placeBonusType(CellType::BONUS_STONE, stoneToPlace);
    placeBonusType(CellType::BONUS_ROBBERY, robberyToPlace);
}

bool Game::useExchangeCoupon(int connection, int row, int col) {
    if (playerExchangeCoupons[connection] <= 0) {
        return false;
    }

    if (row < 0 || col < 0) {
        playerExchangeCoupons[connection] = 0;
        return true;
    }

    if (!board.isValidPosition(row, col) || !board.isEmpty(row, col)) {
        return false;
    }

    int colorId = getPlayerColorId(connection);
    if (colorId == -1) {
        return false;
    }

    board.setCell(row, col, Cell(colorId));
    playerExchangeCoupons[connection]--;
    return true;
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
        return it->second == 0; // premier tour si aucun tour joué
    }
    return true; // si pas trouvé, on considère que c'est le premier tour
}

void Game::addExchangeCoupon(int connection, int count) {
    if (count <= 0) {
        return;
    }
    playerExchangeCoupons[connection] += count;
}

int Game::getExchangeCouponCount(int connection) const {
    auto it = playerExchangeCoupons.find(connection);
    if (it != playerExchangeCoupons.end()) {
        return it->second;
    }
    return 0;
}

bool Game::hasPendingStoneBonus(int connection) const {
    auto it = playerPendingStoneBonus.find(connection);
    if (it != playerPendingStoneBonus.end()) {
        return it->second;
    }
    return false;
}

bool Game::hasPendingRobberyBonus(int connection) const {
    auto it = playerPendingRobberyBonus.find(connection);
    if (it != playerPendingRobberyBonus.end()) {
        return it->second;
    }
    return false;
}

bool Game::placeStone(int connection, int row, int col) {
    // vérifie que c'est bien le tour du joueur
    int currentPlayerConn = getCurrentPlayerConnection();
    if (connection != currentPlayerConn) {
        return false; // ce n'est pas le tour de ce joueur
    }
    
    // vérifie que le joueur a un bonus de pierre en attente
    if (!hasPendingStoneBonus(connection)) {
        return false;
    }
    
    // vérifie que la position est valide et vide
    if (!board.isValidPosition(row, col) || !board.isEmpty(row, col)) {
        return false;
    }
    
    // place la pierre
    board.setCell(row, col, Cell(CellType::STONE));
    
    // retire le bonus en attente
    playerPendingStoneBonus[connection] = false;
    
    // met à jour le board AVANT de passer au joueur suivant
    broadcastBoardUpdate();
    
    // passe au joueur suivant maintenant que le bonus est utilisé
    if (isGameOver()) {
        endGame();
    } else {
        nextTurn(); // passe au joueur suivant et distribue une nouvelle tuile
        broadcastBoardUpdate(); // met à jour à nouveau pour le nouveau joueur actif
    }
    
    return true;
}

bool Game::robTile(int connection, int targetPlayerColorId) {
    // vérifie que c'est bien le tour du joueur
    int currentPlayerConn = getCurrentPlayerConnection();
    if (connection != currentPlayerConn) {
        return false; // ce n'est pas le tour de ce joueur
    }
    
    // vérifie que le joueur a un bonus de vol en attente
    if (!hasPendingRobberyBonus(connection)) {
        return false;
    }
    
    // trouve la connexion du joueur cible par sa couleur
    int targetConnection = -1;
    for (int conn : playerConnections) {
        if (getPlayerColorId(conn) == targetPlayerColorId) {
            targetConnection = conn;
            break;
        }
    }
    
    if (targetConnection == -1) {
        return false; // joueur cible non trouvé
    }
    
    // si le joueur cible n'a pas de tuile, on lui en donne une depuis la queue pour la voler
    auto it = playerTiles.find(targetConnection);
    int stolenTile = -1;
    
    if (it == playerTiles.end() || it->second == -1) {
        // le joueur cible n'a pas de tuile, on lui en donne une depuis la queue
        giveTileToPlayer(targetConnection, true); // forceRandom = true pour éviter la tuile 1x1
        it = playerTiles.find(targetConnection);
        if (it == playerTiles.end() || it->second == -1) {
            return false; // impossible de donner une tuile au joueur cible
        }
    }
    
    stolenTile = it->second;
    std::cout << "[GAME] [ROB_TILE] Stealing tile " << stolenTile << " from player " << targetPlayerColorId << " (connection " << targetConnection << ")" << std::endl;
    
    // vole la tuile : transfère la tuile du joueur cible au joueur actif
    playerTiles[targetConnection] = -1; // retire la tuile du joueur cible
    playerTiles[connection] = stolenTile; // donne la tuile au joueur actif
    
    std::cout << "[GAME] [ROB_TILE] After transfer - Robber (conn " << connection << ", color " << getPlayerColorId(connection) << ") now has tile " << playerTiles[connection] << std::endl;
    std::cout << "[GAME] [ROB_TILE] Victim (conn " << targetConnection << ", color " << targetPlayerColorId << ") now has tile " << playerTiles[targetConnection] << std::endl;
    
    // vérification : s'assure que la tuile est bien assignée
    if (playerTiles[connection] != stolenTile) {
        std::cout << "[GAME] [ROB_TILE] WARNING: Tile mismatch! Forcing assignment..." << std::endl;
        playerTiles[connection] = stolenTile; // force l'assignation si nécessaire
    }
    
    // retire le bonus en attente
    playerPendingRobberyBonus[connection] = false;
    
    // vérifie que le joueur actif est toujours le même (ne devrait pas changer)
    int verifyConn = getCurrentPlayerConnection();
    if (verifyConn != connection) {
        std::cout << "[GAME] [ROB_TILE] ERROR: Turn changed! Expected " << connection << " but got " << verifyConn << std::endl;
        // problème : le tour a changé, on ne peut pas voler
        return false;
    }
    
    std::cout << "[GAME] [ROB_TILE] Broadcasting board update - current player: " << verifyConn << ", tile: " << getCurrentPlayerTileId(verifyConn) << std::endl;
    // met à jour le board pour que le joueur voie sa tuile volée (il est encore le joueur actif)
    broadcastBoardUpdate();
    
    // le joueur qui a volé peut maintenant utiliser sa tuile volée
    // on ne passe PAS au joueur suivant immédiatement - le joueur reste actif pour utiliser sa tuile
    // le tour passera quand il placera sa tuile ou utilise un coupon
    
    return true;
}

bool Game::canPlaceTile(int connection, int tileId, int anchorRow, int anchorCol) const {
    int colorId = getPlayerColorId(connection);
    if (colorId == -1) {
        return false;
    }
    
    const Tile& tile = Tile::getTile(Tile::fromInt(tileId));
    bool isFirstTurn = isFirstTurnForPlayer(connection);

    return PlacementRules::canPlaceTile(board, tile, anchorRow, anchorCol, colorId, isFirstTurn);
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
    
    // place tous les blocs de la tuile
    for (const auto& [blockRow, blockCol] : tile.blocks) {
        int actualRow = anchorRow + blockRow;
        int actualCol = anchorCol + blockCol;
        
        // place la cellule avec la couleur du joueur
        board.setCell(actualRow, actualCol, Cell(colorId));
    }
    
    return true;
}

void Game::endGame() {
    if (playerConnections.empty()) { // si il n'y a pas de joueurs
        winnerId = -1; // définit le gagnant à -1 (pas de gagnant)
        return;
    }
    
    if (hasRemainingCoupons()) {
        awaitingFinalCoupons = true;
        return;
    }

    computeWinner();
    awaitingFinalCoupons = false;
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
    memset(&packet, 0, sizeof(packet)); // initialise le packet à 0
    packet.lobbyId = lobbyId; // définit l'identifiant du lobby
    packet.size = board.getSize(); // définit la taille de la grille
    
    int currentPlayerConn = getCurrentPlayerConnection(); // obtient le descripteur de socket du joueur dont c'est le tour
    packet.currentTurnColorId = getPlayerColorId(currentPlayerConn); // obtient l'identifiant de la couleur du joueur dont c'est le tour
    
    // récupère la tuile du joueur actif
    packet.currentPlayerTileId = getCurrentPlayerTileId(currentPlayerConn);
    
    // vérification de sécurité : si le joueur actif n'a pas de tuile mais qu'on devrait en avoir une, on vérifie
    if (currentPlayerConn != -1 && packet.currentPlayerTileId == -1) {
        // vérifie si le joueur a vraiment une tuile dans la map
        auto it = playerTiles.find(currentPlayerConn);
        if (it != playerTiles.end() && it->second != -1) {
            std::cout << "[GAME] [BROADCAST] WARNING: Tile mismatch detected! Correcting from -1 to " << it->second << " for player " << currentPlayerConn << std::endl;
            packet.currentPlayerTileId = it->second; // corrige la tuile
        }
    }
    
    std::cout << "[GAME] [BROADCAST] Player: " << currentPlayerConn << " (color " << packet.currentTurnColorId << "), Tile: " << packet.currentPlayerTileId << std::endl;
    for (int i = 0; i < 9; ++i) {
        packet.exchangeCoupons[i] = 0;
        packet.pendingStoneBonus[i] = false;
        packet.pendingRobberyBonus[i] = false;
    }
    for (int conn : playerConnections) {
        int color = getPlayerColorId(conn);
        if (color >= 0 && color < 9) {
            packet.exchangeCoupons[color] = getExchangeCouponCount(conn);
            packet.pendingStoneBonus[color] = hasPendingStoneBonus(conn);
            packet.pendingRobberyBonus[color] = hasPendingRobberyBonus(conn);
        }
    }
    
    // calcule le nombre de tours complets (tous les joueurs ont joué une fois = 1 tour)
    // le nombre de tours complets est le minimum de tours joués par n'importe quel joueur
    int completedRounds = 0;
    if (!playerConnections.empty()) {
        bool first = true;
        for (int conn : playerConnections) {
            auto it = playerTurnsPlayed.find(conn);
            if (it != playerTurnsPlayed.end()) {
                if (first) {
                    completedRounds = it->second;
                    first = false;
                } else {
                    completedRounds = std::min(completedRounds, it->second); // trouve le minimum
                }
            }
        }
    }
    // les tours commencent à 1, donc on ajoute 1 au nombre de tours complets
    // limite à 9 maximum (quand tous les joueurs ont joué 9 tours)
    packet.turnCount = std::min(completedRounds + 1, 9); // définit le nombre de tours complets (1-9)
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
    int maxSquare = 0;
    int size = board.getSize();
    
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (!board.isPlayerCell(i, j, playerId)) {
                continue;
            }
            
            for (int squareSize = 1; squareSize <= size - std::max(i, j); ++squareSize) {
                bool isValidSquare = true;
                
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
                    break;
                }
            }
        }
    }
    
    return maxSquare;
}

bool Game::isGameOver() const {
    // la partie est terminée quand tous les joueurs ont joué 9 tours (9 tours complets)
    const int ROUNDS_PER_PLAYER = 9;
    
    if (playerConnections.empty()) {
        return false;
    }
    
    // vérifie que tous les joueurs ont joué au moins 9 tours
    for (int conn : playerConnections) {
        auto it = playerTurnsPlayed.find(conn);
        if (it == playerTurnsPlayed.end() || it->second < ROUNDS_PER_PLAYER) {
            return false; // au moins un joueur n'a pas encore joué 9 tours
        }
    }
    
    return true; // tous les joueurs ont joué 9 tours
}

bool Game::hasRemainingCoupons() const {
    for (const auto& entry : playerExchangeCoupons) {
        if (entry.second > 0) {
            return true;
        }
    }
    return false;
}

void Game::computeWinner() {
    int bestTerritory = -1;
    int bestPlayerColor = -1;
    int tieCount = 0;

    for (int conn : playerConnections) {
        int colorId = getPlayerColorId(conn);
        if (colorId == -1) {
            continue;
        }

        int territory = getPlayerTerritoryCount(colorId);
        territory += getExchangeCouponCount(conn);

        if (territory > bestTerritory) {
            bestTerritory = territory;
            bestPlayerColor = colorId;
            tieCount = 1;
        } else if (territory == bestTerritory && bestTerritory != -1) {
            tieCount++;
        }
    }

    winnerId = (tieCount == 1) ? bestPlayerColor : -1;
}

void Game::finalizeWinnerIfReady() {
    if (!awaitingFinalCoupons && hasRemainingCoupons()) {
        awaitingFinalCoupons = true;
    }

    if (awaitingFinalCoupons) {
        if (!hasRemainingCoupons()) {
            awaitingFinalCoupons = false;
            computeWinner();
        }
    }

    broadcastBoardUpdate();
}

