# MapLoader Library

![C++](https://img.shields.io/badge/C++-11%20or%20later-blue.svg)

A comprehensive C++ library for loading, validating, and managing JSON battle map files for real-time strategy (RTS) games, pathfinding applications, and tactical simulations.

## 🚀 Features

### Core Functionality

- **JSON Battle Map Loading**: Parse and load complex battle map data from JSON files
- **Comprehensive Validation**: Robust validation of map structure, dimensions, and data integrity
- **Multiple Data Access Methods**: Flexible APIs for accessing map data in various formats
- **ASCII Visualization**: Terminal-based battle map rendering with Unicode characters
- **Error Handling**: Detailed error reporting and graceful failure handling
- **Memory Efficient**: Optimized data structures for large battle maps

### Battle Map Support

- **Multi-Layer Maps**: Support for complex maps with multiple terrain layers
- **Tileset Management**: Handle multiple tilesets with different tile dimensions
- **Terrain Types**: Built-in support for various terrain types (reachable, elevated, start/target positions)
- **Flexible Canvas**: Support for arbitrary canvas dimensions and tile sizes

### Integration Features

- **Pathfinding Ready**: Designed to integrate seamlessly with pathfinding algorithms
- **Game Engine Compatible**: Easy integration with game engines and simulation frameworks
- **Cross-Platform**: Compatible with Linux, Windows, and macOS
- **Well-Documented**: Comprehensive Doxygen documentation included

## 🔧 Installation

### Prerequisites

- **C++ Compiler**: GCC 4.8+ or Clang 3.4+ with C++11 support
- **jsoncpp Library**: For JSON parsing functionality

### Ubuntu/Debian Installation

```bash
# Install dependencies
sudo apt update
sudo apt install -y libjsoncpp-dev build-essential cmake

# Build using provided Makefile (from parent directory)
make maploader
```

## ⚡ Quick Start

### Basic Usage

```cpp
#include "MapLoader/MapLoader.h"
#include <iostream>

int main() {
    // Create MapLoader instance
    MapLoader loader;

    // Load battle map from JSON file
    if (loader.loadFromFile("battle_map.json")) {
        std::cout << "Map loaded successfully!" << std::endl;

        // Display map information
        loader.displayMapInfo();

        // Show ASCII visualization
        loader.displayMap(0);  // Display first layer

        // Access specific tile data
        int tile = loader.getTileAt(0, 5, 10);  // Layer 0, position (5,10)
        if (tile != MapLoader::INVALID_TILE) {
            std::cout << "Tile at (5,10): " << tile << std::endl;
        }

        // Get map dimensions
        std::cout << "Map size: " << loader.getMapWidth()
                  << "x" << loader.getMapHeight() << " tiles" << std::endl;
    } else {
        std::cerr << "Failed to load battle map!" << std::endl;
        return 1;
    }

    return 0;
}
```

### Compilation

```bash
# Using provided Makefile (from parent directory)
make maploader

# Manual compilation
g++ -std=c++11 -Wall -O2 -IMapLoader map_loader_demo.cpp MapLoader/MapLoader.cpp -ljsoncpp -o maploader

# Run your program
./maploader battle_map.json
```

## 📄 JSON Format Specification

The MapLoader expects JSON files with the following structure:

### Complete Example

```json
{
  "canvas": {
    "width": 1024,
    "height": 1024
  },
  "tilesets": [
    {
      "name": "terrain_tileset.png",
      "image": "assets/terrain_tileset.png",
      "imagewidth": 512,
      "imageheight": 512,
      "tilewidth": 32,
      "tileheight": 32
    }
  ],
  "layers": [
    {
      "name": "battle_terrain",
      "tileset": "terrain_tileset.png",
      "data": [
        -1, -1, -1, 3, 3, -1, -1, -1, -1, 0, -1, -1, -1, -1, 8, -1, -1, -1, -1,
        3, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
      ]
    }
  ]
}
```

### Field Descriptions

#### Canvas Object

- **width** (integer): Total canvas width in pixels
- **height** (integer): Total canvas height in pixels

#### Tileset Objects

- **name** (string): Unique identifier for the tileset
- **image** (string): Path to the tileset image file
- **imagewidth** (integer): Width of the tileset image in pixels
- **imageheight** (integer): Height of the tileset image in pixels
- **tilewidth** (integer): Width of individual tiles in pixels
- **tileheight** (integer): Height of individual tiles in pixels

#### Layer Objects

- **name** (string): Descriptive name for the layer
- **tileset** (string): Reference to tileset name
- **data** (array): Battle map data in row-major order

### Data Array Format

The `data` array contains integers representing different terrain types:

- Layer dimensions are calculated as: `canvas_size / tile_size`
- Data is stored in row-major order: `index = y * width + x`
- Array size must exactly match calculated dimensions

## 🗺️ Terrain Types

<!-- markdownlint-disable MD038 -->

### Standard Terrain Values

| Value  | Symbol  | Description       | Usage                              |
| ------ | ------- | ----------------- | ---------------------------------- |
| `-1`   | `░░`    | Reachable terrain | Units can move through these tiles |
| `0`    | `S `    | Starting position | Initial unit placement             |
| `8`    | `T `    | Target position   | Objective or destination           |
| `3`    | `▲▲`    | Elevated terrain  | Obstacles or impassable areas      |
| Others | Numbers | Custom terrain    | User-defined terrain types         |

### ASCII Visualization

When using `displayMap()`, terrain is rendered as:

```plaintext
░░░░▲▲▲▲░░░░
░░S ░░░░T ░░
░░░░▲▲▲▲░░░░
░░░░░░░░░░░░
```

- **`░░`**: Reachable areas (Unicode U+2591)
- **`S `**: Starting positions
- **`T `**: Target positions
- **`▲▲`**: Elevated terrain/obstacles (Unicode U+25B2)

## 📖 API Documentation

### Core Classes

#### MapLoader Class

The main class providing all map loading functionality.

```cpp
class MapLoader {
public:
    // Constants
    static const int INVALID_TILE = INT_MIN;

    // Constructors
    MapLoader();

    // Loading Methods
    bool loadFromFile(const std::string& filename);
    bool loadFromString(const std::string& jsonString);
    bool isMapLoaded() const;

    // Data Access
    const std::vector<Layer>& getLayers() const;
    const std::vector<Tileset>& getTilesets() const;
    const Canvas& getCanvas() const;

    // Utility Methods
    int getTileAt(int layerIndex, int x, int y) const;
    int getMapWidth() const;
    int getMapHeight() const;

    // Display Methods
    void displayMap(int layerIndex = 0) const;
    void displayMapInfo() const;

    // Management
    void clear();
};
```

### Supporting Structures

#### Tileset Structure

```cpp
struct Tileset {
    std::string name;        // Tileset identifier
    std::string image;       // Image file path
    int imageWidth;          // Image width in pixels
    int imageHeight;         // Image height in pixels
    int tileWidth;           // Individual tile width
    int tileHeight;          // Individual tile height
};
```

#### Layer Structure

```cpp
struct Layer {
    std::string name;           // Layer name
    std::string tileset;        // Associated tileset name
    std::vector<int> data;      // Battle map data
    int width;                  // Layer width in tiles
    int height;                 // Layer height in tiles
};
```

#### Canvas Structure

```cpp
struct Canvas {
    int width;   // Canvas width in pixels
    int height;  // Canvas height in pixels
};
```

## 💡 Usage Examples

### Example 1: Basic Map Loading and Analysis

```cpp
#include "MapLoader/MapLoader.h"

void analyzeMap(const std::string& filename) {
    MapLoader loader;

    if (!loader.loadFromFile(filename)) {
        std::cerr << "Failed to load: " << filename << std::endl;
        return;
    }

    // Get map dimensions
    int width = loader.getMapWidth();
    int height = loader.getMapHeight();

    std::cout << "Map Analysis for: " << filename << std::endl;
    std::cout << "Dimensions: " << width << "x" << height << std::endl;

    // Analyze terrain distribution
    const std::vector<Layer>& layers = loader.getLayers();
    if (!layers.empty()) {
        const Layer& mainLayer = layers[0];

        int reachable = 0, obstacles = 0, starts = 0, targets = 0;
        for (int tile : mainLayer.data) {
            switch (tile) {
                case -1: reachable++; break;
                case 0:  starts++; break;
                case 3:  obstacles++; break;
                case 8:  targets++; break;
            }
        }

        int total = width * height;
        std::cout << "Terrain Distribution:" << std::endl;
        std::cout << "  Reachable: " << reachable << " ("
                  << (100.0 * reachable / total) << "%)" << std::endl;
        std::cout << "  Obstacles: " << obstacles << " ("
                  << (100.0 * obstacles / total) << "%)" << std::endl;
        std::cout << "  Start positions: " << starts << std::endl;
        std::cout << "  Target positions: " << targets << std::endl;
    }
}
```

### Example 2: Integration with Pathfinding

```cpp
#include "MapLoader/MapLoader.h"

class PathfindingIntegration {
private:
    MapLoader mapLoader;
    std::vector<std::vector<bool>> walkableGrid;

public:
    bool loadMap(const std::string& filename) {
        if (!mapLoader.loadFromFile(filename)) {
            return false;
        }

        // Convert to pathfinding-friendly format
        int width = mapLoader.getMapWidth();
        int height = mapLoader.getMapHeight();

        walkableGrid.resize(height, std::vector<bool>(width, false));

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int tile = mapLoader.getTileAt(0, x, y);
                // Mark reachable tiles as walkable
                walkableGrid[y][x] = (tile == -1 || tile == 0 || tile == 8);
            }
        }

        return true;
    }

    bool isWalkable(int x, int y) const {
        if (x < 0 || x >= walkableGrid[0].size() ||
            y < 0 || y >= walkableGrid.size()) {
            return false;
        }
        return walkableGrid[y][x];
    }

    std::vector<std::pair<int, int>> getStartPositions() const {
        std::vector<std::pair<int, int>> starts;
        int width = mapLoader.getMapWidth();
        int height = mapLoader.getMapHeight();

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (mapLoader.getTileAt(0, x, y) == 0) {
                    starts.emplace_back(x, y);
                }
            }
        }

        return starts;
    }
};
```

### Example 3: Custom Terrain Processing

```cpp
#include "MapLoader/MapLoader.h"

class TerrainProcessor {
public:
    enum TerrainType {
        IMPASSABLE = -999,
        GROUND = -1,
        START = 0,
        ELEVATED = 3,
        TARGET = 8,
        WATER = 100,
        FOREST = 101,
        DESERT = 102
    };

    static std::string getTerrainName(int type) {
        switch (type) {
            case GROUND: return "Ground";
            case START: return "Starting Position";
            case ELEVATED: return "Elevated Terrain";
            case TARGET: return "Target Position";
            case WATER: return "Water";
            case FOREST: return "Forest";
            case DESERT: return "Desert";
            default: return "Unknown Terrain (" + std::to_string(type) + ")";
        }
    }

    static bool isPassable(int terrainType) {
        return terrainType == GROUND || terrainType == START ||
               terrainType == TARGET || terrainType == DESERT;
    }

    static int getMovementCost(int terrainType) {
        switch (terrainType) {
            case GROUND:
            case START:
            case TARGET: return 1;
            case FOREST: return 2;
            case DESERT: return 3;
            case WATER: return 5;
            case ELEVATED: return 999; // Very high cost
            default: return 1;
        }
    }
};

void processCustomTerrain(const std::string& filename) {
    MapLoader loader;
    if (!loader.loadFromFile(filename)) return;

    int width = loader.getMapWidth();
    int height = loader.getMapHeight();

    std::cout << "Custom Terrain Analysis:" << std::endl;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int tile = loader.getTileAt(0, x, y);

            std::cout << "(" << x << "," << y << "): "
                      << TerrainProcessor::getTerrainName(tile);

            if (TerrainProcessor::isPassable(tile)) {
                std::cout << " [Passable, Cost: "
                          << TerrainProcessor::getMovementCost(tile) << "]";
            } else {
                std::cout << " [Impassable]";
            }
            std::cout << std::endl;
        }
    }
}
```

## 🔧 Build Instructions

### Using Makefile (Recommended)

The library includes a comprehensive Makefile with multiple targets:

```bash
# Build everything (from parent directory)
make all

# Build only the map loader library
make maploader

# Clean build artifacts
make clean

# Install dependencies (Ubuntu/Debian)
make install-deps

# Run with a specific map file
make run-maploader FILE=your_map.json

# Show help
make help
```

## 🔍 Troubleshooting

### Common Issues and Solutions

#### 1. Compilation Errors

**Problem**: `jsoncpp/json/json.h: No such file or directory`

**Solution**:

```bash
# Ubuntu/Debian
sudo apt install libjsoncpp-dev

# Check installation
pkg-config --exists jsoncpp && echo "jsoncpp found" || echo "jsoncpp not found"
```

**Problem**: `undefined reference to Json::Value::Value()`

**Solution**: Make sure to link with `-ljsoncpp`:

```bash
g++ your_files.cpp -ljsoncpp -o your_program
```

#### 2. Runtime Errors

**Problem**: "Error parsing JSON: ..."

**Solutions**:

- Validate JSON syntax using an online JSON validator
- Check that all required fields are present
- Ensure numeric values are not quoted strings
- Verify file permissions and accessibility

**Problem**: "Layer data size doesn't match calculated dimensions"

**Solutions**:

- Check that `canvas_width / tile_width` and `canvas_height / tile_height` give integer results
- Verify data array size equals `width × height`
- Ensure canvas and tileset dimensions are consistent

**Problem**: Map displays incorrectly in terminal

**Solutions**:

- Ensure terminal supports Unicode (UTF-8)
- Try a different terminal emulator
- Check font support for Unicode characters (░, ▲)

#### 3. Integration Issues

**Problem**: Performance issues with large maps

**Solutions**:

- Use `getTileAt()` for selective access instead of copying entire layers
- Consider caching frequently accessed data
- Process maps in chunks for very large datasets

**Problem**: Memory usage concerns

**Solutions**:

- Use `clear()` method when switching between maps
- Consider loading only necessary layers
- Implement custom data structures for specific use cases

### Debugging Tips

1. **Enable Debug Output**: Compile with `-DDEBUG` for additional logging
2. **Validate JSON First**: Use online tools to validate JSON syntax before loading
3. **Check Map Dimensions**: Ensure canvas size is evenly divisible by tile size
4. **Test with Simple Maps**: Start with small, simple maps before complex ones

---
