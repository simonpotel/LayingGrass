#include "GameState.hpp"
#include <SFML/Graphics.hpp>

namespace {
    const sf::Color PLAYERS_COLORS_ARRAY[9] = {
        sf::Color(34, 139, 34),   // Forest Green
        sf::Color(65, 105, 225),  // Royal Blue
        sf::Color(220, 20, 60),   // Crimson
        sf::Color(255, 140, 0),   // Dark Orange
        sf::Color(147, 112, 219), // Medium Purple
        sf::Color(0, 206, 209),   // Dark Turquoise
        sf::Color(255, 105, 180), // Hot Pink
        sf::Color(218, 165, 32),  // Goldenrod
        sf::Color(46, 139, 87)    // Sea Green
    };
}

const sf::Color* GameState::PLAYERS_COLORS = PLAYERS_COLORS_ARRAY;

GameState::GameState()
    : currentState(ClientState::WAITING_FOR_LOBBIES),
      selectedLobbyId(-1),
      currentLobbyId(-1),
      username(""),
      selectedColorId(-1),
      requestSent(false),
      board(20),
      currentTurnColorId(-1),
      turnCount(0),
      gameOver(false),
      winnerId(-1),
      currentPlayerTileId(-1),
      tileRotation(0),
      tileFlippedH(false),
      tileFlippedV(false),
      gameEndWinnerId(-1),
      gameEndLobbyId(-1),
      exchangeCouponCount(0),
      pendingStoneBonus(false),
      pendingRobberyBonus(false),
      previewRow(-1),
      previewCol(-1),
      previewRotation(0),
      previewFlippedH(false),
      previewFlippedV(false),
      previewColorId(-1) {
    upcomingTiles.fill(-1);
}

GameState::~GameState() {
}

void GameState::updateLobbies(const LobbyListPacket& packet) {
    std::lock_guard<std::mutex> lock(lobbiesMutex); 
    
    lobbies.clear();

    for (int i = 0; i < packet.lobbyCount; ++i) {
        lobbies.push_back(packet.lobbies[i]);
    }

    if (currentState == ClientState::WAITING_FOR_LOBBIES && !lobbies.empty()) {
        currentState = ClientState::SELECTING_LOBBY;
    } else if (currentState == ClientState::GAME_END && !lobbies.empty()) {
        currentState = ClientState::SELECTING_LOBBY;
    }
}

void GameState::updateBoard(const BoardUpdatePacket& packet) {
    // si la taille a changé, on recrée le board
    if (board.getSize() != packet.size) {
        board = Board(packet.size);
    }
    
    // met à jour les cellules du board depuis le packet
    for (int i = 0; i < packet.size; ++i) {
        for (int j = 0; j < packet.size; ++j) {
            int value = packet.grid[i * 30 + j];
            board.setCellValue(i, j, value);
        }
    }
    
    // réinitialise la transformation si la tuile change
    int oldTileId = currentPlayerTileId;
    int oldTurnColorId = currentTurnColorId;
    currentTurnColorId = packet.currentTurnColorId;
    turnCount = packet.turnCount;
    gameOver = packet.gameOver;
    winnerId = packet.winnerId;
    currentPlayerTileId = packet.currentPlayerTileId;
    if (selectedColorId >= 0 && selectedColorId < 9) {
        exchangeCouponCount = packet.exchangeCoupons[selectedColorId];
        pendingStoneBonus = packet.pendingStoneBonus[selectedColorId];
        pendingRobberyBonus = packet.pendingRobberyBonus[selectedColorId];
    } else {
        exchangeCouponCount = 0;
        pendingStoneBonus = false;
        pendingRobberyBonus = false;
    }

    for (int i = 0; i < 5; ++i) {
        upcomingTiles[i] = packet.upcomingTiles[i];
    }
    
    if (oldTileId != currentPlayerTileId) {
        resetTileTransform();
    }
    
    // réinitialise la prévisualisation si le tour change
    if (oldTurnColorId != currentTurnColorId) {
        previewRow = -1;
        previewCol = -1;
        previewColorId = -1;
    }
}

void GameState::resetTileTransform() {
    tileRotation = 0;
    tileFlippedH = false;
    tileFlippedV = false;
}

void GameState::resetGameData() {
    board = Board(20);
    board.reset();
    currentTurnColorId = -1;
    turnCount = 0;
    gameOver = false;
    winnerId = -1;
    currentPlayerTileId = -1;
    resetTileTransform();
    exchangeCouponCount = 0;
    pendingStoneBonus = false;
    pendingRobberyBonus = false;
    upcomingTiles.fill(-1);
}

std::vector<LobbyInfo> GameState::getLobbies() const {
    std::lock_guard<std::mutex> lock(lobbiesMutex);
    return lobbies;
}

std::string GameState::getPlayerNameByColorId(int lobbyId, int colorId) const {
    std::lock_guard<std::mutex> lock(lobbiesMutex);
    
    for (const auto& lobby : lobbies) {
        if (lobby.lobbyId == lobbyId) {
            for (int i = 0; i < lobby.playerCount; ++i) {
                if (lobby.players[i].colorId == colorId) {
                    return std::string(lobby.players[i].playerName);
                }
            }
            break;
        }
    }
    
    return ""; // joueur non trouvé
}

void GameState::updateTilePreview(const TilePreviewPacket& packet) {
    previewRow = packet.row;
    previewCol = packet.col;
    previewRotation = packet.rotation;
    previewFlippedH = packet.flippedH;
    previewFlippedV = packet.flippedV;
    previewColorId = packet.colorId;
}
