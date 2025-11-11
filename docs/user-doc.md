# User Documentation

## Table of Contents

1. [Run the Game](#run-the-game)

## Run the Game

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
