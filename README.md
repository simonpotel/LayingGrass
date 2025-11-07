# LayingGrass

Laying Grass is a tile-placement game where players place grass tiles on a grid to expand their territory. The goal is to create the largest square territory possible. The game supports 2 to 9 players on a 20x20 or 30x30 grid.

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![SFML](https://img.shields.io/badge/SFML-8CC445?style=for-the-badge&logo=sfml&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)

![GoogleTest](https://img.shields.io/badge/GoogleTest-4285F4?style=for-the-badge&logo=google&logoColor=white)
![GitHub Actions](https://img.shields.io/badge/GitHub_Actions-2088FF?style=for-the-badge&logo=github-actions&logoColor=white)

## Preview

### Lobby Selection
![Selecting Lobby](docs/preview/SelectingLobby.png)

### Entering Username
![Entering Username](docs/preview/EnteringUsername.png)
![Entering Username 2](docs/preview/EnteringUsername2.png)

### In Lobby
![In Lobby](docs/preview/InLobby.png)

### In Game
![In Game](docs/preview/InGame.png)
![In Game 2](docs/preview/InGame2.png)

### Tile Viewer
![Tile Viewer](docs/preview/TileViewer.png)

## Quick Start

Pre-built binaries are available in the [Releases](https://github.com/simonpotel/LayingGrass/releases) section of the repository.

### Download and Run

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
```bash
./LayingGrassServer-ubuntu
./LayingGrassClient-ubuntu
```

On macOS:
```bash
./LayingGrassServer-macos
./LayingGrassClient-macos
```

On Windows:
```bash
LayingGrassServer-windows.exe
LayingGrassClient-windows.exe
```

## Documentation

- [User Documentation](docs/user-doc.md) - How to run and play the game
- [Technical Documentation](docs/technical-doc.md) - Build instructions and technical architecture

