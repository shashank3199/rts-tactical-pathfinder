/**
 * @file main.cpp
 * @brief RTS Battle Unit Pathfinding Solver with Multi-Unit Support
 * @author Shashank Goyal
 * @date 2025
 * @version 2.0
 *
 * This is the main pathfinding application that provides comprehensive pathfinding
 * capabilities for battle units in RTS-style games. It supports:
 * - Multiple pathfinding algorithms (A*, BFS, DFS)
 * - Configurable movement direction orders
 * - Multi-unit pathfinding with conflict resolution strategies
 * - Real-time path animation with various styles
 * - Performance benchmarking and algorithm comparison
 *
 * The application can operate in two modes:
 * 1. Single-unit pathfinding mode (default)
 * 2. Multi-unit pathfinding mode (with --multi-unit flag)
 *
 * @see PathFinder, MultiUnitPathFinder, PathAnimator
 * @copyright Copyright (c) 2025. All rights reserved.
 */

#include "MapLoader/MapLoader.h"
#include "PathFinder/PathFinder.h"
#include "PathAnimator/PathAnimator.h"
#include "MultiUnitPathFinder/MultiUnitPathFinder.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <algorithm>

void printUsage(const char *programName)
{
    std::cout << "Usage: " << programName << " <battle_map.json> [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --algorithm ALGO    - Pathfinding algorithm (astar, bfs, dfs, all) **SINGLE UNIT ONLY**" << std::endl;
    std::cout << "  --move-order ORDER  - Move direction order (e.g., rdlu, uldr, ldru) **BFS and DFS ONLY**" << std::endl;
    std::cout << "  --multi-unit        - Enable multi-unit pathfinding mode" << std::endl;
    std::cout << "  --strategy STRAT    - Multi-unit strategy (sequential, priority, cooperative, wait)" << std::endl;
    std::cout << "  --animate           - Animate the path after finding it" << std::endl;
    std::cout << "  --step-by-step      - Step-by-step animation (manual control)" << std::endl;
    std::cout << "  --no-animation      - Skip animation (default)" << std::endl;
    std::cout << "  --speed SPEED       - Animation speed (very_slow, slow, normal, fast, very_fast)" << std::endl;
    std::cout << "  --style STYLE       - Animation style (simple, trail, numbered, highlight) **SINGLE UNIT ONLY** " << std::endl;
    std::cout << "  --help or -h        - Show this help message" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " battle_map.json --algorithm astar --move-order uldr --animate --speed fast" << std::endl;
    std::cout << "  " << programName << " battle_map.json --multi-unit --strategy priority --animate --style trail --speed 200" << std::endl;
}

void demonstrateMoveOrders(PathFinder &pathfinder, const std::string &selectedAlgorithm)
{
    std::cout << "\n"
              << std::string(60, '=') << std::endl;
    std::cout << "DEMONSTRATING DIFFERENT MOVE ORDERS" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    std::vector<std::string> moveOrders = {"rdlu", "uldr", "ldru", "dlur"};

    for (const auto &order : moveOrders)
    {
        std::cout << "\n--- Move Order: " << order << " ---" << std::endl;

        if (!pathfinder.setMoveOrder(order))
        {
            std::cout << "Failed to set move order: " << order << std::endl;
            continue;
        }

        pathfinder.printMoveOrder();

        std::vector<Position> path;
        auto start = std::chrono::high_resolution_clock::now();

        if (selectedAlgorithm == "astar")
            path = pathfinder.findPathAStar();
        else if (selectedAlgorithm == "bfs")
            path = pathfinder.findPathBFS();
        else if (selectedAlgorithm == "dfs")
            path = pathfinder.findPathDFS();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        if (!path.empty())
        {
            std::cout << "Path found: " << path.size() << " steps, "
                      << duration.count() << " microseconds" << std::endl;
        }
        else
        {
            std::cout << "No path found" << std::endl;
        }
    }
}

bool setupMultiUnitScenario(MultiUnitPathFinder &multiPathfinder)
{
    std::cout << "\n=== Setting up Multi-Unit Scenario ===\n";

    // Try to auto-setup units from map positions
    if (multiPathfinder.autoSetupUnitsFromMap())
    {
        std::cout << "Successfully auto-detected units from map positions!\n";
        multiPathfinder.displayUnits();
        return true;
    }

    std::cout << "Failed to auto-detect units from map positions. Please check the map data." << std::endl;
    return false;
}

ConflictResolutionStrategy parseStrategy(const std::string &strategyStr)
{
    if (strategyStr == "sequential")
        return ConflictResolutionStrategy::SEQUENTIAL;
    else if (strategyStr == "priority")
        return ConflictResolutionStrategy::PRIORITY_BASED;
    else if (strategyStr == "cooperative")
        return ConflictResolutionStrategy::COOPERATIVE;
    else if (strategyStr == "wait")
        return ConflictResolutionStrategy::WAIT_AND_RETRY;
    else
    {
        std::cout << "Unknown strategy: " << strategyStr << ", using sequential" << std::endl;
        return ConflictResolutionStrategy::SEQUENTIAL;
    }
}

int countSuccessfulPaths(const PathfindingResult &result)
{
    int count = 0;
    for (const auto &unit : result.units)
    {
        if (unit.pathFound)
            count++;
    }
    return count;
}

PathfindingResult createPartialResult(const PathfindingResult &originalResult)
{
    PathfindingResult partialResult;
    partialResult.allPathsFound = false; // Mark as partial result

    // Copy only units that found paths
    for (const auto &unit : originalResult.units)
    {
        if (unit.pathFound)
        {
            partialResult.units.push_back(unit);
        }
    }

    if (!partialResult.units.empty())
    {
        // Generate step-by-step positions for successful units only
        partialResult.stepByStepPositions = MultiUnitPathFinder::generateStepByStepPositions(partialResult.units);
        partialResult.totalSteps = partialResult.stepByStepPositions.size();
    }

    return partialResult;
}

void displayMapWithSuccessfulPaths(const MultiUnitPathFinder &pathfinder, const PathfindingResult &result)
{
    const BattleMap &battleMap = pathfinder.getBattleMap();

    std::cout << "\n=== Battle Map with Successful Unit Paths ===\n";

    // Create a map to track which units pass through each position
    std::map<Position, std::vector<int>, PositionComparator> unitAtPosition;

    // Track all positions used by each unit that found a path
    for (const auto &unit : result.units)
    {
        if (unit.pathFound)
        {
            for (const auto &pos : unit.path)
            {
                unitAtPosition[pos].push_back(unit.id);
            }
        }
    }

    // Display the map
    for (int y = 0; y < battleMap.height; ++y)
    {
        for (int x = 0; x < battleMap.width; ++x)
        {
            Position currentPos(x, y);
            int tile = battleMap.grid[y][x];

            auto it = unitAtPosition.find(currentPos);
            if (it != unitAtPosition.end())
            {
                if (it->second.size() == 1)
                {
                    std::cout << it->second[0] << " "; // Single unit ID
                }
                else
                {
                    std::cout << "*" << it->second.size(); // Multiple units (* + count)
                }
            }
            else if (tile == -1)
            {
                std::cout << "░░"; // Reachable
            }
            else if (tile == 3)
            {
                std::cout << "▲▲"; // Elevated
            }
            else
            {
                std::cout << std::setw(2) << tile;
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void displayStepByStepForSuccessfulUnits(const PathfindingResult &result)
{
    if (result.stepByStepPositions.empty())
    {
        std::cout << "No step-by-step positions available" << std::endl;
        return;
    }

    std::cout << "\n=== Step-by-Step Unit Movements (Successful Units) ===\n";

    for (size_t timeStep = 0; timeStep < result.stepByStepPositions.size(); ++timeStep)
    {
        std::cout << "Time Step " << timeStep << ":\n";
        const auto &positions = result.stepByStepPositions[timeStep];

        for (size_t unitIndex = 0; unitIndex < positions.size() && unitIndex < result.units.size(); ++unitIndex)
        {
            std::cout << "  Unit " << result.units[unitIndex].id << ": ("
                      << positions[unitIndex].x << "," << positions[unitIndex].y << ")\n";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char *argv[])
{
    // Check for help flag first (can be anywhere in arguments)
    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--help" || std::string(argv[i]) == "-h")
        {
            printUsage(argv[0]);
            return 0;
        }
    }

    if (argc < 2)
    {
        std::cerr << "Error: Map file argument required." << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    std::string filename = argv[1];
    std::string algorithm = "astar";        // default
    std::string moveOrder = "rdlu";         // default
    std::string strategyStr = "sequential"; // default
    std::string speedStr = "normal";        // default
    std::string styleStr = "trail";         // default
    bool enableAnimation = false;
    bool stepByStepAnimation = false;
    bool multiUnitMode = false;

    // Parse command line arguments
    for (int i = 2; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--algorithm" && i + 1 < argc)
        {
            algorithm = argv[++i];
        }
        else if (arg == "--move-order" && i + 1 < argc)
        {
            moveOrder = argv[++i];
        }
        else if (arg == "--multi-unit")
        {
            multiUnitMode = true;
        }
        else if (arg == "--strategy" && i + 1 < argc)
        {
            strategyStr = argv[++i];
        }
        else if (arg == "--animate")
        {
            enableAnimation = true;
        }
        else if (arg == "--step-by-step")
        {
            enableAnimation = true;
            stepByStepAnimation = true;
        }
        else if (arg == "--no-animation")
        {
            enableAnimation = false;
        }
        else if (arg == "--speed" && i + 1 < argc)
        {
            speedStr = argv[++i];
        }
        else if (arg == "--style" && i + 1 < argc)
        {
            styleStr = argv[++i];
        }
        else if (arg == "astar" || arg == "bfs" || arg == "dfs" || arg == "all")
        {
            algorithm = arg;
        }
        else
        {
            std::cerr << "Warning: Unknown argument '" << arg << "'" << std::endl;
        }
    }

    // Parse animation settings
    AnimationStyle animationStyle = PathAnimator::parseAnimationStyle(styleStr);
    AnimationSpeed animationSpeed = PathAnimator::parseAnimationSpeed(speedStr);

    std::cout << "=== RTS Battle Unit Pathfinding Solver ===" << std::endl;
    std::cout << "Loading battle map from: " << filename << std::endl;
    std::cout << "Move order: " << moveOrder << std::endl;
    std::cout << "Multi-unit mode: " << (multiUnitMode ? "ENABLED" : "DISABLED") << std::endl;

    if (enableAnimation)
    {
        std::cout << "Animation: ENABLED (" << (stepByStepAnimation ? "step-by-step" : "automatic") << ")" << std::endl;
        std::cout << "Animation style: " << styleStr << std::endl;
        std::cout << "Animation speed: " << speedStr << " (" << static_cast<int>(animationSpeed) << "ms)" << std::endl;
    }
    else
    {
        std::cout << "Animation: DISABLED" << std::endl;
    }

    // Validate move order
    if (!PathFinder::isValidMoveOrder(moveOrder))
    {
        std::cerr << "Error: Invalid move order '" << moveOrder << "'" << std::endl;
        std::cerr << "Valid move orders should contain exactly 'r', 'd', 'l', 'u' (e.g., rdlu, uldr)" << std::endl;
        return 1;
    }

    // Step 1: Load the battle map using MapLoader
    MapLoader mapLoader;
    if (!mapLoader.loadFromFile(filename))
    {
        std::cerr << "Failed to load battle map from file: " << filename << std::endl;
        return 1;
    }

    // Step 2: Extract map data for pathfinding
    const std::vector<Layer> &layers = mapLoader.getLayers();
    if (layers.empty())
    {
        std::cerr << "Error: No layers found in the battle map" << std::endl;
        return 1;
    }

    const Layer &battleLayer = layers[0]; // Use first layer for pathfinding

    if (multiUnitMode)
    {
        // Multi-Unit Pathfinding Mode
        std::cout << "\n"
                  << std::string(60, '=') << std::endl;
        std::cout << "MULTI-UNIT PATHFINDING MODE" << std::endl;
        std::cout << std::string(60, '=') << std::endl;

        MultiUnitPathFinder multiPathfinder(moveOrder);

        // Convert layer data format for multi-unit pathfinder
        std::vector<std::vector<int>> gridData(battleLayer.height, std::vector<int>(battleLayer.width));
        for (int y = 0; y < battleLayer.height; ++y)
        {
            for (int x = 0; x < battleLayer.width; ++x)
            {
                gridData[y][x] = battleLayer.data[y * battleLayer.width + x];
            }
        }

        if (!multiPathfinder.loadMapFromGrid(gridData))
        {
            std::cerr << "Failed to initialize multi-unit pathfinder with map data" << std::endl;
            return 1;
        }

        // Setup multi-unit scenario
        bool multiSetupSuccess = setupMultiUnitScenario(multiPathfinder);
        if (!multiSetupSuccess)
        {
            std::cerr << "Failed to set up multi-unit scenario. Exiting." << std::endl;
            return 1;
        }

        // Set strategy
        ConflictResolutionStrategy strategy = parseStrategy(strategyStr);
        multiPathfinder.setConflictResolutionStrategy(strategy);

        // Display available strategies
        MultiUnitPathFinder::printConflictResolutionStrategies();

        // Find paths for all units
        PathfindingResult result = multiPathfinder.findPathsForAllUnits();

        // Display results
        multiPathfinder.displayPathfindingResult(result);

        // Show results even if not all paths were found
        int successfulPaths = countSuccessfulPaths(result);

        if (successfulPaths > 0)
        {
            std::cout << "\n=== Displaying Results for " << successfulPaths << " Successful Units ===\n";

            if (result.allPathsFound)
            {
                // All paths found - use original result
                multiPathfinder.displayMapWithAllPaths(result);

                if (enableAnimation)
                {
                    std::cout << "\n=== Multi-Unit Animation (All Units) ===\n";

                    PathAnimator animator;
                    MultiUnitAnimationConfig multiConfig;
                    multiConfig.style = animationStyle;
                    multiConfig.speed = animationSpeed;
                    multiConfig.showUnitIds = true;
                    multiConfig.showTrails = true;
                    multiConfig.highlightCollisions = true;
                    multiConfig.showProgress = true;
                    animator.setMultiUnitConfig(multiConfig);

                    // Print legend
                    animator.printMultiUnitAnimationLegend();

                    if (stepByStepAnimation)
                    {
                        animator.animateMultiUnitPathsStepByStep(multiPathfinder.getBattleMap(), result);
                    }
                    else
                    {
                        animator.animateMultiUnitPaths(multiPathfinder.getBattleMap(), result);
                    }
                }
            }
            else
            {
                // Partial paths found - show what we have
                std::cout << "Note: Showing paths for units that succeeded (partial result)\n";

                // Create a partial result with only successful units
                PathfindingResult partialResult = createPartialResult(result);

                if (!partialResult.units.empty())
                {
                    // Use display method that can handle partial results
                    displayMapWithSuccessfulPaths(multiPathfinder, partialResult);

                    if (enableAnimation)
                    {
                        std::cout << "\n=== Multi-Unit Animation (" << successfulPaths << " Units) ===\n";
                        std::cout << "Note: Animating only units that found paths\n";

                        PathAnimator animator;
                        MultiUnitAnimationConfig multiConfig;
                        multiConfig.style = animationStyle;
                        multiConfig.speed = animationSpeed;
                        multiConfig.showUnitIds = true;
                        multiConfig.showTrails = true;
                        multiConfig.highlightCollisions = false; // No collisions in partial result
                        multiConfig.showProgress = true;
                        animator.setMultiUnitConfig(multiConfig);

                        // Print legend
                        animator.printMultiUnitAnimationLegend();

                        if (stepByStepAnimation)
                        {
                            animator.animatePartialMultiUnitPathsStepByStep(multiPathfinder.getBattleMap(), partialResult);
                        }
                        else
                        {
                            animator.animatePartialMultiUnitPaths(multiPathfinder.getBattleMap(), partialResult);
                        }
                    }
                }
            }

            // Display step-by-step movements option
            std::cout << "\nDo you want to see detailed step-by-step unit movements for successful units? (y/n): ";
            char choice;
            std::cin >> choice;
            if (choice == 'y' || choice == 'Y')
            {
                if (result.allPathsFound)
                {
                    multiPathfinder.displayStepByStep(result);
                }
                else
                {
                    PathfindingResult partialResult = createPartialResult(result);
                    if (!partialResult.units.empty())
                    {
                        // Use step-by-step display that can handle partial results
                        displayStepByStepForSuccessfulUnits(partialResult);
                    }
                    else
                    {
                        std::cout << "No successful units to display step-by-step movements for." << std::endl;
                    }
                }
            }
        }
        else
        {
            std::cout << "\n=== No Successful Paths Found ===\n";
            std::cout << "None of the units could find a path to their targets.\n";
            std::cout << "Consider:\n";
            std::cout << "- Checking if start and target positions are reachable\n";
            std::cout << "- Trying a different conflict resolution strategy\n";
            std::cout << "- Verifying map connectivity\n";
        }
    }
    else
    {
        // Single-Unit Pathfinding Mode - with updated animation settings
        std::cout << "\n"
                  << std::string(60, '=') << std::endl;
        std::cout << "SINGLE-UNIT PATHFINDING MODE" << std::endl;
        std::cout << std::string(60, '=') << std::endl;

        // Step 3: Initialize PathFinder with custom move order
        PathFinder pathfinder(moveOrder);
        if (!pathfinder.loadMapFromData(battleLayer.data, battleLayer.width, battleLayer.height))
        {
            std::cerr << "Failed to initialize pathfinder with battle map data" << std::endl;
            return 1;
        }

        // Step 4: Display map information and validation
        pathfinder.displayMapInfo();
        pathfinder.validateMap();
        pathfinder.getBattleMap().displayMap();

        // Option to demonstrate different move orders
        if (algorithm == "all")
        {
            std::cout << "\nDo you want to see how different move orders affect pathfinding? (y/n): ";
            char choice;
            std::cin >> choice;
            if (choice == 'y' || choice == 'Y')
            {
                demonstrateMoveOrders(pathfinder, "astar");
            }
        }

        // Reset to original move order for main pathfinding
        pathfinder.setMoveOrder(moveOrder);

        // Step 5: Run pathfinding algorithm(s)
        if (algorithm == "all")
        {
            std::cout << "\n"
                      << std::string(60, '=') << std::endl;
            std::cout << "COMPARING ALL PATHFINDING ALGORITHMS" << std::endl;
            std::cout << std::string(60, '=') << std::endl;

            pathfinder.printMoveOrder();

            // Run A* algorithm
            std::cout << "\n--- Running A* Algorithm ---" << std::endl;
            auto start = std::chrono::high_resolution_clock::now();
            std::vector<Position> astarPath = pathfinder.findPathAStar();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout << "A* execution time: " << duration.count() << " microseconds" << std::endl;

            // Run BFS algorithm
            std::cout << "\n--- Running BFS Algorithm ---" << std::endl;
            start = std::chrono::high_resolution_clock::now();
            std::vector<Position> bfsPath = pathfinder.findPathBFS();
            end = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout << "BFS execution time: " << duration.count() << " microseconds" << std::endl;

            // Run DFS algorithm
            std::cout << "\n--- Running DFS Algorithm ---" << std::endl;
            start = std::chrono::high_resolution_clock::now();
            std::vector<Position> dfsPath = pathfinder.findPathDFS();
            end = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout << "DFS execution time: " << duration.count() << " microseconds" << std::endl;

            // Compare results
            std::cout << "\n=== Algorithm Comparison ===\n";
            std::vector<std::pair<std::string, std::vector<Position>>> results = {
                {"A*", astarPath}, {"BFS", bfsPath}, {"DFS", dfsPath}};

            for (const auto &result : results)
            {
                if (!result.second.empty())
                {
                    int length = PathFinder::calculatePathLength(result.second);
                    std::cout << result.first << " path length: " << length << " units" << std::endl;
                }
                else
                {
                    std::cout << result.first << " path length: NO PATH FOUND" << std::endl;
                }
            }

            // Choose best path for animation
            std::vector<Position> bestPath;
            if (!astarPath.empty())
                bestPath = astarPath;
            else if (!bfsPath.empty())
                bestPath = bfsPath;
            else if (!dfsPath.empty())
                bestPath = dfsPath;

            if (!bestPath.empty())
            {
                pathfinder.getBattleMap().displayMapWithPath(bestPath);

                if (enableAnimation)
                {
                    PathAnimator animator;
                    AnimationConfig config;
                    config.style = animationStyle;
                    config.speed = animationSpeed;
                    config.unitSymbol = 'U';
                    config.showProgress = true;
                    animator.setConfig(config);

                    if (stepByStepAnimation)
                    {
                        animator.animatePathStepByStep(pathfinder.getBattleMap(), bestPath);
                    }
                    else
                    {
                        animator.animatePath(pathfinder.getBattleMap(), bestPath);
                    }
                }
            }
        }
        else
        {
            // Run single algorithm
            std::cout << "\n"
                      << std::string(50, '=') << std::endl;
            std::cout << "RUNNING PATHFINDING ALGORITHM: " << algorithm << std::endl;
            std::cout << std::string(50, '=') << std::endl;

            auto start = std::chrono::high_resolution_clock::now();
            std::vector<Position> path;

            if (algorithm == "astar")
                path = pathfinder.findPathAStar();
            else if (algorithm == "bfs")
                path = pathfinder.findPathBFS();
            else if (algorithm == "dfs")
                path = pathfinder.findPathDFS();
            else
            {
                std::cerr << "Error: Unknown algorithm '" << algorithm << "'" << std::endl;
                return 1;
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;

            if (!path.empty())
            {
                PathFinder::displayPath(path);
                pathfinder.getBattleMap().displayMapWithPath(path);

                if (enableAnimation)
                {
                    PathAnimator animator;
                    AnimationConfig config;
                    config.style = animationStyle;
                    config.speed = animationSpeed;
                    config.unitSymbol = 'U';
                    config.showProgress = true;
                    animator.setConfig(config);

                    if (stepByStepAnimation)
                    {
                        animator.animatePathStepByStep(pathfinder.getBattleMap(), path);
                    }
                    else
                    {
                        animator.animatePath(pathfinder.getBattleMap(), path);
                    }
                }
            }
            else
            {
                std::cout << "No path found!" << std::endl;
            }
        }
    }

    std::cout << "\n=== Pathfinding analysis completed ===\n";
    return 0;
}