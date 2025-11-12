# User Documentation

## Table of Contents

1. [Run the Game](#1-run-the-game)
2. [Game Overview](#2-game-overview)
3. [How to Play](#3-how-to-play)
   - [Connecting to a Server](#connecting-to-a-server)
   - [Joining a Lobby](#joining-a-lobby)
   - [Game Setup](#game-setup)
4. [Game Rules](#4-game-rules)
   - [Tile Placement](#tile-placement)
   - [Turn System](#turn-system)
   - [Board Size](#board-size)
5. [Game Mechanics](#5-game-mechanics)
   - [Tiles](#tiles)
   - [Exchange Tiles](#exchange-tiles)
   - [Bonuses](#bonuses)
   - [Stones](#stones)
6. [Controls](#6-controls)
   - [During Your Turn](#during-your-turn)
   - [Exchange Tile Selection](#exchange-tile-selection)
   - [Final Phase](#final-phase)
7. [Winning](#7-winning)

## 1. Run the Game

### Quick Start

Pre-built binaries are available in the [Releases](https://github.com/simonpotel/LayingGrass/releases) section of the repository.

#### Download and Run

1. Go to the [Releases page](https://github.com/simonpotel/LayingGrass/releases)
2. Download the binaries for your platform:
   - `LayingGrassClient-ubuntu` and `LayingGrassServer-ubuntu` for Linux
   - `LayingGrassClient-macos` and `LayingGrassServer-macos` for macOS
   - `LayingGrassClient-windows.exe` and `LayingGrassServer-windows.exe` for Windows

3. Make the binaries executable (Linux/macOS):
```bash
chmod +x LayingGrassClient-ubuntu LayingGrassServer-ubuntu
```

4. Run the game:

**Server:**
```bash
./LayingGrassServer-ubuntu          # Default port: 5555
./LayingGrassServer-ubuntu 8080     # Custom port: 8080
```

**Client:**
```bash
./LayingGrassClient-ubuntu                          # Default: 127.0.0.1:5555
./LayingGrassClient-ubuntu 192.168.1.100            # Custom IP, default port: 5555
./LayingGrassClient-ubuntu 192.168.1.100 8080       # Custom IP and port
```

On macOS:
```bash
./LayingGrassServer-macos          # Default port: 5555
./LayingGrassServer-macos 8080     # Custom port: 8080

./LayingGrassClient-macos                          # Default: 127.0.0.1:5555
./LayingGrassClient-macos 192.168.1.100            # Custom IP, default port: 5555
./LayingGrassClient-macos 192.168.1.100 8080       # Custom IP and port
```

On Windows:
```bash
LayingGrassServer-windows.exe          # Default port: 5555
LayingGrassServer-windows.exe 8080     # Custom port: 8080

LayingGrassClient-windows.exe                          # Default: 127.0.0.1:5555
LayingGrassClient-windows.exe 192.168.1.100            # Custom IP, default port: 5555
LayingGrassClient-windows.exe 192.168.1.100 8080      # Custom IP and port
```

## 2. Game Overview

Laying Grass is a strategic tile-placement game where players compete to expand their territory on a grid. The objective is to create the largest territory possible by placing grass tiles strategically. The game supports 2 to 9 players and features various bonuses and special mechanics that add depth to the gameplay.

**Key Features:**
- Turn-based tile placement strategy
- Multiple bonus types that provide strategic advantages
- Exchange tiles system for tactical tile management
- Territory expansion mechanics
- Competitive multiplayer gameplay

## 3. How to Play

### Connecting to a Server

1. **Start the server** on the host machine (default port: 5555)
2. **Launch the client** and connect to the server IP address
3. The client will automatically fetch the list of available lobbies

### Joining a Lobby

1. **Select a lobby** from the list displayed on screen
2. **Enter your username** when prompted
3. **Choose your color** from the available colors (each player must have a unique color)
4. Wait for the lobby to fill up (2-9 players required)
5. The game starts automatically when the lobby is full

### Game Setup

- **Board Size**: 
  - 20x20 grid for 2-4 players
  - 30x30 grid for 5-9 players
- **Turn Order**: Randomly shuffled at game start
- **Starting Tile**: All players receive a 1x1 tile (TILE_0) for their first turn
- **Exchange Tiles**: Each player starts with 1 exchange tile
- **Bonuses**: Randomly placed on the board before the game starts

## 4. Game Rules

### Tile Placement

**First Turn:**
- You can place your tile anywhere on the board
- Must be a valid position (all blocks within board bounds)

**Subsequent Turns:**
- Your tile must be **adjacent** to your existing territory
- Adjacency is checked in the 4 cardinal directions (North, South, East, West)
- You **cannot** place tiles that touch opponent territory
- You **cannot** place on occupied cells (except bonuses, which are overwritten)

**Tile Transformations:**
- Tiles can be rotated (0°, 90°, 180°, 270°)
- Tiles can be flipped horizontally and/or vertically
- Transformations can be combined before placement

### Turn System

- Each player takes turns placing tiles
- The game consists of **9 rounds** (each player plays 9 turns)
- After placing a tile, you receive a new random tile for your next turn
- If you capture a bonus, you must use it immediately before the turn ends
- If you cannot place your tile, you can discard it (skips your turn)

### Board Size

The board size is automatically determined based on the number of players:
- **2-4 players**: 20x20 grid (400 cells)
- **5-9 players**: 30x30 grid (900 cells)

## 5. Game Mechanics

### Tiles

- **96 different tile shapes** are available in the game
- Each tile consists of multiple connected blocks
- Tiles are distributed from a pre-shuffled queue
- First turn always uses TILE_0 (single 1x1 block)
- Subsequent tiles are randomly selected from the queue

**Tile Preview:**
- Hover your mouse over the board to see a preview of tile placement
- Green outline = valid placement
- Red outline = invalid placement

### Exchange Tiles

Exchange tiles allow you to:
1. **Exchange your current tile** for one from the upcoming tiles queue
2. **Remove a stone** from the board
3. **Place a 1x1 tile** on an empty cell

**How to Get Exchange Tiles:**
- Start with 1 exchange tile
- Capture Exchange Bonus cells to receive more

**Using Exchange Tiles:**
- **During the game**: Press **C** to select an exchange tile, then press **1-5** to choose which tile to exchange
- **Remove stone**: Press **X** while hovering over a stone
- **Place 1x1**: Press **C** or **X** while hovering over an empty cell (during final phase)

**Final Phase:**
- After all 9 rounds are completed, players can use remaining exchange tiles
- Press **C** to place a 1x1 tile or remove a stone
- Press **X** to remove a stone
- Press **N** to skip using an exchange tile

### Bonuses

Three types of bonuses are placed on the board:

#### Exchange Bonus
- **Appearance**: Green bonus cell
- **Capture**: Place tiles on all 4 cardinal directions (N/S/E/W) around the bonus
- **Reward**: 1 exchange tile
- **Note**: If you place a tile that covers a bonus, the bonus is lost

#### Stone Bonus
- **Appearance**: Yellow bonus cell
- **Capture**: Place tiles on all 4 cardinal directions (N/S/E/W) around the bonus
- **Reward**: Place 1 stone immediately on any empty cell
- **Usage**: Must be used before your turn ends
- **Note**: Stones block tile placement for all players

#### Robbery Bonus
- **Appearance**: Orange bonus cell
- **Capture**: Place tiles on all 4 cardinal directions (N/S/E/W) around the bonus
- **Reward**: Steal 1 tile from an opponent
- **Usage**: Must be used before your turn ends
- **Target**: Click on any opponent's territory cell to steal their current tile

### Stones

- **Placing Stones**: Use Stone Bonus to place a stone on any empty cell
- **Removing Stones**: Use an exchange tile (press **X** while hovering over a stone)
- **Effect**: Stones block tile placement for all players
- **Strategy**: Use stones to block opponents or protect your territory

## 6. Controls

### During Your Turn

**Tile Manipulation:**
- **R**: Rotate tile 90° clockwise
- **F**: Flip tile horizontally
- **V**: Flip tile vertically

**Placement:**
- **Left Click**: Place tile at mouse position
- **Mouse Hover**: Preview tile placement (green = valid, red = invalid)

**Exchange Tiles:**
- **C**: Open exchange tile selection menu (if you have upcoming tiles available)
- **X**: Remove stone with exchange tile (hover over stone first)
- **1-5**: Select tile from exchange menu (1 = first upcoming tile, 5 = fifth)

**Other:**
- **Tab**: View all available tiles (debug mode)
- **Discard Button**: Click to discard tile if placement is impossible

### Exchange Tile Selection

When you press **C** and have upcoming tiles available:
1. Exchange tile menu appears showing 5 upcoming tiles
2. Press **1-5** to select which tile to exchange
3. Press **Esc** or **C** again to cancel

### Final Phase

After the game ends (all players completed 9 rounds):
- **C**: Place 1x1 tile or remove stone (hover over target cell)
- **X**: Remove stone (hover over stone)
- **N**: Skip using exchange tile
- **Left Click**: Place 1x1 tile or remove stone (click on target cell)

## 7. Winning

**Game End Condition:**
- The game ends when all players have completed 9 rounds
- Players can then use remaining exchange tiles
- Once all exchange tiles are used or skipped, the winner is determined

**Scoring:**
- **Territory**: Count of all cells you control
- **Exchange Tiles**: Each unused exchange tile adds 1 point
- **Final Score**: Territory cells + Exchange tiles

**Winner:**
- The player with the **highest score** wins
- In case of a tie, no winner is declared (winnerId = -1)

**Strategy Tips:**
- Expand your territory efficiently
- Capture bonuses for strategic advantages
- Use exchange tiles to remove opponent stones or get better tiles
- Block opponents with stones when possible
- Plan ahead for the largest possible territory
