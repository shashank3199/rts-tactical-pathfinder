# MultiUnitPathFinder Library

[![C++](https://img.shields.io/badge/C%2B%2B-11%2B-blue.svg)](https://isocpp.org/)

A sophisticated C++ library for coordinating pathfinding across multiple units simultaneously, featuring advanced conflict resolution strategies, temporal pathfinding, and comprehensive collision management for tactical battle scenarios.

## 🎯 Overview

The **MultiUnitPathFinder** extends the PathFinder framework to handle complex multi-unit scenarios where units must coordinate their movements to avoid collisions while reaching their individual targets. It provides intelligent conflict resolution strategies suitable for RTS games, tactical simulations, and coordinated robotics applications.

## ✨ Key Features

### 🤝 **Multi-Unit Coordination**

- **Temporal Pathfinding**: Time-aware pathfinding that considers unit positions over time
- **Conflict Resolution**: Four sophisticated strategies for handling unit interactions
- **Priority Management**: Flexible priority assignment for strategic unit precedence
- **Automatic Setup**: Intelligent unit detection and configuration from map data

### ⚔️ **Conflict Resolution Strategies**

1. **Sequential**: Units pathfind one after another, avoiding previous paths
2. **Priority-Based**: Higher priority units get preference in pathfinding
3. **Cooperative**: Mutual path compatibility through multiple attempts
4. **Wait-and-Retry**: Units can wait in place when blocked by other units

### 🎮 **Advanced Features**

- **Partial Path Support**: Handle scenarios where some units cannot find paths
- **Animation Integration**: Seamless integration with PathAnimator for visualization
- **Step-by-Step Analysis**: Detailed time-step progression for debugging
- **Collision Detection**: Real-time identification and resolution of unit conflicts

## 🔧 Installation

### Prerequisites

- **PathFinder Library**: Core pathfinding algorithms (included in project)
- **C++ Compiler**: GCC 4.8+ or Clang 3.4+ with C++11 support
- **Standard Libraries**: STL containers, algorithms, memory management

### Building

```bash
# Using provided Makefile (from parent directory)
make pathfinder  # Includes MultiUnitPathFinder

# Manual compilation
g++ -std=c++11 -Wall -O2 -IPathFinder -IMultiUnitPathFinder -IMapLoader \
    main.cpp PathFinder/PathFinder.cpp MultiUnitPathFinder/MultiUnitPathFinder.cpp \
    MapLoader/MapLoader.cpp -ljsoncpp -o pathfinder
```

## ⚡ Quick Start

### Basic Multi-Unit Coordination

```cpp
#include "MultiUnitPathFinder/MultiUnitPathFinder.h"

int main() {
    // Initialize multi-unit pathfinder
    MultiUnitPathFinder coordinator("uldr");  // Movement order preference

    // Load battle map
    std::vector<std::vector<int>> grid = {
        {-1, -1, -1, -1, -1, -1, -1},
        {-1,  3,  3, -1,  3,  3, -1},
        { 0, -1, -1, -1, -1, -1,  8},
        { 0, -1,  3, -1,  3, -1,  8},
        {-1, -1, -1, -1, -1, -1, -1}
    };
    coordinator.loadMapFromGrid(grid);

    // Add units with priorities
    coordinator.addUnit(1, Position(0, 2), Position(6, 2));  // Unit 1
    coordinator.addUnit(2, Position(0, 3), Position(6, 3));  // Unit 2
    coordinator.setUnitPriority(1, 100);  // High priority
    coordinator.setUnitPriority(2, 50);   // Lower priority

    // Configure conflict resolution strategy
    coordinator.setConflictResolutionStrategy(
        ConflictResolutionStrategy::PRIORITY_BASED
    );

    // Find coordinated paths
    PathfindingResult result = coordinator.findPathsForAllUnits();

    // Display results
    if (result.allPathsFound) {
        std::cout << "All units found paths!" << std::endl;
        coordinator.displayMapWithAllPaths(result);
        coordinator.displayStepByStep(result);
    } else {
        std::cout << "Some units failed to find paths." << std::endl;
        coordinator.displayPathfindingResult(result);
    }

    return 0;
}
```

### Auto-Setup from Map Data

```cpp
// Automatically detect units from map positions
MultiUnitPathFinder coordinator;
coordinator.loadMapFromGrid(battleGrid);

if (coordinator.autoSetupUnitsFromMap()) {
    coordinator.displayUnits();  // Show detected units

    // Set strategy and find paths
    coordinator.setConflictResolutionStrategy(
        ConflictResolutionStrategy::COOPERATIVE
    );

    auto result = coordinator.findPathsForAllUnits();
    coordinator.displayPathfindingResult(result);
}
```

## ⚔️ Conflict Resolution Strategies

### 1. Sequential Strategy

**How it works**: Units pathfind one after another, with each subsequent unit avoiding the paths of previously processed units.

**Characteristics**:

- **Time Complexity**: O(n x A) where n = units, A = single pathfinding complexity
- **Optimality**: Sub-optimal for later units
- **Reliability**: High success rate
- **Use Case**: When unit order importance is clear

```cpp
coordinator.setConflictResolutionStrategy(ConflictResolutionStrategy::SEQUENTIAL);
auto result = coordinator.findPathsForAllUnits();
```

**Advantages**:

- Guaranteed conflict-free paths
- Predictable behavior
- Fast execution

**Disadvantages**:

- Later units get suboptimal paths
- Order dependency

### 2. Priority-Based Strategy

**How it works**: Units are sorted by priority and processed sequentially, ensuring higher priority units get optimal paths.

**Characteristics**:

- **Time Complexity**: O(n x A + n log n) for sorting
- **Optimality**: Good for high-priority units
- **Reliability**: High success rate
- **Use Case**: When unit importance varies significantly

```cpp
// Set unit priorities
coordinator.setUnitPriority(commanderUnit, 100);
coordinator.setUnitPriority(supportUnit, 50);
coordinator.setUnitPriority(scoutUnit, 10);

coordinator.setConflictResolutionStrategy(ConflictResolutionStrategy::PRIORITY_BASED);
```

**Advantages**:

- Important units get optimal paths
- Clear precedence system
- Suitable for hierarchical scenarios

**Disadvantages**:

- Low-priority units may suffer
- Requires careful priority assignment

### 3. Cooperative Strategy

**How it works**: Attempts multiple different unit orderings to find mutually compatible paths for all units.

**Characteristics**:

- **Time Complexity**: O(n^2 x A) with multiple attempts
- **Optimality**: Better overall solutions
- **Reliability**: Good with sufficient attempts
- **Use Case**: When all units are equally important

```cpp
coordinator.setConflictResolutionStrategy(ConflictResolutionStrategy::COOPERATIVE);
auto result = coordinator.findPathsForAllUnits();
```

**Advantages**:

- Better overall path quality
- Fairer distribution of path costs
- Explores multiple solutions

**Disadvantages**:

- Higher computational cost
- Non-deterministic results
- May still fail in complex scenarios

### 4. Wait-and-Retry Strategy

**How it works**: Units can wait in place when blocked, potentially resolving conflicts through temporal coordination.

**Characteristics**:

- **Time Complexity**: O(n x A x k) where k = retry factor
- **Optimality**: Variable, depends on waiting decisions
- **Reliability**: Good for temporal conflicts
- **Use Case**: When units can afford to wait

```cpp
coordinator.setConflictResolutionStrategy(ConflictResolutionStrategy::WAIT_AND_RETRY);
auto result = coordinator.findPathsForAllUnits();
```

**Advantages**:

- Handles temporal conflicts well
- Flexible timing coordination
- Can resolve deadlocks

**Disadvantages**:

- May result in longer total time
- Complex temporal planning
- Unpredictable completion times

### Strategy Comparison

| Strategy       | Time Complexity | Path Quality            | Success Rate | Best Use Case           |
| -------------- | --------------- | ----------------------- | ------------ | ----------------------- |
| Sequential     | O(n x A)        | Variable                | High         | Clear unit hierarchy    |
| Priority-Based | O(n x A)        | Good for priority units | High         | Mixed importance units  |
| Cooperative    | O(n^2 x A)      | Good overall            | Medium       | Equal importance units  |
| Wait-and-Retry | O(n x A x k)    | Variable                | Medium       | Time-flexible scenarios |

## 📖 API Documentation

### Core Classes

#### MultiUnitPathFinder Class

```cpp
class MultiUnitPathFinder : public PathFinder {
public:
    // Constructors
    MultiUnitPathFinder();
    MultiUnitPathFinder(const std::string& moveOrder);

    // Unit Management
    void addUnit(int unitId, const Position& startPos, const Position& targetPos);
    void addUnit(const Unit& unit);
    void removeUnit(int unitId);
    void clearUnits();
    bool autoSetupUnitsFromMap();

    // Priority Management
    void setUnitPriority(int unitId, int priority);
    int getUnitPriority(int unitId) const;

    // Configuration
    void setConflictResolutionStrategy(ConflictResolutionStrategy strategy);
    ConflictResolutionStrategy getConflictResolutionStrategy() const;

    // Pathfinding Operations
    PathfindingResult findPathsForAllUnits();

    // Information and Queries
    std::vector<Unit> getUnits() const;
    int getUnitCount() const;
    bool validateUnitPaths(const PathfindingResult& result) const;

    // Display and Visualization
    void displayUnits() const;
    void displayPathfindingResult(const PathfindingResult& result) const;
    void displayStepByStep(const PathfindingResult& result) const;
    void displayMapWithAllPaths(const PathfindingResult& result) const;

    // Map Loading
    bool loadMapWithUnits(const std::vector<std::vector<int>>& grid,
                         const std::vector<Unit>& units);

    // Static Utility Methods
    static bool hasCollision(const std::vector<std::vector<Position>>& stepByStepPositions,
                           int timeStep);
    static std::vector<std::pair<int, int>> findCollisions(
        const std::vector<std::vector<Position>>& stepByStepPositions);
    static std::vector<std::vector<Position>> generateStepByStepPositions(
        const std::vector<Unit>& unitsWithPaths);
    static void printConflictResolutionStrategies();
};
```

### Supporting Structures

#### Unit Structure

```cpp
struct Unit {
    int id;                          // Unique unit identifier
    Position startPos;               // Starting position
    Position targetPos;              // Target position
    std::vector<Position> path;      // Calculated path
    bool pathFound;                  // Whether a valid path was found

    Unit();                          // Default constructor
    Unit(int unitId, const Position& start, const Position& target);
};
```

#### PathfindingResult Structure

```cpp
struct PathfindingResult {
    std::vector<Unit> units;                                    // All units with paths
    bool allPathsFound;                                         // Success status
    int totalSteps;                                             // Total time steps
    std::vector<std::vector<Position>> stepByStepPositions;     // Time-step positions

    PathfindingResult();             // Default constructor
};
```

#### ConflictResolutionStrategy Enumeration

```cpp
enum class ConflictResolutionStrategy {
    SEQUENTIAL,                      // Process units one by one
    PRIORITY_BASED,                  // Process by priority order
    COOPERATIVE,                     // Multiple attempts for compatibility
    WAIT_AND_RETRY                   // Allow waiting when blocked
};
```

## 💡 Usage Examples

### Example 1: Strategic Unit Coordination

```cpp
#include "MultiUnitPathFinder/MultiUnitPathFinder.h"

void coordinateStrike(const std::vector<std::vector<int>>& battleMap) {
    MultiUnitPathFinder commander;
    commander.loadMapFromGrid(battleMap);

    // Setup strike team
    commander.addUnit(1, Position(0, 0), Position(19, 19));    // Commander
    commander.addUnit(2, Position(1, 0), Position(18, 19));   // Heavy unit
    commander.addUnit(3, Position(0, 1), Position(19, 18));   // Support
    commander.addUnit(4, Position(2, 0), Position(17, 19));   // Scout

    // Set tactical priorities
    commander.setUnitPriority(1, 100);  // Commander highest priority
    commander.setUnitPriority(2, 80);   // Heavy unit second
    commander.setUnitPriority(3, 60);   // Support third
    commander.setUnitPriority(4, 90);   // Scout needs mobility

    // Use priority strategy for tactical coordination
    commander.setConflictResolutionStrategy(
        ConflictResolutionStrategy::PRIORITY_BASED
    );

    auto result = commander.findPathsForAllUnits();

    if (result.allPathsFound) {
        std::cout << "Strike coordinated successfully!" << std::endl;

        // Analyze coordination
        auto conflicts = MultiUnitPathFinder::findCollisions(result.stepByStepPositions);
        if (conflicts.empty()) {
            std::cout << "No conflicts detected - perfect coordination!" << std::endl;
        } else {
            std::cout << "Warning: " << conflicts.size() << " conflicts detected" << std::endl;
        }

        commander.displayStepByStep(result);
    } else {
        std::cout << "Strike coordination failed - trying cooperative strategy..." << std::endl;

        // Fallback to cooperative strategy
        commander.setConflictResolutionStrategy(
            ConflictResolutionStrategy::COOPERATIVE
        );

        auto fallbackResult = commander.findPathsForAllUnits();
        commander.displayPathfindingResult(fallbackResult);
    }
}
```

### Example 2: Dynamic Conflict Resolution

```cpp
#include "MultiUnitPathFinder/MultiUnitPathFinder.h"

class AdaptiveCoordinator {
private:
    MultiUnitPathFinder pathfinder;
    std::vector<ConflictResolutionStrategy> strategies;

public:
    AdaptiveCoordinator() {
        strategies = {
            ConflictResolutionStrategy::SEQUENTIAL,
            ConflictResolutionStrategy::PRIORITY_BASED,
            ConflictResolutionStrategy::COOPERATIVE,
            ConflictResolutionStrategy::WAIT_AND_RETRY
        };
    }

    PathfindingResult findBestSolution(const std::vector<std::vector<int>>& grid,
                                      const std::vector<Unit>& units) {
        pathfinder.loadMapFromGrid(grid);
        pathfinder.clearUnits();

        for (const auto& unit : units) {
            pathfinder.addUnit(unit);
        }

        PathfindingResult bestResult;
        int bestScore = -1;

        // Try each strategy and pick the best result
        for (auto strategy : strategies) {
            pathfinder.setConflictResolutionStrategy(strategy);
            auto result = pathfinder.findPathsForAllUnits();

            int score = evaluateResult(result);

            std::cout << "Strategy " << static_cast<int>(strategy)
                      << " score: " << score << std::endl;

            if (score > bestScore) {
                bestScore = score;
                bestResult = result;
            }
        }

        return bestResult;
    }

private:
    int evaluateResult(const PathfindingResult& result) {
        if (!result.allPathsFound) {
            // Count successful units
            int successCount = 0;
            for (const auto& unit : result.units) {
                if (unit.pathFound) successCount++;
            }
            return successCount * 10;  // Partial success
        }

        // All paths found - evaluate quality
        int totalPathLength = 0;
        for (const auto& unit : result.units) {
            totalPathLength += unit.path.size();
        }

        // Check for conflicts
        auto conflicts = MultiUnitPathFinder::findCollisions(result.stepByStepPositions);
        int conflictPenalty = conflicts.size() * 50;

        // Higher score is better (shorter paths, fewer conflicts)
        return 1000 - totalPathLength - conflictPenalty;
    }
};
```

### Example 3: Real-Time Unit Management

```cpp
#include "MultiUnitPathFinder/MultiUnitPathFinder.h"

class RealTimeCoordinator {
private:
    MultiUnitPathFinder pathfinder;
    std::map<int, Unit> activeUnits;
    int nextUnitId;

public:
    RealTimeCoordinator() : nextUnitId(1) {
        pathfinder.setConflictResolutionStrategy(
            ConflictResolutionStrategy::PRIORITY_BASED
        );
    }

    bool initialize(const std::vector<std::vector<int>>& battleMap) {
        return pathfinder.loadMapFromGrid(battleMap);
    }

    int addNewUnit(const Position& start, const Position& target, int priority = 50) {
        int unitId = nextUnitId++;

        pathfinder.addUnit(unitId, start, target);
        pathfinder.setUnitPriority(unitId, priority);

        activeUnits[unitId] = Unit(unitId, start, target);

        // Recalculate all paths when new unit is added
        replanAllUnits();

        return unitId;
    }

    void removeUnit(int unitId) {
        pathfinder.removeUnit(unitId);
        activeUnits.erase(unitId);

        // Replan remaining units for better paths
        replanAllUnits();
    }

    void updateUnitTarget(int unitId, const Position& newTarget) {
        auto it = activeUnits.find(unitId);
        if (it != activeUnits.end()) {
            it->second.targetPos = newTarget;

            // Remove and re-add unit with new target
            pathfinder.removeUnit(unitId);
            pathfinder.addUnit(it->second);

            replanAllUnits();
        }
    }

    PathfindingResult getCurrentPlan() {
        return pathfinder.findPathsForAllUnits();
    }

    void setUnitPriority(int unitId, int priority) {
        pathfinder.setUnitPriority(unitId, priority);
        replanAllUnits();
    }

    std::vector<Position> getUnitPath(int unitId) {
        auto result = pathfinder.findPathsForAllUnits();

        for (const auto& unit : result.units) {
            if (unit.id == unitId && unit.pathFound) {
                return unit.path;
            }
        }

        return {};  // No path found
    }

    void displayCurrentSituation() {
        std::cout << "Active units: " << activeUnits.size() << std::endl;

        auto result = getCurrentPlan();
        pathfinder.displayPathfindingResult(result);

        if (result.allPathsFound) {
            auto conflicts = MultiUnitPathFinder::findCollisions(result.stepByStepPositions);
            std::cout << "Current conflicts: " << conflicts.size() << std::endl;
        }
    }

private:
    void replanAllUnits() {
        // Trigger replanning - in a real-time system, this might be
        // done asynchronously or with rate limiting
        auto result = pathfinder.findPathsForAllUnits();

        // Update internal unit data with new paths
        for (const auto& unit : result.units) {
            auto it = activeUnits.find(unit.id);
            if (it != activeUnits.end()) {
                it->second.path = unit.path;
                it->second.pathFound = unit.pathFound;
            }
        }
    }
};
```

## 📊 Performance Analysis

### Benchmarking Multi-Unit Strategies

```cpp
#include <chrono>

class MultiUnitBenchmark {
public:
    struct StrategyResult {
        ConflictResolutionStrategy strategy;
        bool allPathsFound;
        int successfulUnits;
        int totalPathLength;
        long long executionTime;  // microseconds
        int conflictCount;
    };

    static std::vector<StrategyResult> benchmarkStrategies(
        const std::vector<std::vector<int>>& grid,
        const std::vector<Unit>& units) {

        std::vector<StrategyResult> results;

        std::vector<ConflictResolutionStrategy> strategies = {
            ConflictResolutionStrategy::SEQUENTIAL,
            ConflictResolutionStrategy::PRIORITY_BASED,
            ConflictResolutionStrategy::COOPERATIVE,
            ConflictResolutionStrategy::WAIT_AND_RETRY
        };

        for (auto strategy : strategies) {
            MultiUnitPathFinder pathfinder;
            pathfinder.loadMapFromGrid(grid);

            for (const auto& unit : units) {
                pathfinder.addUnit(unit);
            }

            pathfinder.setConflictResolutionStrategy(strategy);

            auto start = std::chrono::high_resolution_clock::now();
            auto result = pathfinder.findPathsForAllUnits();
            auto end = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            // Analyze results
            int successfulUnits = 0;
            int totalPathLength = 0;

            for (const auto& unit : result.units) {
                if (unit.pathFound) {
                    successfulUnits++;
                    totalPathLength += unit.path.size();
                }
            }

            int conflictCount = 0;
            if (result.allPathsFound) {
                auto conflicts = MultiUnitPathFinder::findCollisions(result.stepByStepPositions);
                conflictCount = conflicts.size();
            }

            results.push_back({
                strategy,
                result.allPathsFound,
                successfulUnits,
                totalPathLength,
                duration.count(),
                conflictCount
            });
        }

        return results;
    }

    static void printBenchmarkResults(const std::vector<StrategyResult>& results) {
        std::cout << std::setw(15) << "Strategy"
                  << std::setw(12) << "All Found"
                  << std::setw(12) << "Successful"
                  << std::setw(15) << "Total Length"
                  << std::setw(15) << "Time (μs)"
                  << std::setw(12) << "Conflicts" << std::endl;
        std::cout << std::string(85, '-') << std::endl;

        std::vector<std::string> strategyNames = {
            "Sequential", "Priority", "Cooperative", "Wait-Retry"
        };

        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            std::cout << std::setw(15) << strategyNames[i]
                      << std::setw(12) << (result.allPathsFound ? "Yes" : "No")
                      << std::setw(12) << result.successfulUnits
                      << std::setw(15) << result.totalPathLength
                      << std::setw(15) << result.executionTime
                      << std::setw(12) << result.conflictCount << std::endl;
        }
    }
};
```

### Performance Characteristics

#### Scalability Analysis

| Number of Units | Sequential | Priority-Based | Cooperative | Wait-and-Retry |
| --------------- | ---------- | -------------- | ----------- | -------------- |
| 2-5 units       | Excellent  | Excellent      | Good        | Good           |
| 6-10 units      | Good       | Good           | Fair        | Fair           |
| 11-20 units     | Fair       | Fair           | Poor        | Poor           |
| 20+ units       | Poor       | Poor           | Very Poor   | Very Poor      |

#### Memory Usage

- **Sequential**: O(n x path_length) for path storage
- **Priority-Based**: O(n x path_length + n log n) for sorting
- **Cooperative**: O(n x path_length x attempts) for multiple attempts
- **Wait-and-Retry**: O(n x path_length x retry_factor) for temporal states

## 🔧 Integration Guide

### Integration with PathAnimator

```cpp
#include "MultiUnitPathFinder/MultiUnitPathFinder.h"
#include "PathAnimator/PathAnimator.h"

void animateMultiUnitCoordination(const std::vector<std::vector<int>>& grid) {
    // Setup multi-unit scenario
    MultiUnitPathFinder coordinator;
    coordinator.loadMapFromGrid(grid);
    coordinator.autoSetupUnitsFromMap();

    coordinator.setConflictResolutionStrategy(
        ConflictResolutionStrategy::PRIORITY_BASED
    );

    auto result = coordinator.findPathsForAllUnits();

    if (result.allPathsFound) {
        // Setup animation
        PathAnimator animator;
        MultiUnitAnimationConfig config;
        config.style = AnimationStyle::WITH_TRAIL;
        config.speed = AnimationSpeed::NORMAL;
        config.showUnitIds = true;
        config.highlightCollisions = true;
        animator.setMultiUnitConfig(config);

        // Animate the coordinated paths
        animator.animateMultiUnitPaths(coordinator.getBattleMap(), result);
    } else {
        // Handle partial results
        int successfulUnits = 0;
        for (const auto& unit : result.units) {
            if (unit.pathFound) successfulUnits++;
        }

        std::cout << successfulUnits << "/" << result.units.size()
                  << " units found paths" << std::endl;

        // Create partial result for animation
        PathfindingResult partialResult;
        for (const auto& unit : result.units) {
            if (unit.pathFound) {
                partialResult.units.push_back(unit);
            }
        }

        if (!partialResult.units.empty()) {
            partialResult.stepByStepPositions =
                MultiUnitPathFinder::generateStepByStepPositions(partialResult.units);

            PathAnimator animator;
            animator.animatePartialMultiUnitPaths(coordinator.getBattleMap(), partialResult);
        }
    }
}
```

### Integration with Game Engines

```cpp
class GameMultiUnitManager {
private:
    MultiUnitPathFinder pathfinder;
    std::map<int, GameUnit*> gameUnits;

public:
    bool initialize(const GameMap& map) {
        auto grid = convertGameMapToGrid(map);
        return pathfinder.loadMapFromGrid(grid);
    }

    void addGameUnit(GameUnit* unit, int priority = 50) {
        Position start = convertToPosition(unit->getPosition());
        Position target = convertToPosition(unit->getTargetPosition());

        pathfinder.addUnit(unit->getId(), start, target);
        pathfinder.setUnitPriority(unit->getId(), priority);

        gameUnits[unit->getId()] = unit;
    }

    void updateUnitCoordination() {
        auto result = pathfinder.findPathsForAllUnits();

        // Update game units with new paths
        for (const auto& unit : result.units) {
            auto gameUnit = gameUnits[unit.id];
            if (gameUnit && unit.pathFound) {
                auto gamePath = convertToGamePath(unit.path);
                gameUnit->setPath(gamePath);
            }
        }
    }

    void setCoordinationStrategy(ConflictResolutionStrategy strategy) {
        pathfinder.setConflictResolutionStrategy(strategy);
        updateUnitCoordination();  // Recalculate with new strategy
    }
};
```

## 🎯 Best Practices

### Strategy Selection Guidelines

1. **Use Sequential** when:

   - Units have clear hierarchy/importance order
   - Simple scenarios with few units
   - Performance is critical

2. **Use Priority-Based** when:

   - Units have different tactical importance
   - Some units are more critical than others
   - You need predictable behavior for important units

3. **Use Cooperative** when:

   - All units are equally important
   - Path quality for all units matters
   - You can afford higher computational cost

4. **Use Wait-and-Retry** when:
   - Units can afford to wait
   - Temporal flexibility is available
   - Other strategies fail due to conflicts

### Performance Optimization

```cpp
// Cache pathfinder instances for different strategies
class StrategyManager {
    std::map<ConflictResolutionStrategy, std::unique_ptr<MultiUnitPathFinder>> cache;

public:
    MultiUnitPathFinder* getPathfinder(ConflictResolutionStrategy strategy) {
        auto it = cache.find(strategy);
        if (it == cache.end()) {
            cache[strategy] = std::make_unique<MultiUnitPathFinder>();
            cache[strategy]->setConflictResolutionStrategy(strategy);
        }
        return cache[strategy].get();
    }
};

// Pre-validate unit setups
bool validateUnitSetup(const MultiUnitPathFinder& pathfinder) {
    if (pathfinder.getUnitCount() == 0) {
        std::cerr << "No units configured" << std::endl;
        return false;
    }

    if (!pathfinder.isMapLoaded()) {
        std::cerr << "Map not loaded" << std::endl;
        return false;
    }

    return true;
}

// Limit unit count for performance
const int MAX_UNITS = 15;
if (pathfinder.getUnitCount() > MAX_UNITS) {
    std::cerr << "Warning: " << pathfinder.getUnitCount()
              << " units may cause performance issues" << std::endl;
}
```

### Debugging Multi-Unit Issues

```cpp
void debugMultiUnitPathfinding(MultiUnitPathFinder& pathfinder) {
    // Display unit configuration
    pathfinder.displayUnits();

    // Test each strategy
    std::vector<ConflictResolutionStrategy> strategies = {
        ConflictResolutionStrategy::SEQUENTIAL,
        ConflictResolutionStrategy::PRIORITY_BASED,
        ConflictResolutionStrategy::COOPERATIVE,
        ConflictResolutionStrategy::WAIT_AND_RETRY
    };

    for (auto strategy : strategies) {
        pathfinder.setConflictResolutionStrategy(strategy);
        auto result = pathfinder.findPathsForAllUnits();

        std::cout << "Strategy " << static_cast<int>(strategy) << ": ";
        if (result.allPathsFound) {
            std::cout << "SUCCESS - All paths found" << std::endl;

            auto conflicts = MultiUnitPathFinder::findCollisions(result.stepByStepPositions);
            if (!conflicts.empty()) {
                std::cout << "  Warning: " << conflicts.size() << " conflicts detected" << std::endl;
            }
        } else {
            int successful = 0;
            for (const auto& unit : result.units) {
                if (unit.pathFound) successful++;
            }
            std::cout << "PARTIAL - " << successful << "/" << result.units.size()
                      << " units found paths" << std::endl;
        }
    }
}
```

## 🔍 Troubleshooting

### Common Issues

#### No Paths Found for Any Units

```cpp
// Check basic pathfinding capability
MultiUnitPathFinder debugPathfinder;
debugPathfinder.loadMapFromGrid(grid);

// Test individual units with single-unit pathfinding
for (const auto& unit : units) {
    auto singlePath = debugPathfinder.findPathAStar(unit.startPos, unit.targetPos);
    if (singlePath.empty()) {
        std::cerr << "Unit " << unit.id << " has no valid path individually" << std::endl;
        // Check start and target reachability
    }
}
```

#### Excessive Conflicts

```cpp
auto result = pathfinder.findPathsForAllUnits();
auto conflicts = MultiUnitPathFinder::findCollisions(result.stepByStepPositions);

if (conflicts.size() > result.units.size() * 0.1) {  // More than 10% conflict rate
    std::cout << "High conflict rate detected. Consider:" << std::endl;
    std::cout << "- Using WAIT_AND_RETRY strategy" << std::endl;
    std::cout << "- Reducing unit count" << std::endl;
    std::cout << "- Adjusting start/target positions" << std::endl;
}
```

#### Performance Degradation

```cpp
// Monitor pathfinding time
auto start = std::chrono::high_resolution_clock::now();
auto result = pathfinder.findPathsForAllUnits();
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

if (duration.count() > 1000) {  // More than 1 second
    std::cout << "Pathfinding taking too long (" << duration.count() << "ms)" << std::endl;
    std::cout << "Consider reducing unit count or using simpler strategy" << std::endl;
}
```

### Debug Utilities

```cpp
// Comprehensive debugging function
void debugMultiUnitScenario(MultiUnitPathFinder& pathfinder) {
    std::cout << "=== Multi-Unit Debug Information ===" << std::endl;

    // Basic info
    std::cout << "Units: " << pathfinder.getUnitCount() << std::endl;
    std::cout << "Map loaded: " << (pathfinder.isMapLoaded() ? "Yes" : "No") << std::endl;

    if (pathfinder.isMapLoaded()) {
        const auto& map = pathfinder.getBattleMap();
        std::cout << "Map size: " << map.width << "x" << map.height << std::endl;
    }

    // Unit details
    pathfinder.displayUnits();

    // Strategy performance
    auto strategies = {
        ConflictResolutionStrategy::SEQUENTIAL,
        ConflictResolutionStrategy::PRIORITY_BASED,
        ConflictResolutionStrategy::COOPERATIVE,
        ConflictResolutionStrategy::WAIT_AND_RETRY
    };

    for (auto strategy : strategies) {
        pathfinder.setConflictResolutionStrategy(strategy);

        auto start = std::chrono::high_resolution_clock::now();
        auto result = pathfinder.findPathsForAllUnits();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        std::cout << "Strategy " << static_cast<int>(strategy)
                  << ": " << (result.allPathsFound ? "SUCCESS" : "FAILED")
                  << " (" << duration.count() << " μs)" << std::endl;
    }
}
```

---
