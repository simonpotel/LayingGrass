# LayingGrass

Laying Grass is a tile-placement game where players place grass tiles on a grid to expand their territory. The goal is to create the largest square territory possible. The game supports 2 to 9 players on a 20x20 or 30x30 grid.

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)

## Prerequisites

### Linux
```bash
sudo apt-get update
sudo apt-get install -y libpthread-stubs0-dev libgl1-mesa-dev libx11-dev libxrandr-dev libxcursor-dev libxi-dev libudev-dev libfreetype6-dev libglew-dev libjpeg-dev libsndfile1-dev libopenal-dev libflac-dev libvorbis-dev
```

### macOS
No additional dependencies required.

### Windows
No additional dependencies required.

## Build

```bash
mkdir build
cd build
cmake ..
make
```

On Windows:
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Run

```bash
./build/LayingGrassServer
./build/LayingGrassClient
```

On Windows:
```bash
build\Release\LayingGrassServer.exe
build\Release\LayingGrassClient.exe
```

## Tests

After building, run tests with:

```bash
cd build
ctest
```

Or run tests directly:
```bash
./build/LayingGrassTests
```

On Windows:
```bash
cd build
ctest -C Release
# or
build\Release\LayingGrassTests.exe
```

