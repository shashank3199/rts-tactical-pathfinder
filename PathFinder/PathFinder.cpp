/**
 * @file PathFinder.cpp
 * @brief Core pathfinding library for battle map navigation with configurable algorithms and move orders - Implementation File
 * @author Shashank Goyal
 * @date 2025
 * @version 1.0
 *
 * This file contains the implementation of the PathFinder class,
 * which provides A*, BFS, and DFS pathfinding algorithms for tactical battle map navigation.
 * Features include configurable movement direction orders, multi-unit support,
 * and comprehensive battle map analysis.
 *
 * @copyright Copyright (c) 2025. All rights reserved.
 */

#include "PathFinder.h"
#include <iostream>
#include <algorithm>
#include <climits>
#include <cmath>
#include <iomanip>
#include <stack>
#include <queue>

// BattleMap methods
bool BattleMap::isReachable(int x, int y) const
{
    if (!isValidPosition(x, y))
        return false;
    int tile = grid[y][x];
    // Reachable: -1 (ground), 0 (start), 8 (target)
    return (tile == -1 || tile == 0 || tile == 8);
}

bool BattleMap::isValidPosition(int x, int y) const
{
    return (x >= 0 && x < width && y >= 0 && y < height);
}

void BattleMap::findAllStartAndTargetPositions()
{
    allStartPositions.clear();
    allTargetPositions.clear();

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int tile = grid[y][x];
            if (tile == 0)
            {
                allStartPositions.emplace_back(x, y);
                // Set first found as default
                if (!hasValidStart)
                {
                    startPos = Position(x, y);
                    hasValidStart = true;
                }
            }
            else if (tile == 8)
            {
                allTargetPositions.emplace_back(x, y);
                // Set first found as default
                if (!hasValidTarget)
                {
                    targetPos = Position(x, y);
                    hasValidTarget = true;
                }
            }
        }
    }

    std::cout << "Found " << allStartPositions.size() << " start positions and "
              << allTargetPositions.size() << " target positions" << std::endl;
}

Position BattleMap::getStartPosition(int index) const
{
    if (index >= 0 && index < static_cast<int>(allStartPositions.size()))
    {
        return allStartPositions[index];
    }
    return Position(-1, -1); // Invalid position
}

Position BattleMap::getTargetPosition(int index) const
{
    if (index >= 0 && index < static_cast<int>(allTargetPositions.size()))
    {
        return allTargetPositions[index];
    }
    return Position(-1, -1); // Invalid position
}

void BattleMap::displayMap() const
{
    std::cout << "\n=== Battle Map ===\n";
    std::cout << "Dimensions: " << width << "x" << height << "\n";
    std::cout << "Start positions: " << allStartPositions.size() << ", Target positions: " << allTargetPositions.size() << "\n";

    // Display all start and target positions
    for (size_t i = 0; i < allStartPositions.size(); ++i)
    {
        std::cout << "Start " << i << ": (" << allStartPositions[i].x << "," << allStartPositions[i].y << ")";
        if (i < allStartPositions.size() - 1)
            std::cout << ", ";
    }
    std::cout << "\n";

    for (size_t i = 0; i < allTargetPositions.size(); ++i)
    {
        std::cout << "Target " << i << ": (" << allTargetPositions[i].x << "," << allTargetPositions[i].y << ")";
        if (i < allTargetPositions.size() - 1)
            std::cout << ", ";
    }
    std::cout << "\n\n";

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int tile = grid[y][x];
            if (tile == 0)
            {
                std::cout << "S ";
            }
            else if (tile == 8)
            {
                std::cout << "T ";
            }
            else if (tile == -1)
            {
                std::cout << "░░";
            }
            else if (tile == 3)
            {
                std::cout << "▲▲";
            }
            else
            {
                std::cout << std::setw(2) << std::fixed << std::setprecision(0) << tile;
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void BattleMap::displayMapWithPath(const std::vector<Position> &path) const
{
    std::cout << "\n=== Battle Map with Path ===\n";
    std::cout << "Path length: " << path.size() << " steps\n\n";

    // Create a set for quick path lookup
    std::unordered_set<Position, PositionHash> pathSet(path.begin(), path.end());

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            Position current(x, y);
            int tile = grid[y][x];

            if (tile == 0)
            {
                std::cout << "S ";
            }
            else if (tile == 8)
            {
                std::cout << "T ";
            }
            else if (pathSet.find(current) != pathSet.end())
            {
                std::cout << "██"; // Path marker
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
                std::cout << std::setw(2) << std::fixed << std::setprecision(0) << tile;
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

PathFinder::PathFinder()
{
    setDefaultMoveOrder();
}

PathFinder::PathFinder(const std::string &moveOrder)
{
    if (!setMoveOrder(moveOrder))
    {
        std::cerr << "Warning: Invalid move order '" << moveOrder << "', using default (rdlu)" << std::endl;
        setDefaultMoveOrder();
    }
}

void PathFinder::setDefaultMoveOrder()
{
    currentMoveOrder = "rdlu"; // right, down, left, up
    moveDirections = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
}

bool PathFinder::parseMoveOrder(const std::string &moveOrder)
{
    if (!isValidMoveOrder(moveOrder))
    {
        return false;
    }

    moveDirections.clear();

    for (char direction : moveOrder)
    {
        switch (direction)
        {
        case 'r':
        case 'R':
            moveDirections.push_back({1, 0}); // Right
            break;
        case 'd':
        case 'D':
            moveDirections.push_back({0, 1}); // Down
            break;
        case 'l':
        case 'L':
            moveDirections.push_back({-1, 0}); // Left
            break;
        case 'u':
        case 'U':
            moveDirections.push_back({0, -1}); // Up
            break;
        default:
            return false; // Should not reach here if validation worked
        }
    }

    currentMoveOrder = moveOrder;
    return true;
}

bool PathFinder::setMoveOrder(const std::string &moveOrder)
{
    return parseMoveOrder(moveOrder);
}

std::string PathFinder::getMoveOrder() const
{
    return currentMoveOrder;
}

void PathFinder::printMoveOrder() const
{
    std::cout << "Current move order: " << currentMoveOrder << " (";
    for (size_t i = 0; i < currentMoveOrder.length(); ++i)
    {
        char dir = currentMoveOrder[i];
        switch (dir)
        {
        case 'r':
        case 'R':
            std::cout << "Right";
            break;
        case 'd':
        case 'D':
            std::cout << "Down";
            break;
        case 'l':
        case 'L':
            std::cout << "Left";
            break;
        case 'u':
        case 'U':
            std::cout << "Up";
            break;
        }
        if (i < currentMoveOrder.length() - 1)
            std::cout << ", ";
    }
    std::cout << ")" << std::endl;
}

bool PathFinder::isValidMoveOrder(const std::string &moveOrder)
{
    if (moveOrder.empty() || moveOrder.length() != 4)
    {
        return false;
    }

    // Check if all four directions are present exactly once
    bool hasRight = false, hasDown = false, hasLeft = false, hasUp = false;

    for (char c : moveOrder)
    {
        switch (c)
        {
        case 'r':
        case 'R':
            if (hasRight)
                return false; // Duplicate
            hasRight = true;
            break;
        case 'd':
        case 'D':
            if (hasDown)
                return false; // Duplicate
            hasDown = true;
            break;
        case 'l':
        case 'L':
            if (hasLeft)
                return false; // Duplicate
            hasLeft = true;
            break;
        case 'u':
        case 'U':
            if (hasUp)
                return false; // Duplicate
            hasUp = true;
            break;
        default:
            return false; // Invalid character
        }
    }

    return hasRight && hasDown && hasLeft && hasUp;
}

bool PathFinder::loadMapFromGrid(const std::vector<std::vector<int>> &grid)
{
    if (grid.empty() || grid[0].empty())
    {
        std::cerr << "Error: Empty grid provided" << std::endl;
        return false;
    }

    battleMap.height = grid.size();
    battleMap.width = grid[0].size();
    battleMap.grid = grid;
    battleMap.hasValidStart = false;
    battleMap.hasValidTarget = false;

    battleMap.findAllStartAndTargetPositions();

    if (battleMap.allStartPositions.empty())
    {
        std::cerr << "Error: No starting positions (0) found in the map" << std::endl;
        return false;
    }

    if (battleMap.allTargetPositions.empty())
    {
        std::cerr << "Error: No target positions (8) found in the map" << std::endl;
        return false;
    }

    std::cout << "Battle map loaded successfully!" << std::endl;
    return true;
}

bool PathFinder::loadMapFromData(const std::vector<int> &data, int width, int height)
{
    if (data.size() != static_cast<size_t>(width * height))
    {
        std::cerr << "Error: Data size doesn't match dimensions" << std::endl;
        return false;
    }

    // Convert flat data to 2D grid
    std::vector<std::vector<int>> grid(height, std::vector<int>(width));
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            grid[y][x] = data[y * width + x];
        }
    }

    return loadMapFromGrid(grid);
}

std::vector<Position> PathFinder::findPathAStar()
{
    return findPathAStar(battleMap.startPos, battleMap.targetPos);
}

std::vector<Position> PathFinder::findPathBFS()
{
    return findPathBFS(battleMap.startPos, battleMap.targetPos);
}

std::vector<Position> PathFinder::findPathDFS()
{
    return findPathDFS(battleMap.startPos, battleMap.targetPos);
}

std::vector<Position> PathFinder::findPathAStar(const Position &start, const Position &target)
{
    if (!isMapLoaded())
    {
        std::cerr << "Error: No battle map loaded" << std::endl;
        return {};
    }

    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, NodeComparator> openSet;
    std::unordered_set<Position, PositionHash> closedSet;
    std::unordered_set<Position, PositionHash> openSetPositions;

    // Start node
    auto startNode = std::make_shared<Node>(start, 0.0, calculateHeuristic(start, target));

    openSet.push(startNode);
    openSetPositions.insert(start);

    int nodesExplored = 0;

    while (!openSet.empty())
    {
        auto current = openSet.top();
        openSet.pop();
        openSetPositions.erase(current->pos);

        nodesExplored++;

        // Check if we reached the target
        if (current->pos == target)
        {
            return reconstructPath(current);
        }

        closedSet.insert(current->pos);

        // Explore neighbors
        std::vector<Position> neighbors = getNeighbors(current->pos);
        for (const Position &neighbor : neighbors)
        {
            if (closedSet.find(neighbor) != closedSet.end())
            {
                continue; // Skip if already evaluated
            }

            double tentativeGCost = current->gCost + 1.0; // Movement cost is 1

            // Check if this path to neighbor is better
            if (openSetPositions.find(neighbor) == openSetPositions.end())
            {
                auto neighborNode = std::make_shared<Node>(
                    neighbor,
                    tentativeGCost,
                    calculateHeuristic(neighbor, target),
                    current);
                openSet.push(neighborNode);
                openSetPositions.insert(neighbor);
            }
        }
    }

    return {}; // No path found
}

std::vector<Position> PathFinder::findPathBFS(const Position &start, const Position &target)
{
    if (!isMapLoaded())
    {
        std::cerr << "Error: No battle map loaded" << std::endl;
        return {};
    }

    std::queue<std::shared_ptr<Node>> openQueue;
    std::unordered_set<Position, PositionHash> visited;

    // Start node
    auto startNode = std::make_shared<Node>(start, 0.0, 0.0);

    openQueue.push(startNode);
    visited.insert(start);

    while (!openQueue.empty())
    {
        auto current = openQueue.front();
        openQueue.pop();

        // Check if we reached the target
        if (current->pos == target)
        {
            return reconstructPath(current);
        }

        // Explore neighbors
        std::vector<Position> neighbors = getNeighbors(current->pos);
        for (const Position &neighbor : neighbors)
        {
            if (visited.find(neighbor) == visited.end())
            {
                visited.insert(neighbor);

                auto neighborNode = std::make_shared<Node>(
                    neighbor,
                    current->gCost + 1.0,
                    0.0,
                    current);
                openQueue.push(neighborNode);
            }
        }
    }

    return {}; // No path found
}

std::vector<Position> PathFinder::findPathDFS(const Position &start, const Position &target)
{
    if (!isMapLoaded())
    {
        std::cerr << "Error: No battle map loaded" << std::endl;
        return {};
    }

    std::stack<std::vector<Position>> pathStack;
    std::unordered_set<Position, PositionHash> visited;

    // Start with initial path containing only start position
    pathStack.push({start});

    int maxPathLength = battleMap.width * battleMap.height; // Prevent infinite loops

    while (!pathStack.empty())
    {
        auto currentPath = pathStack.top();
        pathStack.pop();

        Position currentPos = currentPath.back();

        // Check if we reached the target
        if (currentPos == target)
        {
            return currentPath;
        }

        // Skip if path is too long (avoid infinite loops)
        if (currentPath.size() > static_cast<size_t>(maxPathLength))
        {
            continue;
        }

        // Mark as visited for this path
        if (visited.find(currentPos) != visited.end())
        {
            continue;
        }
        visited.insert(currentPos);

        // Explore neighbors in REVERSE ORDER for DFS stack behavior
        std::vector<Position> neighbors = getNeighbors(currentPos);
        std::reverse(neighbors.begin(), neighbors.end());

        for (const Position &neighbor : neighbors)
        {
            // Check if neighbor is already in current path (avoid cycles)
            bool inCurrentPath = false;
            for (const Position &pathPos : currentPath)
            {
                if (pathPos == neighbor)
                {
                    inCurrentPath = true;
                    break;
                }
            }

            if (!inCurrentPath)
            {
                std::vector<Position> newPath = currentPath;
                newPath.push_back(neighbor);
                pathStack.push(newPath);
            }
        }
    }

    return {}; // No path found
}

double PathFinder::calculateHeuristic(const Position &from, const Position &to) const
{
    // Manhattan distance (only horizontal and vertical movement allowed)
    return std::abs(from.x - to.x) + std::abs(from.y - to.y);
}

std::vector<Position> PathFinder::getNeighbors(const Position &pos) const
{
    std::vector<Position> neighbors;

    // Use the configured move directions instead of hardcoded ones
    for (const auto &dir : moveDirections)
    {
        int newX = pos.x + dir.first;
        int newY = pos.y + dir.second;

        if (battleMap.isReachable(newX, newY))
        {
            neighbors.emplace_back(newX, newY);
        }
    }

    return neighbors;
}

std::vector<Position> PathFinder::getNeighborsWithOccupiedCheck(const Position &pos, int currentTime,
                                                                const std::map<int, std::set<Position, PositionComparator>> &occupiedPositions) const
{
    std::vector<Position> neighbors;

    for (const auto &dir : moveDirections)
    {
        int newX = pos.x + dir.first;
        int newY = pos.y + dir.second;
        Position newPos(newX, newY);

        // First check if position is valid and reachable
        if (!battleMap.isValidPosition(newX, newY))
        {
            continue;
        }

        if (!battleMap.isReachable(newX, newY))
        {
            continue;
        }

        // Check if position is occupied at the next time step
        int nextTime = currentTime + 1;
        auto it = occupiedPositions.find(nextTime);
        bool isOccupied = (it != occupiedPositions.end() && it->second.find(newPos) != it->second.end());

        if (!isOccupied)
        {
            neighbors.push_back(newPos);
        }
    }

    return neighbors;
}

std::vector<Position> PathFinder::reconstructPath(std::shared_ptr<Node> node) const
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

bool PathFinder::isPositionInSet(const Position &pos, const std::unordered_set<Position, PositionHash> &posSet) const
{
    return posSet.find(pos) != posSet.end();
}

bool PathFinder::isMapLoaded() const
{
    return !battleMap.allStartPositions.empty() && !battleMap.allTargetPositions.empty() && !battleMap.grid.empty();
}

const BattleMap &PathFinder::getBattleMap() const
{
    return battleMap;
}

void PathFinder::displayMapInfo() const
{
    if (!isMapLoaded())
    {
        std::cerr << "Error: No battle map loaded" << std::endl;
        return;
    }

    std::cout << "\n=== Battle Map Information ===\n";
    std::cout << "Dimensions: " << battleMap.width << "x" << battleMap.height << std::endl;
    std::cout << "Start Positions (" << battleMap.allStartPositions.size() << "): ";
    for (size_t i = 0; i < battleMap.allStartPositions.size(); ++i)
    {
        std::cout << "(" << battleMap.allStartPositions[i].x << "," << battleMap.allStartPositions[i].y << ")";
        if (i < battleMap.allStartPositions.size() - 1)
            std::cout << ", ";
    }
    std::cout << std::endl;

    std::cout << "Target Positions (" << battleMap.allTargetPositions.size() << "): ";
    for (size_t i = 0; i < battleMap.allTargetPositions.size(); ++i)
    {
        std::cout << "(" << battleMap.allTargetPositions[i].x << "," << battleMap.allTargetPositions[i].y << ")";
        if (i < battleMap.allTargetPositions.size() - 1)
            std::cout << ", ";
    }
    std::cout << std::endl;

    printMoveOrder();

    // Count terrain types
    int reachable = 0, elevated = 0, start = 0, target = 0;
    for (int y = 0; y < battleMap.height; ++y)
    {
        for (int x = 0; x < battleMap.width; ++x)
        {
            int tile = battleMap.grid[y][x];
            if (tile == -1)
                reachable++;
            else if (tile == 3)
                elevated++;
            else if (tile == 0)
                start++;
            else if (tile == 8)
                target++;
        }
    }

    int totalTiles = battleMap.width * battleMap.height;
    std::cout << "Terrain Analysis:\n";
    std::cout << "  Reachable positions: " << reachable << " (" << (reachable * 100.0 / totalTiles) << "%)\n";
    std::cout << "  Elevated terrain: " << elevated << " (" << (elevated * 100.0 / totalTiles) << "%)\n";
    std::cout << "  Starting positions: " << start << "\n";
    std::cout << "  Target positions: " << target << "\n";
}

void PathFinder::validateMap() const
{
    if (!isMapLoaded())
    {
        std::cerr << "Error: No battle map loaded" << std::endl;
        return;
    }

    std::cout << "\n=== Map Validation ===\n";
    std::cout << "Start positions found: " << battleMap.allStartPositions.size() << std::endl;
    std::cout << "Target positions found: " << battleMap.allTargetPositions.size() << std::endl;

    // Check map size
    if (battleMap.width < 32 || battleMap.height < 32)
    {
        std::cout << "WARNING: Map size (" << battleMap.width << "x" << battleMap.height
                  << ") is smaller than recommended minimum (32x32)" << std::endl;
    }

    std::cout << "Map validation completed.\n";
}

bool PathFinder::validatePath(const std::vector<Position> &path, const BattleMap &map)
{
    if (path.empty())
        return false;

    // Check if all positions in path are reachable and connected
    for (size_t i = 0; i < path.size(); ++i)
    {
        const Position &pos = path[i];

        // Check if position is reachable
        if (!map.isReachable(pos.x, pos.y))
        {
            return false;
        }

        // Check if consecutive positions are adjacent (no diagonal movement)
        if (i > 0)
        {
            const Position &prev = path[i - 1];
            int dx = std::abs(pos.x - prev.x);
            int dy = std::abs(pos.y - prev.y);

            if ((dx + dy) != 1)
            { // Must be exactly one step away
                return false;
            }
        }
    }

    return true;
}

void PathFinder::displayPath(const std::vector<Position> &path)
{
    if (path.empty())
    {
        std::cout << "No path to display (empty path)" << std::endl;
        return;
    }

    std::cout << "\n=== Path Details ===\n";
    std::cout << "Path length: " << path.size() << " steps" << std::endl;
    std::cout << "Path coordinates:" << std::endl;

    for (size_t i = 0; i < path.size(); ++i)
    {
        std::cout << "  Step " << std::setw(3) << i << ": ("
                  << std::setw(2) << path[i].x << ","
                  << std::setw(2) << path[i].y << ")";

        if (i == 0)
            std::cout << " [START]";
        else if (i == path.size() - 1)
            std::cout << " [TARGET]";

        std::cout << std::endl;
    }

    int totalDistance = calculatePathLength(path);
    std::cout << "Total path distance: " << totalDistance << " units" << std::endl;
}

int PathFinder::calculatePathLength(const std::vector<Position> &path)
{
    if (path.size() <= 1)
        return 0;

    int length = 0;
    for (size_t i = 1; i < path.size(); ++i)
    {
        // Since only horizontal/vertical movement is allowed, distance is always 1
        length += 1;
    }

    return length;
}