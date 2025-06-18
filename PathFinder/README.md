# PathFinder Library - Advanced Tactical Battle Map Navigation

![C++](https://img.shields.io/badge/C++-11%20or%20later-blue.svg)

A comprehensive C++ pathfinding library designed for tactical battle map navigation featuring multiple algorithms, configurable movement orders, and optimized performance for real-time strategy applications.

## 🎯 Overview

The **PathFinder Library** provides a complete solution for tactical pathfinding in battle scenarios, offering multiple algorithms with configurable movement direction priorities. It serves as the foundation for both single-unit navigation and coordination systems.

## ✨ Key Features

### 🧭 **Advanced Pathfinding Algorithms**

- **A\* Algorithm**: Optimal pathfinding with Manhattan distance heuristic
- **Breadth-First Search (BFS)**: Guaranteed shortest path discovery
- **Depth-First Search (DFS)**: Exploratory pathfinding with comprehensive coverage
- **Configurable Movement Orders**: Customize unit movement direction priorities

### ⚙️ **Flexible Configuration**

- **Movement Direction Orders**: Control exploration priority (e.g., "rdlu", "uldr")
- **Custom Heuristics**: Manhattan distance optimized for grid-based movement
- **Battle Map Integration**: Seamless integration with MapLoader system
- **Memory Efficient**: Optimized data structures for large battle maps

### 🎮 **Game Engine Ready**

- **Real-Time Performance**: Optimized algorithms for responsive gameplay
- **Multiple Output Formats**: Flexible path representation for different use cases
- **Extensive Validation**: Comprehensive path and map validation
- **Cross-Platform**: Compatible with Linux, Windows, and macOS

## 🔧 Installation

### Prerequisites

- **C++ Compiler**: GCC 4.8+ or Clang 3.4+ with C++11 support
- **Standard Libraries**: STL containers, algorithms, and memory management
- **Optional**: jsoncpp for MapLoader integration

### Building

```bash
# Using provided Makefile (from parent directory)
make pathfinder

# Manual compilation
g++ -std=c++11 -Wall -O2 -IPathFinder -IMapLoader \
    main.cpp PathFinder/PathFinder.cpp MapLoader/MapLoader.cpp \
    -ljsoncpp -o pathfinder
```

## ⚡ Quick Start

### Basic Single-Unit Pathfinding

```cpp
#include "PathFinder/PathFinder.h"

int main() {
    // Initialize with custom movement order
    PathFinder pathfinder("uldr");  // Up-Left-Down-Right priority

    // Load battle map
    std::vector<std::vector<int>> grid = {
        {-1, -1, -1, -1, -1},
        {-1,  3,  3, -1, -1},
        { 0, -1,  3, -1,  8},
        {-1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1}
    };
    pathfinder.loadMapFromGrid(grid);

    // Find optimal path using A*
    std::vector<Position> path = pathfinder.findPathAStar();

    // Display results
    if (!path.empty()) {
        PathFinder::displayPath(path);
        pathfinder.getBattleMap().displayMapWithPath(path);

        std::cout << "Path length: " << PathFinder::calculatePathLength(path)
                  << " units" << std::endl;
    } else {
        std::cout << "No path found!" << std::endl;
    }

    return 0;
}
```

### Running Algorithm Comparison

```cpp
// Compare all three algorithms
PathFinder pathfinder("rdlu");
pathfinder.loadMapFromGrid(battleGrid);

// Test each algorithm
auto astarPath = pathfinder.findPathAStar();
auto bfsPath = pathfinder.findPathBFS();
auto dfsPath = pathfinder.findPathDFS();

// Compare results
std::cout << "A* path length: " << PathFinder::calculatePathLength(astarPath) << std::endl;
std::cout << "BFS path length: " << PathFinder::calculatePathLength(bfsPath) << std::endl;
std::cout << "DFS path length: " << PathFinder::calculatePathLength(dfsPath) << std::endl;
```

## 🧠 Algorithm Overview

### Algorithm Comparison

| Algorithm | Time Complexity | Space Complexity | Optimality  | Best Use Case                     |
| --------- | --------------- | ---------------- | ----------- | --------------------------------- |
| **A\***   | O(b^d)          | O(b^d)           | Optimal     | General purpose, optimal paths    |
| **BFS**   | O(b^d)          | O(b^d)           | Optimal     | Unweighted graphs, shortest paths |
| **DFS**   | O(b^m)          | O(bm)            | Non-optimal | Exploration, memory-constrained   |

Where _b = branching factor, d = depth of solution, m = maximum depth_

### A\* Algorithm

**Strengths:**

- Optimal pathfinding with admissible heuristic
- Efficient for large maps with good heuristic guidance
- Balanced between optimality and performance

**Best For:**

- General-purpose pathfinding
- Real-time applications requiring optimal paths
- Maps with varying terrain costs

```cpp
// A* with custom start and target
std::vector<Position> path = pathfinder.findPathAStar(
    Position(0, 0),   // Start position
    Position(10, 10)  // Target position
);
```

### Breadth-First Search (BFS)

**Strengths:**

- Guarantees shortest path in unweighted graphs
- Explores systematically level by level
- Finds path if one exists

**Best For:**

- Unweighted pathfinding scenarios
- When shortest path is critically important
- Educational and comparison purposes

```cpp
// BFS pathfinding
std::vector<Position> shortestPath = pathfinder.findPathBFS();
```

### Depth-First Search (DFS)

**Strengths:**

- Memory efficient (O(bm) space complexity)
- Good for path exploration and analysis
- Can find very long paths efficiently

**Best For:**

- Memory-constrained environments
- Exploratory pathfinding
- Testing map connectivity

```cpp
// DFS exploration
std::vector<Position> exploratoryPath = pathfinder.findPathDFS();
```

## 🧭 Movement Orders

Movement orders control the priority of direction exploration during pathfinding, significantly affecting path characteristics and performance.

### Standard Movement Orders

| Order    | Direction Priority    | Characteristics      | Best Use Case                    |
| -------- | --------------------- | -------------------- | -------------------------------- |
| `"rdlu"` | Right->Down->Left->Up | Bottom-right bias    | Default balanced approach        |
| `"uldr"` | Up->Left->Down->Right | Top-left bias        | Aggressive/defensive positioning |
| `"ldru"` | Left->Down->Right->Up | Left-side preference | Flanking maneuvers               |
| `"dlur"` | Down->Left->Up->Right | Downward exploration | Retreat/fallback paths           |

### Movement Order Impact

```cpp
// Test different movement orders
std::vector<std::string> orders = {"rdlu", "uldr", "ldru", "dlur"};

for (const auto& order : orders) {
    PathFinder pathfinder(order);
    pathfinder.loadMapFromGrid(grid);

    auto path = pathfinder.findPathDFS();
    std::cout << "Order " << order << ": "
              << PathFinder::calculatePathLength(path) << " steps" << std::endl;
}
```

### Custom Movement Orders

```cpp
// Validate custom movement order
if (PathFinder::isValidMoveOrder("urdl")) {
    PathFinder pathfinder("urdl");  // Up->Right->Down->Left
    // Custom pathfinding logic
}
```

**Requirements for Valid Movement Orders:**

- Exactly 4 characters
- Each of 'r', 'd', 'l', 'u' appears exactly once
- Case insensitive

## 📖 API Documentation

### Core Classes

#### PathFinder Class

```cpp
class PathFinder {
public:
    // Constructors
    PathFinder();                                    // Default constructor (rdlu order)
    PathFinder(const std::string& moveOrder);       // Custom movement order

    // Map Loading
    bool loadMapFromGrid(const std::vector<std::vector<int>>& grid);
    bool loadMapFromData(const std::vector<int>& data, int width, int height);

    // Movement Order Configuration
    bool setMoveOrder(const std::string& moveOrder);
    std::string getMoveOrder() const;
    void printMoveOrder() const;

    // Pathfinding Algorithms
    std::vector<Position> findPathAStar();                                    // Default positions
    std::vector<Position> findPathBFS();                                     // Default positions
    std::vector<Position> findPathDFS();                                     // Default positions

    std::vector<Position> findPathAStar(const Position& start, const Position& target);  // Custom positions
    std::vector<Position> findPathBFS(const Position& start, const Position& target);   // Custom positions
    std::vector<Position> findPathDFS(const Position& start, const Position& target);   // Custom positions

    // Information and Validation
    bool isMapLoaded() const;
    const BattleMap& getBattleMap() const;
    void displayMapInfo() const;
    void validateMap() const;

    // Static Utilities
    static bool validatePath(const std::vector<Position>& path, const BattleMap& map);
    static void displayPath(const std::vector<Position>& path);
    static int calculatePathLength(const std::vector<Position>& path);
    static bool isValidMoveOrder(const std::string& moveOrder);
};
```

### Supporting Structures

#### Position Structure

```cpp
struct Position {
    int x, y;                                       // Coordinates

    Position();                                     // Default constructor
    Position(int x, int y);                         // Parameter constructor

    bool operator==(const Position& other) const;   // Equality comparison
    bool operator!=(const Position& other) const;   // Inequality comparison
    bool operator<(const Position& other) const;    // Ordering for containers
};
```

#### BattleMap Structure

```cpp
struct BattleMap {
    std::vector<std::vector<int>> grid;             // 2D terrain grid
    int width, height;                              // Map dimensions
    Position startPos, targetPos;                   // Primary positions
    std::vector<Position> allStartPositions;        // All start positions
    std::vector<Position> allTargetPositions;       // All target positions

    bool isReachable(int x, int y) const;          // Check if position is traversable
    bool isValidPosition(int x, int y) const;      // Check if position is in bounds
    void displayMap() const;                        // ASCII map display
    void displayMapWithPath(const std::vector<Position>& path) const;  // Path overlay
};
```

## 💡 Usage Examples

### Example 1: Algorithm Performance Comparison

```cpp
#include "PathFinder/PathFinder.h"
#include <chrono>

void compareAlgorithms(const std::vector<std::vector<int>>& grid) {
    PathFinder pathfinder("rdlu");
    pathfinder.loadMapFromGrid(grid);

    // Benchmark A*
    auto start = std::chrono::high_resolution_clock::now();
    auto astarPath = pathfinder.findPathAStar();
    auto end = std::chrono::high_resolution_clock::now();
    auto astarTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Benchmark BFS
    start = std::chrono::high_resolution_clock::now();
    auto bfsPath = pathfinder.findPathBFS();
    end = std::chrono::high_resolution_clock::now();
    auto bfsTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Benchmark DFS
    start = std::chrono::high_resolution_clock::now();
    auto dfsPath = pathfinder.findPathDFS();
    end = std::chrono::high_resolution_clock::now();
    auto dfsTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Display results
    std::cout << "Performance Comparison:" << std::endl;
    std::cout << "A*:  " << astarPath.size() << " steps, " << astarTime.count() << " μs" << std::endl;
    std::cout << "BFS: " << bfsPath.size() << " steps, " << bfsTime.count() << " μs" << std::endl;
    std::cout << "DFS: " << dfsPath.size() << " steps, " << dfsTime.count() << " μs" << std::endl;
}
```

### Example 2: Movement Order Analysis

```cpp
#include "PathFinder/PathFinder.h"

void analyzeMoveOrders(const std::vector<std::vector<int>>& grid) {
    std::vector<std::string> orders = {"rdlu", "uldr", "ldru", "dlur"};
    std::vector<std::string> descriptions = {
        "Right-Down-Left-Up (Default)",
        "Up-Left-Down-Right (Aggressive)",
        "Left-Down-Right-Up (Defensive)",
        "Down-Left-Up-Right (Retreat)"
    };

    std::cout << "Movement Order Analysis:" << std::endl;

    for (size_t i = 0; i < orders.size(); ++i) {
        PathFinder pathfinder(orders[i]);
        pathfinder.loadMapFromGrid(grid);

        auto path = pathfinder.findPathAStar();

        std::cout << descriptions[i] << ":" << std::endl;
        std::cout << "  Path length: " << path.size() << " steps" << std::endl;

        if (!path.empty()) {
            std::cout << "  Start: (" << path[0].x << "," << path[0].y << ")" << std::endl;
            std::cout << "  End: (" << path.back().x << "," << path.back().y << ")" << std::endl;

            // Analyze path direction tendencies
            int rightMoves = 0, leftMoves = 0, upMoves = 0, downMoves = 0;
            for (size_t j = 1; j < path.size(); ++j) {
                int dx = path[j].x - path[j-1].x;
                int dy = path[j].y - path[j-1].y;

                if (dx > 0) rightMoves++;
                else if (dx < 0) leftMoves++;
                if (dy > 0) downMoves++;
                else if (dy < 0) upMoves++;
            }

            std::cout << "  Moves - Right: " << rightMoves << ", Left: " << leftMoves
                      << ", Down: " << downMoves << ", Up: " << upMoves << std::endl;
        }
        std::cout << std::endl;
    }
}
```

### Example 3: Custom Pathfinding Integration

```cpp
#include "PathFinder/PathFinder.h"

class AdvancedPathfinder {
private:
    PathFinder pathfinder;
    std::vector<std::vector<Position>> pathCache;

public:
    AdvancedPathfinder(const std::string& moveOrder = "rdlu")
        : pathfinder(moveOrder) {}

    bool initialize(const std::vector<std::vector<int>>& grid) {
        return pathfinder.loadMapFromGrid(grid);
    }

    std::vector<Position> findOptimalPath(const Position& start, const Position& target) {
        // Try A* first
        auto path = pathfinder.findPathAStar(start, target);

        if (path.empty()) {
            // Fallback to BFS if A* fails
            path = pathfinder.findPathBFS(start, target);
        }

        return path;
    }

    std::vector<Position> findAlternatePath(const Position& start, const Position& target) {
        // Get primary path
        auto primaryPath = findOptimalPath(start, target);

        if (primaryPath.empty()) {
            return {};
        }

        // Try different movement orders for alternate paths
        std::vector<std::string> alternateOrders = {"uldr", "ldru", "dlur"};

        for (const auto& order : alternateOrders) {
            PathFinder altPathfinder(order);
            altPathfinder.loadMapFromGrid(pathfinder.getBattleMap().grid);

            auto altPath = altPathfinder.findPathAStar(start, target);

            // Return if significantly different from primary path
            if (!altPath.empty() &&
                std::abs(static_cast<int>(altPath.size()) - static_cast<int>(primaryPath.size())) > 2) {
                return altPath;
            }
        }

        return primaryPath; // Return primary if no good alternate found
    }

    void analyzePathOptions(const Position& start, const Position& target) {
        std::cout << "Path Analysis from (" << start.x << "," << start.y
                  << ") to (" << target.x << "," << target.y << "):" << std::endl;

        auto astarPath = pathfinder.findPathAStar(start, target);
        auto bfsPath = pathfinder.findPathBFS(start, target);

        std::cout << "A* path length: " << astarPath.size() << std::endl;
        std::cout << "BFS path length: " << bfsPath.size() << std::endl;

        if (astarPath == bfsPath) {
            std::cout << "A* and BFS found identical paths (optimal)" << std::endl;
        } else {
            std::cout << "A* and BFS found different paths" << std::endl;
        }
    }
};
```

## 📊 Performance Analysis

### Benchmarking Framework

```cpp
#include <chrono>
#include <vector>
#include <iostream>

class PathfindingBenchmark {
public:
    struct BenchmarkResult {
        std::string algorithm;
        std::string moveOrder;
        size_t pathLength;
        long long executionTime;  // microseconds
        bool pathFound;
    };

    static std::vector<BenchmarkResult> runBenchmarks(
        const std::vector<std::vector<int>>& grid,
        const std::vector<std::string>& algorithms = {"astar", "bfs", "dfs"},
        const std::vector<std::string>& moveOrders = {"rdlu", "uldr", "ldru", "dlur"}) {

        std::vector<BenchmarkResult> results;

        for (const auto& algorithm : algorithms) {
            for (const auto& order : moveOrders) {
                PathFinder pathfinder(order);
                pathfinder.loadMapFromGrid(grid);

                auto start = std::chrono::high_resolution_clock::now();
                std::vector<Position> path;

                if (algorithm == "astar") {
                    path = pathfinder.findPathAStar();
                } else if (algorithm == "bfs") {
                    path = pathfinder.findPathBFS();
                } else if (algorithm == "dfs") {
                    path = pathfinder.findPathDFS();
                }

                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                results.push_back({
                    algorithm,
                    order,
                    path.size(),
                    duration.count(),
                    !path.empty()
                });
            }
        }

        return results;
    }

    static void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
        std::cout << std::setw(10) << "Algorithm"
                  << std::setw(8) << "Order"
                  << std::setw(12) << "Path Length"
                  << std::setw(15) << "Time (μs)"
                  << std::setw(10) << "Found" << std::endl;
        std::cout << std::string(55, '-') << std::endl;

        for (const auto& result : results) {
            std::cout << std::setw(10) << result.algorithm
                      << std::setw(8) << result.moveOrder
                      << std::setw(12) << (result.pathFound ? std::to_string(result.pathLength) : "N/A")
                      << std::setw(15) << result.executionTime
                      << std::setw(10) << (result.pathFound ? "Yes" : "No") << std::endl;
        }
    }
};
```

### Performance Characteristics

#### Time Complexity Analysis

- **A\***: Depends heavily on heuristic quality and branching factor
- **BFS**: Guaranteed to explore minimum nodes for shortest path
- **DFS**: Can be very fast or very slow depending on path structure

#### Space Complexity Considerations

- **A\***: Maintains open and closed sets, memory usage grows with search space
- **BFS**: Queue-based, memory usage proportional to breadth
- **DFS**: Stack-based, memory usage proportional to maximum depth

#### Movement Order Impact on Performance

Different movement orders can significantly affect:

- **Path Shape**: Direction of path curvature
- **Exploration Pattern**: Order of node expansion
- **Performance**: Some orders may find paths faster for specific layouts

## 🔧 Integration Guide

### Integration with MapLoader

```cpp
#include "MapLoader/MapLoader.h"
#include "PathFinder/PathFinder.h"

bool loadAndPath(const std::string& filename) {
    // Load map using MapLoader
    MapLoader mapLoader;
    if (!mapLoader.loadFromFile(filename)) {
        return false;
    }

    // Extract layer data for pathfinding
    const std::vector<Layer>& layers = mapLoader.getLayers();
    if (layers.empty()) {
        return false;
    }

    const Layer& battleLayer = layers[0];

    // Convert to PathFinder format
    std::vector<std::vector<int>> grid(battleLayer.height,
                                     std::vector<int>(battleLayer.width));
    for (int y = 0; y < battleLayer.height; ++y) {
        for (int x = 0; x < battleLayer.width; ++x) {
            grid[y][x] = battleLayer.data[y * battleLayer.width + x];
        }
    }

    // Initialize pathfinder and find path
    PathFinder pathfinder("uldr");
    pathfinder.loadMapFromGrid(grid);

    auto path = pathfinder.findPathAStar();
    if (!path.empty()) {
        PathFinder::displayPath(path);
        return true;
    }

    return false;
}
```

### Integration with Game Engines

```cpp
// Example integration with a hypothetical game engine
class GamePathfinder {
private:
    PathFinder pathfinder;
    GameMap* gameMap;

public:
    bool initialize(GameMap* map, const std::string& moveOrder = "rdlu") {
        gameMap = map;

        // Convert game map to pathfinder format
        auto grid = convertGameMapToGrid(map);
        return pathfinder.loadMapFromGrid(grid);
    }

    std::vector<GamePosition> findGamePath(const GamePosition& start,
                                          const GamePosition& target) {
        // Convert game positions to pathfinder positions
        Position pfStart = convertToPathfinderPos(start);
        Position pfTarget = convertToPathfinderPos(target);

        // Find path
        auto pfPath = pathfinder.findPathAStar(pfStart, pfTarget);

        // Convert back to game positions
        std::vector<GamePosition> gamePath;
        for (const auto& pos : pfPath) {
            gamePath.push_back(convertToGamePos(pos));
        }

        return gamePath;
    }

private:
    std::vector<std::vector<int>> convertGameMapToGrid(GameMap* map) {
        // Implementation specific to your game engine
        // Convert terrain types to pathfinder format
    }

    Position convertToPathfinderPos(const GamePosition& gamePos) {
        // Convert coordinate systems if needed
        return Position(gamePos.x, gamePos.y);
    }

    GamePosition convertToGamePos(const Position& pfPos) {
        // Convert back to game coordinate system
        return GamePosition(pfPos.x, pfPos.y);
    }
};
```

## 🎯 Best Practices

### Choosing the Right Algorithm

1. **Use A\*** for:

   - General-purpose pathfinding
   - When you need optimal paths
   - Real-time applications with performance requirements

2. **Use BFS** for:

   - Unweighted pathfinding scenarios
   - When shortest path is critical
   - Educational purposes and algorithm comparison

3. **Use DFS** for:
   - Memory-constrained environments
   - Path exploration and connectivity testing
   - When any path is acceptable

### Movement Order Selection

1. **"rdlu" (Default)**: Balanced for most scenarios
2. **"uldr"**: Aggressive positioning, upward movement preference
3. **"ldru"**: Defensive positioning, leftward movement preference
4. **"dlur"**: Retreat scenarios, downward movement preference

### Performance Optimization

```cpp
// Pre-validate maps before pathfinding
if (!pathfinder.isMapLoaded()) {
    std::cerr << "Map not loaded properly" << std::endl;
    return;
}

// Cache pathfinder instances for repeated use
class PathfinderCache {
    std::map<std::string, std::unique_ptr<PathFinder>> cache;

public:
    PathFinder* getPathfinder(const std::string& moveOrder) {
        auto it = cache.find(moveOrder);
        if (it == cache.end()) {
            cache[moveOrder] = std::make_unique<PathFinder>(moveOrder);
        }
        return cache[moveOrder].get();
    }
};

// Validate paths after finding them
auto path = pathfinder.findPathAStar();
if (!path.empty() && !PathFinder::validatePath(path, pathfinder.getBattleMap())) {
    std::cerr << "Invalid path generated" << std::endl;
}
```

## 🔍 Troubleshooting

### Common Issues

#### No Path Found

```cpp
// Check map connectivity
pathfinder.validateMap();

// Verify start and target positions
const auto& battleMap = pathfinder.getBattleMap();
if (!battleMap.isReachable(startPos.x, startPos.y)) {
    std::cerr << "Start position is not reachable" << std::endl;
}
if (!battleMap.isReachable(targetPos.x, targetPos.y)) {
    std::cerr << "Target position is not reachable" << std::endl;
}

// Try alternative algorithms
auto astarPath = pathfinder.findPathAStar();
if (astarPath.empty()) {
    auto bfsPath = pathfinder.findPathBFS();  // Fallback
}
```

#### Performance Issues

```cpp
// For large maps, prefer A* over BFS
if (mapWidth * mapHeight > 10000) {
    auto path = pathfinder.findPathAStar();  // More efficient for large maps
}

// Consider different movement orders
if (currentPath.size() > expectedLength) {
    // Try different movement order
    pathfinder.setMoveOrder("uldr");
    auto alternatePath = pathfinder.findPathAStar();
}
```

#### Invalid Movement Orders

```cpp
// Always validate movement orders
std::string customOrder = getUserInput();
if (!PathFinder::isValidMoveOrder(customOrder)) {
    std::cerr << "Invalid movement order: " << customOrder << std::endl;
    customOrder = "rdlu";  // Fallback to default
}
```

### Debug Utilities

```cpp
// Enable detailed map information
pathfinder.displayMapInfo();

// Visualize path on map
if (!path.empty()) {
    pathfinder.getBattleMap().displayMapWithPath(path);
}

// Path validation and analysis
if (PathFinder::validatePath(path, pathfinder.getBattleMap())) {
    std::cout << "Path is valid" << std::endl;
    PathFinder::displayPath(path);
} else {
    std::cout << "Path validation failed" << std::endl;
}
```

---
