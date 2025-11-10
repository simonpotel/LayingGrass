# Technical Documentation

## Table of Contents

1. [Building from Source](#building-from-source)
2. [Tests](#tests)
3. [Game Classes](#game-classes)
   - [Board](#board)
   - [Cell](#cell)
   - [Tile](#tile)
   - [PlacementRules](#placementrules)
   - [Game](#game)
4. [Client Classes](#client-classes)
   - [Client](#client)
   - [GameState](#gamestate)
   - [Render](#render)
5. [Server Classes](#server-classes)
   - [Server](#server)
   - [Lobby](#lobby)
   - [LobbyManager](#lobbymanager)
   - [Player](#player)
6. [Network Classes](#network-classes)
   - [Packet](#packet)
   - [PacketCallback](#packetcallback)
7. [Testing](#testing)

## Building from Source

![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![Ubuntu](https://img.shields.io/badge/Ubuntu-E95420?style=for-the-badge&logo=ubuntu&logoColor=white)
![macOS](https://img.shields.io/badge/mac%20os-000000?style=for-the-badge&logo=macos&logoColor=F0F0F0)
![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)

### Prerequisites

#### Linux
```bash
sudo apt-get update
sudo apt-get install -y libpthread-stubs0-dev libgl1-mesa-dev libx11-dev libxrandr-dev libxcursor-dev libxi-dev libudev-dev libfreetype6-dev libglew-dev libjpeg-dev libsndfile1-dev libopenal-dev libflac-dev libvorbis-dev
```

#### macOS
No additional dependencies required.

#### Windows
No additional dependencies required.

### Build Commands

#### Linux / macOS
```bash
mkdir build
cd build
cmake ..
make
```

#### Windows
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Running from Build

After building, run the game:

#### Linux / macOS
```bash
./build/LayingGrassServer
./build/LayingGrassClient
```

#### Windows
```bash
build\Release\LayingGrassServer.exe
build\Release\LayingGrassClient.exe
```

## Game Classes

### Board

Represents the game board as a 2D grid of `Cell` objects. Board size is 20x20 for 2-4 players, 30x30 for 5-9 players.

**Structure:**
```cpp
class Board {
    std::vector<std::vector<Cell>> grid;
    int size;  // Board dimension (size x size)
};
```

**Key Methods:**
- `getCell(row, col)` - Returns cell at position (returns empty cell if out of bounds)
- `setCell(row, col, cell)` - Sets cell at position
- `isEmpty(row, col)` - Checks if cell is empty
- `isPlayerCell(row, col, playerId)` - Checks if cell belongs to player
- `hasAdjacentPlayerCell(row, col, playerId)` - Checks 4 cardinal directions for player territory
- `touchesOtherPlayerTerritory(row, col, playerId)` - Validates no opponent contact
- `isBonus(row, col)` - Checks if cell contains bonus
- `isBonusCaptured(row, col, playerId)` - Checks if bonus is captured (4 directions occupied)

Cell types: `EMPTY`, `GRASS` (with player ID 0-8), `STONE`, `BONUS_EXCHANGE`, `BONUS_STONE`, `BONUS_ROBBERY`.

### Cell

Represents a single board cell with type and optional player ownership.

**Structure:**
```cpp
enum class CellType {
    EMPTY = -1,
    GRASS = 0,
    STONE = 99,
    BONUS_EXCHANGE = 100,
    BONUS_STONE = 101,
    BONUS_ROBBERY = 102
};

class Cell {
    CellType type;
    int playerId;  // -1 if no player, 0-8 for GRASS cells
};
```

**Key Methods:**
- `toInt()` - Serializes to network format (returns playerId for GRASS, CellType value otherwise)
- `fromInt(value)` - Deserializes from network format
- `isEmpty()`, `isGrass()`, `isStone()`, `isBonus()` - Type checks
- `getPlayerId()` - Returns owner player ID

Network serialization: `EMPTY` = -1, `GRASS` = playerId (0-8), `STONE` = 99, bonuses = 100-102.

### Tile

Represents a tile shape as a collection of block offsets from an anchor point.

**Structure:**
```cpp
enum class TileId {
    TILE_0, TILE_1, ..., TILE_95,
    TOTAL_TILES = 96
};

class Tile {
    std::vector<std::pair<int, int>> blocks;  // (row, col) offsets from anchor
};
```

**Key Methods:**
- `getTile(TileId)` - Static method to get predefined tile by ID
- `rotate90()` - Rotates tile 90° clockwise
- `flipHorizontal()`, `flipVertical()` - Flips tile
- `normalized()` - Ensures anchor is at (0,0)
- `getWidth()`, `getHeight()` - Calculates tile dimensions

Contains 96 predefined tile shapes. Tiles can be transformed (rotation, flips) before placement. First turn uses `TILE_0` (1x1 single block).

### PlacementRules

Namespace containing tile placement validation logic.

**Functions:**
```cpp
namespace PlacementRules {
    bool playerHasCells(const Board& board, int colorId);
    bool canPlaceTile(const Board& board, const Tile& tile, 
                     int anchorRow, int anchorCol, 
                     int colorId, bool isFirstTurn);
}
```

**Placement Rules:**
- First turn: Can place anywhere on board
- Subsequent turns: Must be adjacent to own territory (4 cardinal directions)
- Cannot touch opponent territory
- Cannot place on occupied cells (except bonuses, which are overwritten)
- All tile blocks must be within board bounds

### Game

Manages complete game logic, state, and progression.

**Key Data Structures:**
```cpp
class Game {
    Board board;
    std::vector<int> playerConnections;
    std::vector<int> turnOrder;  // Shuffled at game start
    std::unordered_map<int, int> playerTiles;  // connection -> tileId
    std::unordered_map<int, int> playerTurnsPlayed;
    std::unordered_map<int, int> playerExchangeCoupons;
    std::unordered_map<int, bool> playerPendingStoneBonus;
    std::unordered_map<int, bool> playerPendingRobberyBonus;
    std::vector<int> tileQueue;  // Pre-shuffled tile queue
};
```

**Key Methods:**
- `handleCellClick()` - Processes tile placement or coupon usage
- `placeStone()` - Places stone with stone bonus
- `robTile()` - Steals tile from opponent with robbery bonus
- `discardTile()` - Discards tile if placement impossible
- `nextTurn()` - Advances to next player, distributes tile
- `isGameOver()` - Checks if all players completed 9 rounds
- `computeWinner()` - Calculates winner (territory + coupons)

**Game Flow:**
- Initialization: Creates board, shuffles turn order, places bonuses, distributes first tile
- Turn progression: Player places tile → checks bonus capture → applies bonuses → next turn
- Tile distribution: First turn = TILE_0 (1x1), subsequent = from pre-shuffled queue
- Win condition: Game ends when all players complete 9 rounds
- Winner: Player with most territory cells + exchange coupons

## Client Classes

### Client

Manages TCP connection to server and packet communication.

**Structure:**
```cpp
class Client {
    int socketFd;
    bool connected;
    bool receiving;
    PacketCallbackManager<std::function<void(const void*, size_t)>> callbackManager;
};
```

**Key Methods:**
- `connect(serverIp, port)` - Establishes TCP connection (default: 127.0.0.1:5555)
- `sendConnectRequest()` - Joins lobby with name and color
- `sendCellClick()` - Places tile or uses exchange coupon
- `sendPlaceStone()`, `sendRobTile()`, `sendDiscardTile()` - Bonus actions
- `startReceiving()` - Starts dedicated thread for packet reception
- `receiveLoop()` - Continuously receives and dispatches packets

**Communication:**
- Sends all game actions to server
- Receives packets in background thread
- Uses callback system to handle different packet types
- Thread-safe packet handling

### GameState

Manages client-side game state and state machine.

**Structure:**
```cpp
enum class ClientState {
    WAITING_FOR_LOBBIES,
    SELECTING_LOBBY,
    ENTERING_USERNAME,
    WAITING_FOR_RESPONSE,
    IN_LOBBY,
    IN_GAME,
    GAME_END,
    VIEWING_TILES
};

class GameState {
    ClientState currentState;
    Board board;  // Synchronized copy from server
    std::vector<LobbyInfo> lobbies;
    mutable std::mutex lobbiesMutex;  // Thread safety
    // Player data: turn, tile, coupons, bonuses
};
```

**Key Methods:**
- `updateLobbies()` - Updates lobby list from server
- `updateBoard()` - Synchronizes board state from `BoardUpdatePacket`
- `setState()` - Transitions between client states
- `getBoard()` - Returns synchronized board copy

**State Transitions:**
- `WAITING_FOR_LOBBIES` → `SELECTING_LOBBY` (on lobby list received)
- `SELECTING_LOBBY` → `ENTERING_USERNAME` (user selects lobby)
- `ENTERING_USERNAME` → `WAITING_FOR_RESPONSE` (sends connection request)
- `WAITING_FOR_RESPONSE` → `IN_LOBBY` (connection accepted)
- `IN_LOBBY` → `IN_GAME` (game starts)
- `IN_GAME` → `GAME_END` (game ends)

Thread-safe: Mutex protects shared state between network thread and main rendering thread.

### Render

![SFML](https://img.shields.io/badge/SFML-8CC445?style=for-the-badge&logo=sfml&logoColor=white)

Abstract base class for screen renderers using SFML for graphics. Factory pattern creates appropriate renderer for current state.

**Structure:**
```cpp
class Render {
public:
    virtual void draw(sf::RenderWindow& window, GameState& gameState) = 0;
    virtual bool handleInput(sf::RenderWindow& window, GameState& gameState, sf::Event& event) = 0;
    
    static std::unique_ptr<Render> createScreen(ClientState state);
};
```

**Renderer Implementations:**
- `WaitingForLobbies` - Displays loading message
- `SelectingLobby` - Shows lobby list for selection
- `EnteringUsername` - Text input for player name
- `InLobby` - Lobby waiting screen with player list
- `InGame` - Main game screen with board and UI
- `GameEnd` - End screen with winner information
- `TileViewer` - Debug view of all tiles

**Rendering Flow:**
Main loop in `client.cpp`: poll events → `handleInput()` → `render()` → display. Each screen handles its own input and rendering logic using SFML.

## Server Classes

### Server

Main server class managing connections, lobbies, and game instances.

**Structure:**
```cpp
class Server {
    int serverSocket;
    int port;  // Default: 5555
    bool running;
    std::vector<std::unique_ptr<Player>> players;
    LobbyManager lobbyManager;
    PacketCallbackManager<std::function<void(Player*, const void*, size_t)>> callbackManager;
};
```

**Key Methods:**
- `start()` - Starts server threads (accept connections, game updates, lobby updates)
- `acceptConnections()` - Accepts new client connections
- `handleClient(socket)` - Spawns thread per client for packet handling
- `gameUpdateLoop()` - Periodic check for game endings
- `removePlayer(socket)` - Handles client disconnection

**Thread Architecture:**
- Main thread: Server initialization
- Connection thread: Accepts new clients
- Client handler threads: One per connected client (receives packets)
- Game update thread: Checks game states periodically
- Lobby update thread: Broadcasts lobby list every 2 seconds

### Lobby

Represents a game lobby containing up to 9 players.

**Structure:**
```cpp
class Lobby {
    int lobbyId;
    bool gameStarted;
    std::vector<int> connections;  // Socket descriptors
    std::unordered_map<int, std::string> playerNames;
    std::unordered_map<int, int> playerColors;  // 0-8
    std::unique_ptr<Game> game;
};
```

**Key Methods:**
- `addConnection(connection, name, colorId)` - Adds player (validates duplicates)
- `removeConnection(connection)` - Removes player
- `broadcast(type, data, size)` - Sends packet to all players
- `startGame()` - Creates `Game` instance
- `isFull()` - Checks if 9 players reached
- `canAcceptPlayers()` - Validates lobby can accept new players

**Validation:**
- Rejects duplicate player names
- Rejects duplicate colors
- Maximum 9 players
- Cannot add players if game started

### LobbyManager

Manages multiple lobbies and auto-creates available ones.

**Structure:**
```cpp
class LobbyManager {
    std::vector<std::unique_ptr<Lobby>> lobbies;
    int nextLobbyId;
    bool running;
    std::function<void(const LobbyListPacket&)> updateCallback;
};
```

**Key Methods:**
- `findOrCreateAvailableLobby()` - Finds or creates available lobby
- `findLobbyById(lobbyId)` - Locates lobby by ID
- `updateLobbies()` - Ensures at least one available lobby exists
- `getLobbyListPacket()` - Builds lobby list packet for clients
- `startPeriodicUpdates(callback)` - Starts 2-second update thread

**Auto-Creation:**
Ensures at least one lobby is always available. Creates new lobby when all existing lobbies are full or have started games.

### Player

Represents a connected client.

**Structure:**
```cpp
class Player {
    char playerName[256];
    int playerId;
    int connection;  // Socket descriptor
    int lobbyId;  // -1 if not assigned
};
```

Created when client connects, removed on disconnect. Stores connection info and lobby assignment.

## Network Classes

### Packet

Binary protocol over TCP sockets with header + payload structure.

**Packet Header:**
```cpp
struct PacketHeader {
    PacketType type;  // 4 bytes, network byte order
    int size;         // 4 bytes, network byte order
};
// Total: 8 bytes
```

**Packet Types:**
```cpp
enum class PacketType {
    CONNECT_REQUEST,      // Client → Server
    CONNECT_RESPONSE,     // Server → Client
    LOBBY_LIST,          // Server → Client
    GAME_START,          // Server → Client
    GAME_END,            // Server → Client
    BOARD_UPDATE,        // Server → Client
    CELL_CLICK,          // Client → Server
    START_GAME_REQUEST,  // Client → Server
    TILE_PREVIEW,        // Client ↔ Server
    PLACE_STONE,         // Client → Server
    ROB_TILE,            // Client → Server
    DISCARD_TILE         // Client → Server
};
```

**Transmission:**
- `Packet::sendPacket()` - Sends header + payload with network byte order conversion
- `Packet::receivePacket()` - Receives header, converts byte order, allocates and receives payload

### PacketCallback

Template class managing packet callbacks for routing.

**Structure:**
```cpp
template<typename CallbackType>
class PacketCallbackManager {
    std::unordered_map<PacketType, CallbackType> callbacks;
};
```

**Usage:**
- **Client**: `std::function<void(const void*, size_t)>` - Receives packet data
- **Server**: `std::function<void(Player*, const void*, size_t)>` - Receives player + packet data

**Methods:**
- `registerCallback(type, callback)` - Registers handler for packet type
- `getCallback(type)` - Retrieves callback (returns nullptr if not found)
- `unregisterCallback(type)` - Removes callback

Packets are routed to registered callbacks based on `PacketType` in header.

## Testing

![GoogleTest](https://img.shields.io/badge/GoogleTest-4285F4?style=for-the-badge&logo=google&logoColor=white)
![GitHub Actions](https://img.shields.io/badge/GitHub_Actions-2088FF?style=for-the-badge&logo=github-actions&logoColor=white)

Run tests to execute all test cases and verify coverage of the requirements from the specification document. 
After building, run tests with:

#### Linux / macOS
```bash
cd build
ctest
```

Or run tests directly:
```bash
./build/LayingGrassTests
```

#### Windows
```bash
cd build
ctest -C Release
```

Or run tests directly:
```bash
build\Release\LayingGrassTests.exe
```