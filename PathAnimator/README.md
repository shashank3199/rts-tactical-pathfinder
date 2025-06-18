# PathAnimator Library

![C++](https://img.shields.io/badge/C++-11%20or%20later-blue.svg)

A comprehensive C++ library for creating terminal-based animations of pathfinding algorithms. Provides rich visualization capabilities for both single-unit and multi-unit pathfinding scenarios with customizable animation styles, speeds, and interactive controls.

## 🎯 Overview

**PathAnimator** transforms static pathfinding results into dynamic, visual experiences within terminal environments. It supports various animation styles, real-time collision detection, and interactive controls, making it an invaluable tool for debugging, education, and demonstration of pathfinding algorithms.

## ✨ Key Features

### 🎬 **Animation Capabilities**

- **Single-Unit Animation**: Smooth path visualization with trail effects and step numbering
- **Multi-Unit Animation**: Coordinated movement visualization with collision detection
- **Real-Time Collision Visualization**: Instant identification and highlighting of unit conflicts
- **Step-by-Step Manual Control**: Frame-by-frame progression for detailed analysis
- **Progress Tracking**: Visual progress bars and completion indicators

### 🎨 **Visual Styles**

- **Simple Movement**: Basic unit movement without trails
- **Trail Animation**: Shows path history behind moving units
- **Numbered Steps**: Displays step numbers for path analysis
- **Path Highlighting**: Illuminates entire path during animation

**Note**: Styles are only available for single-unit animations. Multi-unit animations use enhanced visual elements like unit IDs, collision symbols, and target success indicators.

### ⚡ **Performance & Control**

- **Configurable Speeds**: From very slow (1000ms) to very fast (50ms) per step
- **Interactive Controls**: Pause, resume, and step-through capabilities
- **Terminal Optimization**: Efficient screen updates and cursor management
- **Graceful Interruption**: Proper cleanup on Ctrl+C or user quit

### 🤖 **Multi-Unit Intelligence**

- **Collision Detection**: Real-time identification of unit conflicts
- **Target Differentiation**: Distinguishes between collisions and successful arrivals
- **Partial Path Support**: Animates successful units even when some fail
- **Unit Identification**: Clear visual distinction between different units

## 🔧 Installation

### Prerequisites

- **C++ Compiler**: GCC 4.8+ or Clang 3.4+ with C++11 support
- **ANSI-Compatible Terminal**: For color and cursor control
- **PathFinder/MultiUnitPathFinder**: Core pathfinding libraries (included in project)
- **Standard Libraries**: STL containers, threading, and chrono

### Building

```bash
# Using provided Makefile (from parent directory)
make pathfinder  # Includes PathAnimator

# Manual compilation
g++ -std=c++11 -Wall -O2 -IPathFinder -IMultiUnitPathFinder -IPathAnimator \
    main.cpp PathFinder/PathFinder.cpp MultiUnitPathFinder/MultiUnitPathFinder.cpp \
    PathAnimator/PathAnimator.cpp -ljsoncpp -o pathfinder
```

## ⚡ Quick Start

### Basic Single-Unit Animation

```cpp
#include "PathAnimator/PathAnimator.h"
#include "PathFinder/PathFinder.h"

int main() {
    // Setup pathfinder and find path
    PathFinder pathfinder;
    pathfinder.loadMapFromGrid(battleGrid);
    std::vector<Position> path = pathfinder.findPathAStar();

    // Create animator with default settings
    PathAnimator animator;

    // Configure animation style
    AnimationConfig config;
    config.style = AnimationStyle::WITH_TRAIL;
    config.speed = AnimationSpeed::NORMAL;
    config.unitSymbol = 'U';
    config.showProgress = true;
    animator.setConfig(config);

    // Animate the path
    if (!path.empty()) {
        animator.animatePath(pathfinder.getBattleMap(), path);
    }

    return 0;
}
```

### Multi-Unit Animation

```cpp
#include "MultiUnitPathFinder/MultiUnitPathFinder.h"
#include "PathAnimator/PathAnimator.h"

int main() {
    // Setup multi-unit pathfinder
    MultiUnitPathFinder coordinator;
    coordinator.loadMapFromGrid(battleGrid);
    coordinator.autoSetupUnitsFromMap();

    // Find coordinated paths
    PathfindingResult result = coordinator.findPathsForAllUnits();

    // Configure multi-unit animation
    PathAnimator animator;
    MultiUnitAnimationConfig config;
    config.style = AnimationStyle::WITH_TRAIL;
    config.speed = AnimationSpeed::NORMAL;
    config.showUnitIds = true;
    config.highlightCollisions = true;
    config.showTrails = true;
    animator.setMultiUnitConfig(config);

    // Animate multi-unit paths
    if (result.allPathsFound) {
        animator.animateMultiUnitPaths(coordinator.getBattleMap(), result);
    } else {
        // Handle partial results
        animator.animatePartialMultiUnitPaths(coordinator.getBattleMap(), result);
    }

    return 0;
}
```

### Step-by-Step Analysis

```cpp
// Manual step-through for detailed analysis
PathAnimator animator;
if (!path.empty()) {
    // User controls each step with ENTER key
    animator.animatePathStepByStep(pathfinder.getBattleMap(), path);
}

// Multi-unit step-by-step
if (result.allPathsFound) {
    animator.animateMultiUnitPathsStepByStep(coordinator.getBattleMap(), result);
}
```

## 🎨 Animation Styles

### Single-Unit Styles

| Style            | Description       | Visual Effect                                  | Best Use Case        |
| ---------------- | ----------------- | ---------------------------------------------- | -------------------- |
| `SIMPLE`         | Basic movement    | Unit moves without leaving traces              | Quick demonstrations |
| `WITH_TRAIL`     | Trail behind unit | Shows `.` characters marking visited positions | Path analysis        |
| `NUMBERED_STEPS` | Step numbering    | Displays step numbers (0-9) on path positions  | Educational purposes |
| `HIGHLIGHT_PATH` | Path illumination | Highlights entire path with `#` characters     | Path overview        |

#### Single-Unit Visual Examples

```cpp
// Simple movement - just the unit moving
AnimationConfig simpleConfig;
simpleConfig.style = AnimationStyle::SIMPLE;
simpleConfig.unitSymbol = '@';

// Trail animation - shows path history
AnimationConfig trailConfig;
trailConfig.style = AnimationStyle::WITH_TRAIL;
trailConfig.trailSymbol = '.';
trailConfig.unitSymbol = 'U';

// Numbered steps for analysis
AnimationConfig numberedConfig;
numberedConfig.style = AnimationStyle::NUMBERED_STEPS;
numberedConfig.showStepNumbers = true;

// Full path highlighting
AnimationConfig highlightConfig;
highlightConfig.style = AnimationStyle::HIGHLIGHT_PATH;
highlightConfig.pathSymbol = '#';
```

### Multi-Unit Enhancements

Multi-unit animations include additional visual elements:

- **Unit IDs**: Each unit displays with unique symbols (1-9, A-F)
- **Collision Detection**: Conflicting positions shown with `X` + count
- **Target Success**: Multiple units at targets shown as `T` + count

```cpp
MultiUnitAnimationConfig multiConfig;
multiConfig.unitSymbols = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
multiConfig.collisionSymbol = 'X';
multiConfig.showUnitIds = true;
multiConfig.highlightCollisions = true;
```

## ⚙️ Configuration Options

### AnimationSpeed Enumeration

| Speed       | Duration    | Use Case                         |
| ----------- | ----------- | -------------------------------- |
| `VERY_SLOW` | 1000ms/step | Detailed analysis, presentations |
| `SLOW`      | 500ms/step  | Educational demonstrations       |
| `NORMAL`    | 250ms/step  | Standard viewing (default)       |
| `FAST`      | 100ms/step  | Quick overviews                  |
| `VERY_FAST` | 50ms/step   | Rapid demonstrations             |

### AnimationConfig (Single-Unit)

```cpp
struct AnimationConfig {
    AnimationStyle style;               // Visual animation style
    AnimationSpeed speed;               // Animation timing
    char unitSymbol;                    // Character for unit ('U')
    char trailSymbol;                   // Character for trails ('.')
    char pathSymbol;                    // Character for path highlight ('#')
    bool showStepNumbers;               // Display step numbers
    bool clearScreenBetweenFrames;      // Clear screen each frame
    bool showProgress;                  // Display progress bar
};
```

### MultiUnitAnimationConfig

```cpp
struct MultiUnitAnimationConfig {
    AnimationStyle style;               // Visual animation style
    AnimationSpeed speed;               // Animation timing
    std::vector<char> unitSymbols;      // Symbols for each unit
    char trailSymbol;                   // Character for trails ('.')
    char pathSymbol;                    // Character for paths ('#')
    char collisionSymbol;               // Character for collisions ('X')
    bool showUnitIds;                   // Show unit identifiers
    bool showTrails;                    // Display path trails
    bool showProgress;                  // Display progress bar
    bool clearScreenBetweenFrames;      // Clear screen each frame
    bool highlightCollisions;           // Emphasize conflicts
};
```

## 📖 API Documentation

### Core Classes

#### PathAnimator Class

```cpp
class PathAnimator {
public:
    // Constructors
    PathAnimator();
    PathAnimator(const AnimationConfig& config);

    // Configuration Methods
    void setAnimationStyle(AnimationStyle style);
    void setAnimationSpeed(AnimationSpeed speed);
    void setUnitSymbol(char symbol);
    void setTrailSymbol(char symbol);
    void setShowProgress(bool show);
    void setConfig(const AnimationConfig& newConfig);
    const AnimationConfig& getConfig() const;

    // Multi-Unit Configuration
    void setMultiUnitConfig(const MultiUnitAnimationConfig& newConfig);
    const MultiUnitAnimationConfig& getMultiUnitConfig() const;

    // Single-Unit Animation Methods
    bool animatePath(const BattleMap& battleMap, const std::vector<Position>& path);
    bool animatePathStepByStep(const BattleMap& battleMap, const std::vector<Position>& path);

    // Multi-Unit Animation Methods
    bool animateMultiUnitPaths(const BattleMap& battleMap, const PathfindingResult& result);
    bool animateMultiUnitPathsStepByStep(const BattleMap& battleMap, const PathfindingResult& result);
    bool animatePartialMultiUnitPaths(const BattleMap& battleMap, const PathfindingResult& result);
    bool animatePartialMultiUnitPathsStepByStep(const BattleMap& battleMap, const PathfindingResult& result);

    // Utility Methods
    void displayStaticPathOverview(const BattleMap& battleMap, const std::vector<Position>& path) const;
    bool validateAnimationInputs(const BattleMap& battleMap, const std::vector<Position>& path) const;
    void printAnimationLegend() const;
    void displayStaticMultiUnitOverview(const BattleMap& battleMap, const PathfindingResult& result) const;
    void printMultiUnitAnimationLegend() const;

    // Interactive Animation Control
    bool animateWithControls(const BattleMap& battleMap, const std::vector<Position>& path);

    // Static Utility Methods
    static void printAvailableStyles();
    static void printAvailableSpeeds();
    static AnimationConfig createCustomConfig(AnimationStyle style, AnimationSpeed speed,
                                              char unitSymbol = 'U', char trailSymbol = '.');
    static MultiUnitAnimationConfig createCustomMultiUnitConfig(AnimationStyle style, AnimationSpeed speed);

    // Static Parsing Methods
    static AnimationStyle parseAnimationStyle(const std::string& styleStr);
    static AnimationSpeed parseAnimationSpeed(const std::string& speedStr);
};
```

### Animation Enumerations

#### AnimationStyle

```cpp
enum class AnimationStyle {
    SIMPLE,         // Basic unit movement without trail effects
    WITH_TRAIL,     // Show path trail behind moving unit
    NUMBERED_STEPS, // Display step numbers on the path
    HIGHLIGHT_PATH  // Highlight the entire path and show unit moving along it
};
```

#### AnimationSpeed

```cpp
enum class AnimationSpeed {
    VERY_SLOW = 1000, // 1000ms per step - Very slow for detailed analysis
    SLOW = 500,       // 500ms per step - Slow pace for observation
    NORMAL = 250,     // 250ms per step - Standard comfortable viewing speed
    FAST = 100,       // 100ms per step - Fast pace for quick overview
    VERY_FAST = 50    // 50ms per step - Very fast for rapid demonstration
};
```

## 💡 Usage Examples

### Example 1: Custom Animation Configuration

```cpp
#include "PathAnimator/PathAnimator.h"

void demonstrateAnimationStyles(const BattleMap& battleMap, const std::vector<Position>& path) {
    PathAnimator animator;

    // Test each animation style
    std::vector<AnimationStyle> styles = {
        AnimationStyle::SIMPLE,
        AnimationStyle::WITH_TRAIL,
        AnimationStyle::NUMBERED_STEPS,
        AnimationStyle::HIGHLIGHT_PATH
    };

    std::vector<std::string> styleNames = {
        "Simple Movement",
        "Trail Animation",
        "Numbered Steps",
        "Path Highlighting"
    };

    for (size_t i = 0; i < styles.size(); ++i) {
        std::cout << "\n=== Demonstrating: " << styleNames[i] << " ===" << std::endl;

        AnimationConfig config;
        config.style = styles[i];
        config.speed = AnimationSpeed::SLOW;  // Slow for demonstration
        config.unitSymbol = 'U';
        config.trailSymbol = '.';
        config.pathSymbol = '#';
        config.showProgress = true;

        animator.setConfig(config);

        std::cout << "Press Enter to start animation..." << std::endl;
        std::cin.get();

        animator.animatePath(battleMap, path);

        std::cout << "Animation complete. Press Enter to continue..." << std::endl;
        std::cin.get();
    }
}
```

### Example 2: Multi-Unit Animation with Collision Analysis

```cpp
#include "MultiUnitPathFinder/MultiUnitPathFinder.h"
#include "PathAnimator/PathAnimator.h"

void analyzeMultiUnitCollisions(const std::vector<std::vector<int>>& grid) {
    MultiUnitPathFinder coordinator;
    coordinator.loadMapFromGrid(grid);

    // Setup units that might conflict
    coordinator.addUnit(1, Position(0, 0), Position(10, 10));
    coordinator.addUnit(2, Position(10, 0), Position(0, 10));
    coordinator.addUnit(3, Position(5, 0), Position(5, 10));

    // Test different strategies
    std::vector<ConflictResolutionStrategy> strategies = {
        ConflictResolutionStrategy::SEQUENTIAL,
        ConflictResolutionStrategy::PRIORITY_BASED,
        ConflictResolutionStrategy::COOPERATIVE
    };

    std::vector<std::string> strategyNames = {
        "Sequential", "Priority-Based", "Cooperative"
    };

    PathAnimator animator;
    MultiUnitAnimationConfig config;
    config.style = AnimationStyle::WITH_TRAIL;
    config.speed = AnimationSpeed::NORMAL;
    config.showUnitIds = true;
    config.highlightCollisions = true;
    config.showTrails = true;
    animator.setMultiUnitConfig(config);

    for (size_t i = 0; i < strategies.size(); ++i) {
        std::cout << "\n=== Testing Strategy: " << strategyNames[i] << " ===" << std::endl;

        coordinator.setConflictResolutionStrategy(strategies[i]);
        auto result = coordinator.findPathsForAllUnits();

        // Analyze collisions before animation
        if (result.allPathsFound) {
            auto conflicts = MultiUnitPathFinder::findCollisions(result.stepByStepPositions);
            std::cout << "Conflicts detected: " << conflicts.size() << std::endl;

            if (!conflicts.empty()) {
                std::cout << "Conflict time steps: ";
                for (const auto& conflict : conflicts) {
                    std::cout << conflict.first << " ";
                }
                std::cout << std::endl;
            }
        }

        // Animate the result
        if (result.allPathsFound) {
            animator.animateMultiUnitPaths(coordinator.getBattleMap(), result);
        } else {
            animator.animatePartialMultiUnitPaths(coordinator.getBattleMap(), result);
        }

        std::cout << "Strategy " << strategyNames[i] << " complete. Press Enter to continue..." << std::endl;
        std::cin.get();
    }
}
```

### Example 3: Interactive Animation Control

```cpp
#include "PathAnimator/PathAnimator.h"

class InteractiveAnimationDemo {
private:
    PathAnimator animator;
    BattleMap battleMap;
    std::vector<Position> currentPath;

public:
    void initialize(const BattleMap& map, const std::vector<Position>& path) {
        battleMap = map;
        currentPath = path;
    }

    void runInteractiveDemo() {
        while (true) {
            displayMenu();
            int choice = getUserChoice();

            switch (choice) {
                case 1: demonstrateSpeed(); break;
                case 2: demonstrateStyles(); break;
                case 3: runStepByStep(); break;
                case 4: showStaticOverview(); break;
                case 5: customConfiguration(); break;
                case 0: return;
                default: std::cout << "Invalid choice!" << std::endl;
            }
        }
    }

private:
    void displayMenu() {
        std::cout << "\n=== Interactive Animation Demo ===" << std::endl;
        std::cout << "1. Speed Demonstration" << std::endl;
        std::cout << "2. Style Demonstration" << std::endl;
        std::cout << "3. Step-by-Step Mode" << std::endl;
        std::cout << "4. Static Overview" << std::endl;
        std::cout << "5. Custom Configuration" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Choice: ";
    }

    int getUserChoice() {
        int choice;
        std::cin >> choice;
        return choice;
    }

    void demonstrateSpeed() {
        std::vector<AnimationSpeed> speeds = {
            AnimationSpeed::VERY_SLOW,
            AnimationSpeed::SLOW,
            AnimationSpeed::NORMAL,
            AnimationSpeed::FAST,
            AnimationSpeed::VERY_FAST
        };

        std::vector<std::string> speedNames = {
            "Very Slow (1000ms)", "Slow (500ms)", "Normal (250ms)",
            "Fast (100ms)", "Very Fast (50ms)"
        };

        for (size_t i = 0; i < speeds.size(); ++i) {
            std::cout << "\nDemonstrating: " << speedNames[i] << std::endl;

            AnimationConfig config;
            config.style = AnimationStyle::WITH_TRAIL;
            config.speed = speeds[i];
            config.showProgress = true;
            animator.setConfig(config);

            animator.animatePath(battleMap, currentPath);

            std::cout << "Speed demo complete." << std::endl;
        }
    }

    void demonstrateStyles() {
        std::vector<AnimationStyle> styles = {
            AnimationStyle::SIMPLE,
            AnimationStyle::WITH_TRAIL,
            AnimationStyle::NUMBERED_STEPS,
            AnimationStyle::HIGHLIGHT_PATH
        };

        for (auto style : styles) {
            AnimationConfig config;
            config.style = style;
            config.speed = AnimationSpeed::NORMAL;
            animator.setConfig(config);

            animator.animatePath(battleMap, currentPath);
        }
    }

    void runStepByStep() {
        std::cout << "\nRunning step-by-step animation..." << std::endl;
        std::cout << "Press ENTER to advance, 'q' to quit" << std::endl;

        animator.animatePathStepByStep(battleMap, currentPath);
    }

    void showStaticOverview() {
        animator.displayStaticPathOverview(battleMap, currentPath);
        animator.printAnimationLegend();
    }

    void customConfiguration() {
        std::cout << "\n=== Custom Configuration ===" << std::endl;

        // Get user preferences
        std::cout << "Unit symbol (default 'U'): ";
        char unitSymbol;
        std::cin >> unitSymbol;

        std::cout << "Trail symbol (default '.'): ";
        char trailSymbol;
        std::cin >> trailSymbol;

        std::cout << "Animation speed (1-5, 1=very slow, 5=very fast): ";
        int speedChoice;
        std::cin >> speedChoice;

        AnimationSpeed speed = static_cast<AnimationSpeed>(1000 / speedChoice);

        // Create custom config
        AnimationConfig config = PathAnimator::createCustomConfig(
            AnimationStyle::WITH_TRAIL, speed, unitSymbol, trailSymbol
        );

        animator.setConfig(config);
        animator.animatePath(battleMap, currentPath);
    }
};
```

### Example 4: Animation with Performance Monitoring

```cpp
#include "PathAnimator/PathAnimator.h"
#include <chrono>

class PerformanceMonitoredAnimation {
public:
    struct AnimationMetrics {
        size_t pathLength;
        int animationDuration;
        AnimationStyle style;
        AnimationSpeed speed;
        bool completedSuccessfully;
    };

    static AnimationMetrics benchmarkAnimation(
        const BattleMap& battleMap,
        const std::vector<Position>& path,
        AnimationStyle style,
        AnimationSpeed speed) {

        PathAnimator animator;
        AnimationConfig config;
        config.style = style;
        config.speed = speed;
        config.showProgress = false;  // Disable for cleaner benchmarking
        animator.setConfig(config);

        auto start = std::chrono::high_resolution_clock::now();
        bool success = animator.animatePath(battleMap, path);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        return {
            path.size(),
            static_cast<int>(duration.count()),
            style,
            speed,
            success
        };
    }

    static void runPerformanceBenchmark(const BattleMap& battleMap, const std::vector<Position>& path) {
        std::cout << "=== Animation Performance Benchmark ===" << std::endl;

        std::vector<AnimationStyle> styles = {
            AnimationStyle::SIMPLE,
            AnimationStyle::WITH_TRAIL,
            AnimationStyle::NUMBERED_STEPS,
            AnimationStyle::HIGHLIGHT_PATH
        };

        std::vector<AnimationSpeed> speeds = {
            AnimationSpeed::VERY_FAST,
            AnimationSpeed::FAST,
            AnimationSpeed::NORMAL
        };

        std::vector<std::string> styleNames = {
            "Simple", "Trail", "Numbered", "Highlight"
        };

        std::cout << std::setw(12) << "Style"
                  << std::setw(12) << "Speed(ms)"
                  << std::setw(15) << "Total Time(ms)"
                  << std::setw(10) << "Success" << std::endl;
        std::cout << std::string(50, '-') << std::endl;

        for (size_t i = 0; i < styles.size(); ++i) {
            for (auto speed : speeds) {
                auto metrics = benchmarkAnimation(battleMap, path, styles[i], speed);

                std::cout << std::setw(12) << styleNames[i]
                          << std::setw(12) << static_cast<int>(metrics.speed)
                          << std::setw(15) << metrics.animationDuration
                          << std::setw(10) << (metrics.completedSuccessfully ? "Yes" : "No")
                          << std::endl;
            }
        }
    }
};
```

## 🖥️ Terminal Requirements

### Supported Terminals

#### Linux

- **gnome-terminal**: Full support
- **konsole**: Full support
- **xterm**: Full support
- **terminology**: Full support
- **tilix**: Full support

#### macOS

- **Terminal.app**: Full support
- **iTerm2**: Enhanced support with better Unicode
- **Hyper**: Good support

#### Windows

- **Windows Terminal**: Recommended (full ANSI support)
- **WSL terminals**: Full support within WSL environment
- **Command Prompt**: Limited support (no colors)
- **PowerShell**: Good support

### ANSI Support Requirements

The library uses ANSI escape codes for:

```cpp
// Screen control
#define CLEAR_SCREEN "\033[2J"
#define MOVE_CURSOR_HOME "\033[H"
#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"

// Color formatting
std::cout << "\033[" << colorCode << "m";  // Set color
std::cout << "\033[0m";                    // Reset color

// Colors used
const int GREEN = 32;
const int YELLOW = 33;
const int BLUE = 34;
const int MAGENTA = 35;
const int CYAN = 36;
const int BRIGHT_GREEN = 92;
const int BRIGHT_YELLOW = 93;
const int BRIGHT_BLUE = 94;
const int BRIGHT_MAGENTA = 95;
const int BRIGHT_RED = 91;
```

### Unicode Character Requirements

For proper visualization, terminals should support:

- **U+2591** (░): Light shade for reachable terrain
- **U+25B2** (▲): Up-pointing triangle for obstacles
- **Standard ASCII**: S, T, numbers, letters for units and positions

### Performance Considerations

- **Screen Updates**: Optimized to minimize flicker
- **Memory Usage**: Efficient path storage and rendering
- **CPU Usage**: Configurable animation speeds to balance smoothness and performance

```cpp
// Performance optimization example
AnimationConfig performanceConfig;
performanceConfig.clearScreenBetweenFrames = false;  // Reduce screen clearing
performanceConfig.showProgress = false;              // Disable progress bar
performanceConfig.speed = AnimationSpeed::FAST;     // Faster updates
```

## 🔧 Integration Guide

### Integration with Command Line Applications

```cpp
// Parse animation settings from command line
int main(int argc, char* argv[]) {
    std::string styleStr = "trail";
    std::string speedStr = "normal";
    bool stepByStep = false;
    bool enableAnimation = false;

    // Parse arguments...
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--animate") {
            enableAnimation = true;
        } else if (std::string(argv[i]) == "--step-by-step") {
            enableAnimation = true;
            stepByStep = true;
        } else if (std::string(argv[i]) == "--style" && i + 1 < argc) {
            styleStr = argv[++i];
        } else if (std::string(argv[i]) == "--speed" && i + 1 < argc) {
            speedStr = argv[++i];
        }
    }

    if (enableAnimation) {
        PathAnimator animator;
        AnimationConfig config;
        config.style = PathAnimator::parseAnimationStyle(styleStr);
        config.speed = PathAnimator::parseAnimationSpeed(speedStr);
        animator.setConfig(config);

        if (stepByStep) {
            animator.animatePathStepByStep(battleMap, path);
        } else {
            animator.animatePath(battleMap, path);
        }
    }
}
```

### Integration with Makefile System

```bash
# Animation flags for Makefile
ANIMATE_FLAGS=""
ifeq ($(ANIMATE),yes)
    ANIMATE_FLAGS="--animate"
endif
ifeq ($(ANIMATE),step)
    ANIMATE_FLAGS="--step-by-step"
endif

# Enhanced run targets
run-pathfinder:
    ./pathfinder $(FILE) $(ALGO_FLAGS) $(MOVE_FLAGS) $(ANIMATE_FLAGS)

test-with-animation:
    ./pathfinder $(FILE) --algorithm astar --animate --speed fast
```

### Integration with Educational Systems

```cpp
class EducationalAnimator {
private:
    PathAnimator animator;

public:
    void teachPathfindingConcepts() {
        // Demonstrate algorithm differences
        demonstrateAlgorithmDifferences();

        // Show movement order impact
        demonstrateMovementOrders();

        // Explain multi-unit coordination
        demonstrateMultiUnitConcepts();
    }

private:
    void demonstrateAlgorithmDifferences() {
        std::cout << "=== Teaching: Algorithm Differences ===" << std::endl;

        // A* demonstration
        std::cout << "\nA* Algorithm: Optimal pathfinding with heuristics" << std::endl;
        AnimationConfig astarConfig;
        astarConfig.style = AnimationStyle::NUMBERED_STEPS;
        astarConfig.speed = AnimationSpeed::SLOW;
        animator.setConfig(astarConfig);

        // Show A* path with explanation
        // ... pathfinding and animation code

        // BFS demonstration
        std::cout << "\nBFS Algorithm: Guaranteed shortest path" << std::endl;
        // ... similar setup for BFS

        // DFS demonstration
        std::cout << "\nDFS Algorithm: Exploration-based pathfinding" << std::endl;
        // ... similar setup for DFS
    }

    void demonstrateMovementOrders() {
        std::cout << "=== Teaching: Movement Order Impact ===" << std::endl;

        std::vector<std::string> orders = {"rdlu", "uldr", "ldru", "dlur"};
        std::vector<std::string> descriptions = {
            "Right-Down-Left-Up: Balanced approach",
            "Up-Left-Down-Right: Aggressive positioning",
            "Left-Down-Right-Up: Defensive positioning",
            "Down-Left-Up-Right: Retreat patterns"
        };

        for (size_t i = 0; i < orders.size(); ++i) {
            std::cout << "\n" << descriptions[i] << std::endl;
            // Demonstrate each movement order with animation
        }
    }

    void demonstrateMultiUnitConcepts() {
        std::cout << "=== Teaching: Multi-Unit Coordination ===" << std::endl;

        // Show different conflict resolution strategies
        // ... multi-unit pathfinding setup and animation
    }
};
```

## 🎯 Best Practices

### Animation Performance Optimization

```cpp
// For large maps or many units
AnimationConfig optimizedConfig;
optimizedConfig.clearScreenBetweenFrames = false;  // Reduce screen clearing overhead
optimizedConfig.showProgress = false;              // Disable progress bar
optimizedConfig.speed = AnimationSpeed::FAST;     // Use faster speeds

// For educational/presentation purposes
AnimationConfig presentationConfig;
presentationConfig.style = AnimationStyle::NUMBERED_STEPS;
optimizedConfig.speed = AnimationSpeed::SLOW;     // Slower for explanation
optimizedConfig.showProgress = true;              // Show progress for audience
```

### Error Handling

```cpp
// Always validate inputs before animation
if (!animator.validateAnimationInputs(battleMap, path)) {
    std::cerr << "Invalid animation inputs - cannot animate" << std::endl;
    // Show static overview instead
    animator.displayStaticPathOverview(battleMap, path);
    return;
}

// Handle animation failures gracefully
if (!animator.animatePath(battleMap, path)) {
    std::cerr << "Animation failed or was interrupted" << std::endl;
    // Provide fallback visualization
    battleMap.displayMapWithPath(path);
}
```

### Multi-Unit Best Practices

```cpp
// Check for partial results
if (!result.allPathsFound) {
    int successfulUnits = 0;
    for (const auto& unit : result.units) {
        if (unit.pathFound) successfulUnits++;
    }

    if (successfulUnits > 0) {
        std::cout << "Animating " << successfulUnits << " successful units" << std::endl;
        animator.animatePartialMultiUnitPaths(battleMap, result);
    } else {
        std::cout << "No units found paths - showing static overview" << std::endl;
        coordinator.displayUnits();
    }
}

// Handle collision analysis
auto conflicts = MultiUnitPathFinder::findCollisions(result.stepByStepPositions);
if (!conflicts.empty()) {
    std::cout << "Warning: " << conflicts.size() << " collisions detected" << std::endl;
    // Enable collision highlighting
    MultiUnitAnimationConfig config = animator.getMultiUnitConfig();
    config.highlightCollisions = true;
    animator.setMultiUnitConfig(config);
}
```

### User Experience Guidelines

```cpp
// Provide clear instructions to users
void showAnimationInstructions() {
    std::cout << "\n=== Animation Controls ===" << std::endl;
    std::cout << "• Press Ctrl+C to interrupt animation at any time" << std::endl;
    std::cout << "• In step-by-step mode: Press ENTER to advance, 'q' to quit" << std::endl;
    std::cout << "• Animation will start after you press ENTER" << std::endl;
    std::cout << "\nPress ENTER to begin..." << std::endl;
    std::cin.get();
}

// Always show legends for clarity
animator.printAnimationLegend();           // For single-unit
animator.printMultiUnitAnimationLegend();  // For multi-unit
```

## 🔍 Troubleshooting

### Common Issues

#### Animation Not Displaying Properly

```cpp
// Check terminal compatibility
std::cout << "Testing ANSI support..." << std::endl;
std::cout << "\033[32mGreen text\033[0m" << std::endl;
std::cout << "\033[2J\033[H";  // Clear screen and move cursor

if (/* user reports no color or clearing */) {
    std::cout << "Your terminal may not support ANSI codes." << std::endl;
    std::cout << "Try using: Windows Terminal, gnome-terminal, or iTerm2" << std::endl;

    // Fallback to static display
    animator.displayStaticPathOverview(battleMap, path);
}
```

#### Performance Issues

```cpp
// Monitor animation performance
auto start = std::chrono::high_resolution_clock::now();
bool success = animator.animatePath(battleMap, path);
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

if (duration.count() > path.size() * static_cast<int>(config.speed) * 2) {
    std::cout << "Animation performance warning: took " << duration.count()
              << "ms for " << path.size() << " steps" << std::endl;
    std::cout << "Consider using faster animation speed or simpler style" << std::endl;
}
```

#### Unicode Display Issues

```cpp
// Test Unicode support
std::cout << "Testing Unicode support:" << std::endl;
std::cout << "Reachable: ░░" << std::endl;
std::cout << "Obstacles: ▲▲" << std::endl;
std::cout << "If you see question marks or boxes, your terminal" << std::endl;
std::cout << "may not support Unicode properly." << std::endl;

// Provide ASCII fallback
AnimationConfig asciiConfig;
asciiConfig.unitSymbol = 'U';
asciiConfig.trailSymbol = '.';
asciiConfig.pathSymbol = '#';
// Use only ASCII characters
```

### Debug Utilities

```cpp
// Comprehensive animation debugging
void debugAnimation(const PathAnimator& animator, const BattleMap& battleMap,
                   const std::vector<Position>& path) {
    std::cout << "=== Animation Debug Information ===" << std::endl;

    // Validate inputs
    bool validInputs = animator.validateAnimationInputs(battleMap, path);
    std::cout << "Valid inputs: " << (validInputs ? "Yes" : "No") << std::endl;

    if (!validInputs) {
        std::cout << "Validation failed - checking individual components:" << std::endl;
        std::cout << "  Path empty: " << (path.empty() ? "Yes" : "No") << std::endl;
        std::cout << "  Map dimensions: " << battleMap.width << "x" << battleMap.height << std::endl;

        // Check path bounds
        for (const auto& pos : path) {
            if (pos.x < 0 || pos.x >= battleMap.width ||
                pos.y < 0 || pos.y >= battleMap.height) {
                std::cout << "  Out of bounds position: (" << pos.x << "," << pos.y << ")" << std::endl;
            }
        }
    }

    // Display configuration
    const auto& config = animator.getConfig();
    std::cout << "Animation configuration:" << std::endl;
    std::cout << "  Style: " << static_cast<int>(config.style) << std::endl;
    std::cout << "  Speed: " << static_cast<int>(config.speed) << "ms" << std::endl;
    std::cout << "  Unit symbol: '" << config.unitSymbol << "'" << std::endl;
    std::cout << "  Show progress: " << (config.showProgress ? "Yes" : "No") << std::endl;
}
```

### Performance Optimization

```cpp
// Optimize for different scenarios
class OptimizedAnimator {
public:
    static AnimationConfig getOptimizedConfig(const std::vector<Position>& path,
                                             const std::string& scenario) {
        AnimationConfig config;

        if (scenario == "presentation") {
            config.style = AnimationStyle::NUMBERED_STEPS;
            config.speed = AnimationSpeed::SLOW;
            config.showProgress = true;
        } else if (scenario == "debugging") {
            config.style = AnimationStyle::WITH_TRAIL;
            config.speed = AnimationSpeed::NORMAL;
            config.showProgress = true;
        } else if (scenario == "performance") {
            config.style = AnimationStyle::SIMPLE;
            config.speed = AnimationSpeed::VERY_FAST;
            config.showProgress = false;
            config.clearScreenBetweenFrames = false;
        } else { // default
            config.style = AnimationStyle::WITH_TRAIL;
            config.speed = AnimationSpeed::NORMAL;
            config.showProgress = true;
        }

        return config;
    }
};
```

---
