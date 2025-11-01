# Technical Documentation

## Table of Contents

1. [Building from Source](#building-from-source)
2. [Tests](#tests)
3. [Network Architecture](#network-architecture)

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

## Tests
![GoogleTest](https://img.shields.io/badge/GoogleTest-4285F4?style=for-the-badge&logo=google&logoColor=white)
![GitHub Actions](https://img.shields.io/badge/GitHub_Actions-2088FF?style=for-the-badge&logo=github-actions&logoColor=white)

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

## Network Architecture

### Overview

The game uses TCP sockets for client-server communication. The architecture follows a classic client-server model where multiple clients connect to a single server instance.

### Client Connection

The `Client` class manages the client-side connection:

```cpp
bool connect(const char* serverIp, int port);
void disconnect();
int getSocketFd() const { return socketFd; }
bool isConnected() const { return connected; }
```

### Server Connection

The `Server` class manages server-side operations:

```cpp
void registerCallback(PacketType type, std::function<void(Player*, const void*, size_t)> callback);
void start();
void stop();
```

```cpp
void acceptConnections();
void handleClient(int clientSocket);
bool receivePacket(int socket, PacketHeader& header, void*& data);
```

### Packet Structure

Packets follow a simple binary protocol:

1. **PacketHeader**: Contains packet type and size
   - `PacketType type`: Enum indicating the packet type
   - `int size`: Size of the payload data

```cpp
struct PacketHeader {
    PacketType type;
    int size;
};
```

2. **Payload**: Variable-length data following the header

### Packet Transmission

#### Sending Packets

Packets are sent with network byte order conversion:

```cpp
bool sendPacket(int socketFd, PacketType type, const void* data, size_t size);
```

The sending process:
1. Convert packet type and size to network byte order
2. Send header (type + size)
3. Send payload data if size > 0

#### Receiving Packets

The server receives packets in dedicated client handler threads:

```cpp
bool receivePacket(int socket, PacketHeader& header, void*& data);
```

The receiving process:
1. Receive packet type (network byte order)
2. Receive packet size (network byte order)
3. Convert to host byte order
4. Allocate and receive payload data if size > 0

### Packet Callback System

The server uses a callback system to handle different packet types:

```cpp
void registerCallback(PacketType type, std::function<void(Player*, const void*, size_t)> callback);
```

When a packet is received, the server looks up the appropriate callback and executes it with the player, packet data, and size.

### Player Management

Each connected client is represented by a `Player` object:

```cpp
class Player {
public:
    char playerName[256];
    int playerId;
    int connection;

    Player(const char* name, int id, int conn);
    ~Player();
};
```

Players are stored in a vector and managed by unique pointers:

```cpp
std::vector<std::unique_ptr<Player>> players;
```

