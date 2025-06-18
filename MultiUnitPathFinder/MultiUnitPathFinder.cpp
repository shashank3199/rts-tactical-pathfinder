/**
 * @file MultiUnitPathFinder.cpp
 * @brief Multi-unit pathfinding library for coordinated movement on battle maps - Implementation File
 * @author Shashank Goyal
 * @date 2025
 * @version 1.0
 *
 * This file contains the implementation of the MultiUnitPathFinder class,
 * which extends the PathFinder base class to handle pathfinding for multiple units
 * simultaneously. It provides various conflict resolution strategies to prevent
 * units from colliding while finding efficient paths to their targets.
 *
 * @copyright Copyright (c) 2025. All rights reserved.
 */

#include "MultiUnitPathFinder.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cstdlib>

MultiUnitPathFinder::MultiUnitPathFinder() : PathFinder()
{
    strategy = ConflictResolutionStrategy::SEQUENTIAL;
}

MultiUnitPathFinder::MultiUnitPathFinder(const std::string &moveOrder) : PathFinder(moveOrder)
{
    strategy = ConflictResolutionStrategy::SEQUENTIAL;
}

void MultiUnitPathFinder::addUnit(int unitId, const Position &startPos, const Position &targetPos)
{
    // Check if unit with this ID already exists
    for (auto &unit : units)
    {
        if (unit.id == unitId)
        {
            std::cerr << "Warning: Unit with ID " << unitId << " already exists. Updating positions." << std::endl;
            unit.startPos = startPos;
            unit.targetPos = targetPos;
            unit.path.clear();
            unit.pathFound = false;
            return;
        }
    }

    units.emplace_back(unitId, startPos, targetPos);
    unitPriorities[unitId] = 0; // Default priority
}

void MultiUnitPathFinder::addUnit(const Unit &unit)
{
    addUnit(unit.id, unit.startPos, unit.targetPos);
}

void MultiUnitPathFinder::removeUnit(int unitId)
{
    units.erase(std::remove_if(units.begin(), units.end(),
                               [unitId](const Unit &unit)
                               { return unit.id == unitId; }),
                units.end());
    unitPriorities.erase(unitId);
}

void MultiUnitPathFinder::clearUnits()
{
    units.clear();
    unitPriorities.clear();
    clearOccupiedPositions();
}

void MultiUnitPathFinder::setUnitPriority(int unitId, int priority)
{
    unitPriorities[unitId] = priority;
}

int MultiUnitPathFinder::getUnitPriority(int unitId) const
{
    auto it = unitPriorities.find(unitId);
    return (it != unitPriorities.end()) ? it->second : 0;
}

void MultiUnitPathFinder::setConflictResolutionStrategy(ConflictResolutionStrategy newStrategy)
{
    strategy = newStrategy;
}

ConflictResolutionStrategy MultiUnitPathFinder::getConflictResolutionStrategy() const
{
    return strategy;
}

PathfindingResult MultiUnitPathFinder::findPathsForAllUnits()
{
    if (units.empty())
    {
        std::cerr << "Error: No units to find paths for" << std::endl;
        return PathfindingResult();
    }

    if (!isMapLoaded())
    {
        std::cerr << "Error: No map loaded" << std::endl;
        return PathfindingResult();
    }

    std::cout << "\n=== Multi-Unit Pathfinding ===\n";
    std::cout << "Number of units: " << units.size() << std::endl;
    std::cout << "Strategy: ";

    PathfindingResult result;

    switch (strategy)
    {
    case ConflictResolutionStrategy::SEQUENTIAL:
        std::cout << "Sequential" << std::endl;
        result = findPathsSequential();
        break;
    case ConflictResolutionStrategy::PRIORITY_BASED:
        std::cout << "Priority-based" << std::endl;
        result = findPathsPriorityBased();
        break;
    case ConflictResolutionStrategy::COOPERATIVE:
        std::cout << "Cooperative" << std::endl;
        result = findPathsCooperative();
        break;
    case ConflictResolutionStrategy::WAIT_AND_RETRY:
        std::cout << "Wait-and-retry" << std::endl;
        result = findPathsWithWaiting();
        break;
    }

    // Generate step-by-step positions even for partial results: Check if any units found paths (not just all paths)
    bool hasAnyPath = false;
    for (const auto &unit : result.units)
    {
        if (unit.pathFound)
        {
            hasAnyPath = true;
            break;
        }
    }

    if (hasAnyPath)
    {
        result.stepByStepPositions = generateStepByStepPositions(result.units);
        result.totalSteps = result.stepByStepPositions.size();
    }

    return result;
}

std::string MultiUnitPathFinder::positionTimeKey(const Position &pos, int time) const
{
    return std::to_string(pos.x) + "," + std::to_string(pos.y) + "," + std::to_string(time);
}

std::vector<Position> MultiUnitPathFinder::reconstructPathFromNode(std::shared_ptr<PathNode> node) const
{
    std::vector<Position> path;

    while (node != nullptr)
    {
        path.push_back(node->pos);
        node = node->parent;
    }

    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<Position> MultiUnitPathFinder::findPathAStarWithOccupiedCheck(const Position &start, const Position &target)
{
    if (!isMapLoaded())
    {
        std::cerr << "Error: No battle map loaded" << std::endl;
        return {};
    }

    // Validate start and target positions
    if (!battleMap.isValidPosition(start.x, start.y) || !battleMap.isValidPosition(target.x, target.y))
    {
        std::cerr << "Error: Invalid start or target position" << std::endl;
        return {};
    }

    if (!battleMap.isReachable(start.x, start.y) || !battleMap.isReachable(target.x, target.y))
    {
        std::cerr << "Error: Start or target position is not reachable" << std::endl;
        return {};
    }

    std::priority_queue<std::shared_ptr<PathNode>, std::vector<std::shared_ptr<PathNode>>, PathNodeComparator> openSet;
    std::unordered_set<std::string> closedSet;                               // Use string key for position+time
    std::unordered_map<std::string, std::shared_ptr<PathNode>> openSetNodes; // Track nodes in open set

    // Start node
    auto startNode = std::make_shared<PathNode>(start, 0, 0.0, calculateHeuristic(start, target));

    openSet.push(startNode);
    std::string startKey = positionTimeKey(start, 0);
    openSetNodes[startKey] = startNode;

    int maxIterations = battleMap.width * battleMap.height * 100; // Prevent infinite loops
    int iterations = 0;

    while (!openSet.empty() && iterations < maxIterations)
    {
        iterations++;
        auto current = openSet.top();
        openSet.pop();

        std::string currentKey = positionTimeKey(current->pos, current->time);
        openSetNodes.erase(currentKey);

        // Check if we reached the target
        if (current->pos == target)
        {
            std::cout << "Path found after " << iterations << " iterations, "
                      << "final time: " << current->time << std::endl;
            return reconstructPathFromNode(current);
        }

        closedSet.insert(currentKey);

        // Explore neighbors
        std::vector<Position> neighbors = getNeighborsWithOccupiedCheck(current->pos, current->time, occupiedPositionsAtTime);

        for (const Position &neighbor : neighbors)
        {
            int nextTime = current->time + 1;
            std::string neighborKey = positionTimeKey(neighbor, nextTime);

            if (closedSet.find(neighborKey) != closedSet.end())
            {
                continue; // Skip if already evaluated
            }

            double tentativeGCost = current->gCost + 1.0; // Movement cost is 1

            // Check if this path to neighbor is better
            auto existingNode = openSetNodes.find(neighborKey);
            if (existingNode == openSetNodes.end())
            {
                // New node
                auto neighborNode = std::make_shared<PathNode>(
                    neighbor,
                    nextTime,
                    tentativeGCost,
                    calculateHeuristic(neighbor, target),
                    current);
                openSet.push(neighborNode);
                openSetNodes[neighborKey] = neighborNode;
            }
            else if (tentativeGCost < existingNode->second->gCost)
            {
                // Better path found
                existingNode->second->gCost = tentativeGCost;
                existingNode->second->fCost = tentativeGCost + existingNode->second->hCost;
                existingNode->second->parent = current;
            }
        }

        // Add option to wait in place (if current position is not permanently occupied)
        if (canWaitAtPosition(current->pos, current->time + 1))
        {
            int nextTime = current->time + 1;
            std::string waitKey = positionTimeKey(current->pos, nextTime);

            if (closedSet.find(waitKey) == closedSet.end())
            {
                double waitGCost = current->gCost + 1.0; // Waiting also costs 1 time unit

                auto existingWaitNode = openSetNodes.find(waitKey);
                if (existingWaitNode == openSetNodes.end())
                {
                    auto waitNode = std::make_shared<PathNode>(
                        current->pos,
                        nextTime,
                        waitGCost,
                        calculateHeuristic(current->pos, target),
                        current);
                    openSet.push(waitNode);
                    openSetNodes[waitKey] = waitNode;
                }
                else if (waitGCost < existingWaitNode->second->gCost)
                {
                    existingWaitNode->second->gCost = waitGCost;
                    existingWaitNode->second->fCost = waitGCost + existingWaitNode->second->hCost;
                    existingWaitNode->second->parent = current;
                }
            }
        }
    }

    std::cout << "No path found after " << iterations << " iterations" << std::endl;
    return {}; // No path found
}

PathfindingResult MultiUnitPathFinder::findPathsSequential()
{
    PathfindingResult result;
    result.units = units; // Copy units
    clearOccupiedPositions();

    std::cout << "Starting sequential pathfinding for " << result.units.size() << " units" << std::endl;

    for (size_t unitIndex = 0; unitIndex < result.units.size(); ++unitIndex)
    {
        auto &unit = result.units[unitIndex];
        std::cout << "\n=== Processing Unit " << unit.id << " (index " << unitIndex << ") ===" << std::endl;
        std::cout << "Start: (" << unit.startPos.x << "," << unit.startPos.y << ")" << std::endl;
        std::cout << "Target: (" << unit.targetPos.x << "," << unit.targetPos.y << ")" << std::endl;

        // Validate unit positions
        if (!battleMap.isValidPosition(unit.startPos.x, unit.startPos.y) ||
            !battleMap.isValidPosition(unit.targetPos.x, unit.targetPos.y))
        {
            std::cout << "ERROR: Invalid start or target position for Unit " << unit.id << std::endl;
            unit.pathFound = false;
            continue;
        }

        // Check if start position is reachable
        if (!battleMap.isReachable(unit.startPos.x, unit.startPos.y))
        {
            std::cout << "ERROR: Start position is not reachable for Unit " << unit.id << std::endl;
            unit.pathFound = false;
            continue;
        }

        // Check if target position is reachable
        if (!battleMap.isReachable(unit.targetPos.x, unit.targetPos.y))
        {
            std::cout << "ERROR: Target position is not reachable for Unit " << unit.id << std::endl;
            unit.pathFound = false;
            continue;
        }

        // Check if start and target are the same
        if (unit.startPos == unit.targetPos)
        {
            std::cout << "Unit " << unit.id << " is already at target position" << std::endl;
            unit.path = {unit.startPos}; // Path with just the start position
            unit.pathFound = true;
            updateOccupiedPositions(unit.path, 0);
            continue;
        }

        // Find path using A* algorithm with occupied position checking
        std::vector<Position> path = findPathAStarWithOccupiedCheck(unit.startPos, unit.targetPos);

        if (!path.empty())
        {
            unit.path = path;
            unit.pathFound = true;
            updateOccupiedPositions(path, 0);
            std::cout << "SUCCESS: Path found for Unit " << unit.id << " (" << path.size() << " steps)" << std::endl;

            // Print first few steps of the path
            std::cout << "Path preview: ";
            for (size_t i = 0; i < std::min(path.size(), size_t(5)); ++i)
            {
                std::cout << "(" << path[i].x << "," << path[i].y << ")";
                if (i < path.size() - 1)
                    std::cout << " -> ";
            }
            if (path.size() > 5)
                std::cout << " ... ";
            std::cout << std::endl;
        }
        else
        {
            unit.pathFound = false;
            std::cout << "FAILURE: No path found for Unit " << unit.id << std::endl;

            // Try fallback: regular A* without occupied position checking
            std::cout << "Trying fallback pathfinding without occupied position constraints..." << std::endl;
            std::vector<Position> fallbackPath = findPathAStar(unit.startPos, unit.targetPos);
            if (!fallbackPath.empty())
            {
                std::cout << "Fallback path exists (" << fallbackPath.size() << " steps), "
                          << "but blocked by other units" << std::endl;
            }
            else
            {
                std::cout << "No path exists between start and target positions" << std::endl;
            }
        }
    }

    // Check if all paths were found
    result.allPathsFound = true;
    int successCount = 0;
    for (const auto &unit : result.units)
    {
        if (unit.pathFound)
        {
            successCount++;
        }
        else
        {
            result.allPathsFound = false;
        }
    }

    std::cout << "\n=== Sequential Pathfinding Summary ===" << std::endl;
    std::cout << "Units processed: " << result.units.size() << std::endl;
    std::cout << "Successful paths: " << successCount << std::endl;
    std::cout << "Failed paths: " << (result.units.size() - successCount) << std::endl;
    std::cout << "All paths found: " << (result.allPathsFound ? "YES" : "NO") << std::endl;

    return result;
}

PathfindingResult MultiUnitPathFinder::findPathsPriorityBased()
{
    PathfindingResult result;
    result.units = units; // Copy units
    clearOccupiedPositions();

    // Sort units by priority (higher priority first)
    std::sort(result.units.begin(), result.units.end(),
              [this](const Unit &a, const Unit &b)
              {
                  return getUnitPriority(a.id) > getUnitPriority(b.id);
              });

    std::cout << "Unit processing order by priority:" << std::endl;
    for (const auto &unit : result.units)
    {
        std::cout << "  Unit " << unit.id << " (priority: " << getUnitPriority(unit.id) << ")" << std::endl;
    }

    // Use sequential pathfinding on the prioritized list
    // We need to temporarily replace units with sorted units for the sequential algorithm
    std::vector<Unit> originalUnits = units;
    units = result.units;
    PathfindingResult sequentialResult = findPathsSequential();
    units = originalUnits; // Restore original order

    return sequentialResult;
}

PathfindingResult MultiUnitPathFinder::findPathsCooperative()
{
    // Implemented as sequential with multiple attempts
    // @todo: Potential future improvement: could use algorithms like CBS (Conflict-Based Search)
    std::cout << "Note: Cooperative strategy currently implemented as enhanced version of sequential" << std::endl;

    PathfindingResult result;
    result.units = units;
    clearOccupiedPositions();

    int maxAttempts = 3;

    for (int attempt = 0; attempt < maxAttempts; ++attempt)
    {
        std::cout << "\nAttempt " << (attempt + 1) << "/" << maxAttempts << std::endl;

        // Try different unit ordering for each attempt
        if (attempt > 0)
        {
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(result.units.begin(), result.units.end(), g);
            clearOccupiedPositions();
        }

        bool allFound = true;
        for (auto &unit : result.units)
        {
            // Find path similar to sequential method
            BattleMap tempMap = battleMap;
            tempMap.startPos = unit.startPos;
            tempMap.targetPos = unit.targetPos;

            // Clear previous start/target markers
            for (int y = 0; y < tempMap.height; ++y)
            {
                for (int x = 0; x < tempMap.width; ++x)
                {
                    if (tempMap.grid[y][x] == 0 || tempMap.grid[y][x] == 8)
                    {
                        tempMap.grid[y][x] = -1;
                    }
                }
            }

            tempMap.grid[unit.startPos.y][unit.startPos.x] = 0;
            tempMap.grid[unit.targetPos.y][unit.targetPos.x] = 8;

            BattleMap originalMap = battleMap;
            battleMap = tempMap;

            std::vector<Position> path = findPathAStar();
            battleMap = originalMap;

            if (!path.empty())
            {
                unit.path = path;
                unit.pathFound = true;
                updateOccupiedPositions(path, 0);
            }
            else
            {
                unit.pathFound = false;
                allFound = false;
            }
        }

        if (allFound)
        {
            result.allPathsFound = true;
            break;
        }
    }

    return result;
}

PathfindingResult MultiUnitPathFinder::findPathsWithWaiting()
{
    std::cout << "Note: Wait-and-retry strategy allows units to wait in place when blocked" << std::endl;

    // Start with sequential pathfinding
    PathfindingResult result = findPathsSequential();

    // If conflicts exist, try to resolve them by adding wait steps
    if (result.allPathsFound)
    {
        auto conflicts = findCollisions(generateStepByStepPositions(result.units));

        if (!conflicts.empty())
        {
            std::cout << "Detected conflicts, attempting to resolve with wait steps..." << std::endl;

            // For each conflict, try to add wait steps to one of the conflicting units
            for (const auto &conflict : conflicts)
            {
                int timeStep = conflict.first;
                int unitIndex = conflict.second;

                if (unitIndex < static_cast<int>(result.units.size()) && timeStep > 0)
                {
                    // Add a wait step by duplicating the previous position
                    auto &path = result.units[unitIndex].path;
                    if (timeStep < static_cast<int>(path.size()))
                    {
                        path.insert(path.begin() + timeStep, path[timeStep - 1]);
                    }
                }
            }
        }
    }

    return result;
}

bool MultiUnitPathFinder::hasConflict(const std::vector<Position> &path1, const std::vector<Position> &path2) const
{
    size_t maxSteps = std::max(path1.size(), path2.size());

    for (size_t step = 0; step < maxSteps; ++step)
    {
        Position pos1 = (step < path1.size()) ? path1[step] : path1.back();
        Position pos2 = (step < path2.size()) ? path2[step] : path2.back();

        if (pos1 == pos2)
        {
            return true; // Collision at this time step
        }
    }

    return false;
}

bool MultiUnitPathFinder::hasTemporalConflict(const std::vector<Position> &newPath, int startTime) const
{
    for (size_t i = 0; i < newPath.size(); ++i)
    {
        int timeStep = startTime + static_cast<int>(i);
        auto it = occupiedPositionsAtTime.find(timeStep);

        if (it != occupiedPositionsAtTime.end())
        {
            if (it->second.find(newPath[i]) != it->second.end())
            {
                return true; // Position already occupied at this time
            }
        }
    }

    return false;
}

void MultiUnitPathFinder::updateOccupiedPositions(const std::vector<Position> &path, int startTime)
{
    for (size_t i = 0; i < path.size(); ++i)
    {
        int timeStep = startTime + static_cast<int>(i);
        Position pos = path[i];

        // Validate position is within map bounds
        if (battleMap.isValidPosition(pos.x, pos.y))
        {
            occupiedPositionsAtTime[timeStep].insert(pos);
        }
        else
        {
            std::cerr << "Warning: Trying to mark invalid position ("
                      << pos.x << "," << pos.y << ") as occupied" << std::endl;
        }
    }
}

void MultiUnitPathFinder::clearOccupiedPositions()
{
    occupiedPositionsAtTime.clear();
}

std::vector<Position> MultiUnitPathFinder::addWaitSteps(const std::vector<Position> &originalPath, const std::set<int> &waitAtSteps) const
{
    std::vector<Position> newPath;

    for (size_t i = 0; i < originalPath.size(); ++i)
    {
        newPath.push_back(originalPath[i]);

        // If we should wait at this step, duplicate the current position
        if (waitAtSteps.find(static_cast<int>(i)) != waitAtSteps.end())
        {
            newPath.push_back(originalPath[i]); // Stay in place for one additional step
        }
    }

    return newPath;
}

bool MultiUnitPathFinder::canWaitAtPosition(const Position &pos, int timeStep) const
{
    // Check if position is valid and reachable
    if (!battleMap.isValidPosition(pos.x, pos.y) || !battleMap.isReachable(pos.x, pos.y))
    {
        return false;
    }

    // Check if position is occupied at the given time step
    auto it = occupiedPositionsAtTime.find(timeStep);
    return (it == occupiedPositionsAtTime.end() || it->second.find(pos) == it->second.end());
}

std::vector<std::vector<Position>> MultiUnitPathFinder::generateStepByStepPositions(const std::vector<Unit> &unitsWithPaths)
{
    std::vector<std::vector<Position>> allPaths;

    for (const auto &unit : unitsWithPaths)
    {
        if (unit.pathFound)
        {
            allPaths.push_back(unit.path);
        }
    }

    // Extend paths to same length
    if (allPaths.empty())
        return {};

    // Find the maximum path length
    size_t maxLength = 0;
    for (const auto &path : allPaths)
    {
        maxLength = std::max(maxLength, path.size());
    }

    // Extend shorter paths by keeping units at their final positions
    for (auto &path : allPaths)
    {
        if (!path.empty())
        {
            Position finalPos = path.back();
            while (path.size() < maxLength)
            {
                path.push_back(finalPos);
            }
        }
    }

    // Transpose: from paths-per-unit to positions-per-timestep
    std::vector<std::vector<Position>> stepByStep;

    if (!allPaths.empty() && !allPaths[0].empty())
    {
        for (size_t timeStep = 0; timeStep < allPaths[0].size(); ++timeStep)
        {
            std::vector<Position> positionsAtTimeStep;
            for (const auto &path : allPaths)
            {
                if (timeStep < path.size())
                {
                    positionsAtTimeStep.push_back(path[timeStep]);
                }
            }
            stepByStep.push_back(positionsAtTimeStep);
        }
    }

    return stepByStep;
}
void MultiUnitPathFinder::extendPathsToSameLength(std::vector<std::vector<Position>> &paths) const
{
    if (paths.empty())
        return;

    // Find the maximum path length
    size_t maxLength = 0;
    for (const auto &path : paths)
    {
        maxLength = std::max(maxLength, path.size());
    }

    // Extend shorter paths by keeping units at their final positions
    for (auto &path : paths)
    {
        if (!path.empty())
        {
            Position finalPos = path.back();
            while (path.size() < maxLength)
            {
                path.push_back(finalPos);
            }
        }
    }
}

std::vector<Unit> MultiUnitPathFinder::getUnits() const
{
    return units;
}

int MultiUnitPathFinder::getUnitCount() const
{
    return static_cast<int>(units.size());
}

bool MultiUnitPathFinder::validateUnitPaths(const PathfindingResult &result) const
{
    if (!result.allPathsFound)
    {
        return false;
    }

    // Check for collisions in the step-by-step positions
    auto conflicts = findCollisions(result.stepByStepPositions);
    return conflicts.empty();
}

void MultiUnitPathFinder::displayUnits() const
{
    std::cout << "\n=== Units Information ===\n";
    std::cout << "Total units: " << units.size() << std::endl;

    for (const auto &unit : units)
    {
        std::cout << "Unit " << unit.id << ":\n";
        std::cout << "  Start: (" << unit.startPos.x << "," << unit.startPos.y << ")\n";
        std::cout << "  Target: (" << unit.targetPos.x << "," << unit.targetPos.y << ")\n";
        std::cout << "  Priority: " << getUnitPriority(unit.id) << "\n";
        std::cout << "  Path found: " << (unit.pathFound ? "YES" : "NO") << "\n";
        if (unit.pathFound)
        {
            std::cout << "  Path length: " << unit.path.size() << " steps\n";
        }
        std::cout << std::endl;
    }
}

void MultiUnitPathFinder::displayPathfindingResult(const PathfindingResult &result) const
{
    std::cout << "\n=== Pathfinding Results ===\n";
    std::cout << "All paths found: " << (result.allPathsFound ? "YES" : "NO") << std::endl;
    std::cout << "Total time steps: " << result.totalSteps << std::endl;

    if (result.allPathsFound)
    {
        auto conflicts = findCollisions(result.stepByStepPositions);
        std::cout << "Conflicts detected: " << conflicts.size() << std::endl;

        if (!conflicts.empty())
        {
            std::cout << "Conflict details:" << std::endl;
            for (const auto &conflict : conflicts)
            {
                std::cout << "  Time step " << conflict.first << ": Path Intersections/Collisions" << std::endl;
            }
        }
    }

    std::cout << "\nIndividual unit results:" << std::endl;
    for (const auto &unit : result.units)
    {
        std::cout << "  Unit " << unit.id << ": ";
        if (unit.pathFound)
        {
            std::cout << "Path found (" << unit.path.size() << " steps)";
        }
        else
        {
            std::cout << "No path found";
        }
        std::cout << std::endl;
    }
}

void MultiUnitPathFinder::displayStepByStep(const PathfindingResult &result) const
{
    if (!result.allPathsFound || result.stepByStepPositions.empty())
    {
        std::cout << "No valid paths to display step-by-step" << std::endl;
        return;
    }

    std::cout << "\n=== Step-by-Step Unit Movements ===\n";

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

void MultiUnitPathFinder::displayMapWithAllPaths(const PathfindingResult &result) const
{
    if (!result.allPathsFound)
    {
        std::cout << "Cannot display map: not all paths found" << std::endl;
        return;
    }

    std::cout << "\n=== Battle Map with All Unit Paths ===\n";

    // Create a map to track which units pass through each position
    std::map<Position, std::vector<int>, PositionComparator> unitAtPosition;

    // Track all positions used by each unit
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

bool MultiUnitPathFinder::loadMapWithUnits(const std::vector<std::vector<int>> &grid, const std::vector<Unit> &mapUnits)
{
    // Load the base map without specific start/target positions
    if (!loadMapFromGrid(grid))
    {
        return false;
    }

    // Clear existing units and add the provided ones
    clearUnits();
    for (const auto &unit : mapUnits)
    {
        addUnit(unit);
    }

    std::cout << "Map loaded with " << mapUnits.size() << " units" << std::endl;
    return true;
}

bool MultiUnitPathFinder::hasCollision(const std::vector<std::vector<Position>> &stepByStepPositions, int timeStep)
{
    if (timeStep < 0 || timeStep >= static_cast<int>(stepByStepPositions.size()))
    {
        return false;
    }

    const auto &positions = stepByStepPositions[timeStep];
    std::set<Position, PositionComparator> uniquePositions;

    for (const auto &pos : positions)
    {
        if (uniquePositions.find(pos) != uniquePositions.end())
        {
            return true; // Duplicate position found
        }
        uniquePositions.insert(pos);
    }

    return false;
}

std::vector<std::pair<int, int>> MultiUnitPathFinder::findCollisions(const std::vector<std::vector<Position>> &stepByStepPositions)
{
    std::vector<std::pair<int, int>> conflicts;

    for (int timeStep = 0; timeStep < static_cast<int>(stepByStepPositions.size()); ++timeStep)
    {
        const auto &positions = stepByStepPositions[timeStep];

        for (size_t i = 0; i < positions.size(); ++i)
        {
            for (size_t j = i + 1; j < positions.size(); ++j)
            {
                if (positions[i] == positions[j])
                {
                    conflicts.emplace_back(timeStep, static_cast<int>(i)); // Simplified: just report first unit
                }
            }
        }
    }

    return conflicts;
}

bool MultiUnitPathFinder::autoSetupUnitsFromMap()
{
    if (!isMapLoaded())
    {
        std::cerr << "Error: No map loaded" << std::endl;
        return false;
    }

    clearUnits();

    const BattleMap &map = getBattleMap();
    int startCount = map.getStartPositionCount();
    int targetCount = map.getTargetPositionCount();

    std::cout << "\n=== Auto-Setup Multi-Unit Scenario ===\n";
    std::cout << "Found " << startCount << " start positions and " << targetCount << " target positions\n";

    if (startCount == 0 || targetCount == 0)
    {
        std::cerr << "Error: Need at least one start and one target position" << std::endl;
        return false;
    }

    // Validate all positions are reachable
    for (int i = 0; i < startCount; ++i)
    {
        Position start = map.getStartPosition(i);
        if (!map.isReachable(start.x, start.y))
        {
            std::cerr << "Warning: Start position " << i << " at (" << start.x << "," << start.y
                      << ") is not reachable" << std::endl;
        }
    }

    for (int i = 0; i < targetCount; ++i)
    {
        Position target = map.getTargetPosition(i);
        if (!map.isReachable(target.x, target.y))
        {
            std::cerr << "Warning: Target position " << i << " at (" << target.x << "," << target.y
                      << ") is not reachable" << std::endl;
        }
    }

    // Calculate maximum possible distance for priority calculation
    int maxPossibleDistance = map.width + map.height; // Maximum Manhattan distance on the map

    // Strategy 1: If equal number of starts and targets, pair them 1:1
    if (startCount == targetCount)
    {
        std::cout << "Creating " << startCount << " units with 1:1 start-target pairing\n";
        std::cout << "Priority allocation based on distance (shorter distance = higher priority):\n";

        for (int i = 0; i < startCount; ++i)
        {
            Position start = map.getStartPosition(i);
            Position target = map.getTargetPosition(i);

            // Skip if positions are invalid
            if (!map.isValidPosition(start.x, start.y) || !map.isValidPosition(target.x, target.y))
            {
                std::cerr << "Skipping unit " << (i + 1) << " due to invalid positions" << std::endl;
                continue;
            }

            addUnit(i + 1, start, target);

            // Calculate Manhattan distance from start to target
            int distance = std::abs(start.x - target.x) + std::abs(start.y - target.y);
            int priority = maxPossibleDistance - distance; // Shorter distance gets higher priority
            setUnitPriority(i + 1, priority);

            std::cout << "Unit " << (i + 1) << ": (" << start.x << "," << start.y
                      << ") -> (" << target.x << "," << target.y
                      << ") | Distance: " << distance << " | Priority: " << priority << std::endl;
        }
    }
    // Strategy 2: More starts than targets - multiple units per target
    else if (startCount > targetCount)
    {
        std::cout << "Creating " << startCount << " units, distributing targets\n";
        std::cout << "Priority allocation based on distance (shorter distance = higher priority):\n";

        for (int i = 0; i < startCount; ++i)
        {
            Position start = map.getStartPosition(i);
            Position target = map.getTargetPosition(i % targetCount); // Cycle through targets

            // Skip if positions are invalid
            if (!map.isValidPosition(start.x, start.y) || !map.isValidPosition(target.x, target.y))
            {
                std::cerr << "Skipping unit " << (i + 1) << " due to invalid positions" << std::endl;
                continue;
            }

            addUnit(i + 1, start, target);

            // Calculate Manhattan distance from start to target
            int distance = std::abs(start.x - target.x) + std::abs(start.y - target.y);
            int priority = maxPossibleDistance - distance; // Shorter distance gets higher priority
            setUnitPriority(i + 1, priority);

            std::cout << "Unit " << (i + 1) << ": (" << start.x << "," << start.y
                      << ") -> (" << target.x << "," << target.y
                      << ") | Distance: " << distance << " | Priority: " << priority << std::endl;
        }
    }
    // Strategy 3: More targets than starts - use first target for each start
    else // targetCount > startCount
    {
        std::cout << "Creating " << startCount << " units, using first " << startCount << " targets\n";
        std::cout << "Priority allocation based on distance (shorter distance = higher priority):\n";

        for (int i = 0; i < startCount; ++i)
        {
            Position start = map.getStartPosition(i);
            Position target = map.getTargetPosition(i);

            // Skip if positions are invalid
            if (!map.isValidPosition(start.x, start.y) || !map.isValidPosition(target.x, target.y))
            {
                std::cerr << "Skipping unit " << (i + 1) << " due to invalid positions" << std::endl;
                continue;
            }

            addUnit(i + 1, start, target);

            // Calculate Manhattan distance from start to target
            int distance = std::abs(start.x - target.x) + std::abs(start.y - target.y);
            int priority = maxPossibleDistance - distance; // Shorter distance gets higher priority
            setUnitPriority(i + 1, priority);

            std::cout << "Unit " << (i + 1) << ": (" << start.x << "," << start.y
                      << ") -> (" << target.x << "," << target.y
                      << ") | Distance: " << distance << " | Priority: " << priority << std::endl;
        }
    }

    if (getUnitCount() == 0)
    {
        std::cerr << "Error: No valid units were created" << std::endl;
        return false;
    }

    std::cout << "Auto-setup completed with " << getUnitCount() << " units\n";
    std::cout << "Priority system: Units closer to targets get higher priority for earlier pathfinding\n";
    return true;
}

void MultiUnitPathFinder::printConflictResolutionStrategies()
{
    std::cout << "\n=== Available Conflict Resolution Strategies ===\n";
    std::cout << "1. SEQUENTIAL        - Find paths one by one, avoiding previous paths\n";
    std::cout << "2. PRIORITY_BASED    - Process units by priority (higher priority first)\n";
    std::cout << "3. COOPERATIVE       - Attempt to find mutually compatible paths\n";
    std::cout << "4. WAIT_AND_RETRY    - Allow units to wait in place when blocked\n";
}