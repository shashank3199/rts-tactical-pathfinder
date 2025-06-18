/**
 * @file PathFinder.h
 * @brief Core pathfinding library for battle map navigation with configurable algorithms and move orders - Header File
 * @author Shashank Goyal
 * @date 2025
 * @version 1.0
 *
 * This header defines the PathFinder library which provides A*, BFS, and DFS pathfinding algorithms
 * for tactical battle map navigation. Features include configurable movement direction orders,
 * multi-unit support, and comprehensive battle map analysis.
 *
 * @copyright Copyright (c) 2025. All rights reserved.
 */

#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <vector>
#include <queue>
#include <unordered_set>
#include <memory>
#include <string>
#include <map>
#include <set>
#include <functional>

/**
 * @brief Represents a 2D position on the battle map
 *
 * Simple coordinate structure for representing positions on the battle grid.
 * Supports comparison operators for use in STL containers.
 */
struct Position
{
    int x, y; ///< X and Y coordinates on the battle map

    /**
     * @brief Default constructor initializing position to (0,0)
     */
    Position() : x(0), y(0) {}

    /**
     * @brief Constructor with specific coordinates
     * @param x X-coordinate
     * @param y Y-coordinate
     */
    Position(int x, int y) : x(x), y(y) {}

    /**
     * @brief Equality comparison operator
     * @param other Position to compare with
     * @return true if positions are equal
     */
    bool operator==(const Position &other) const
    {
        return x == other.x && y == other.y;
    }

    /**
     * @brief Inequality comparison operator
     * @param other Position to compare with
     * @return true if positions are not equal
     */
    bool operator!=(const Position &other) const
    {
        return !(*this == other);
    }

    /**
     * @brief Less-than comparison operator for STL containers
     * @param other Position to compare with
     * @return true if this position is "less than" other
     */
    bool operator<(const Position &other) const
    {
        return x < other.x || (x == other.x && y < other.y);
    }
};

/**
 * @brief Hash function for Position to enable use in unordered containers
 */
struct PositionHash
{
    /**
     * @brief Hash operator for Position
     * @param pos Position to hash
     * @return Hash value
     */
    std::size_t operator()(const Position &pos) const
    {
        return std::hash<int>()(pos.x) ^ (std::hash<int>()(pos.y) << 1);
    }
};

/**
 * @brief Comparator for Position to enable use in ordered containers
 */
struct PositionComparator
{
    /**
     * @brief Comparison operator for ordered containers
     * @param a First position
     * @param b Second position
     * @return true if a < b
     */
    bool operator()(const Position &a, const Position &b) const
    {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    }
};

/**
 * @brief Represents a tactical battle map with terrain and unit positions
 *
 * The BattleMap structure contains the grid data, dimensions, and methods for
 * terrain analysis and position validation. Supports multiple start and target
 * positions for multi-unit scenarios.
 */
struct BattleMap
{
    std::vector<std::vector<int>> grid; ///< 2D grid representing the battle map terrain
    int width, height;                  ///< Map dimensions in tiles
    Position startPos;                  ///< Primary start position
    Position targetPos;                 ///< Primary target position
    bool hasValidStart;                 ///< Flag indicating if start position is valid
    bool hasValidTarget;                ///< Flag indicating if target position is valid

    // Multi-unit support
    std::vector<Position> allStartPositions;  ///< All available start positions
    std::vector<Position> allTargetPositions; ///< All available target positions

    /**
     * @brief Default constructor initializing empty map
     */
    BattleMap() : width(0), height(0), hasValidStart(false), hasValidTarget(false) {}

    /**
     * @brief Check if a position is reachable (traversable)
     * @param x X-coordinate to check
     * @param y Y-coordinate to check
     * @return true if position is reachable
     *
     * Reachable tiles include: -1 (ground), 0 (start), 8 (target)
     */
    bool isReachable(int x, int y) const;

    /**
     * @brief Check if coordinates are within map bounds
     * @param x X-coordinate to check
     * @param y Y-coordinate to check
     * @return true if position is within map bounds
     */
    bool isValidPosition(int x, int y) const;

    /**
     * @brief Display the battle map in ASCII format to console
     *
     * Shows map with symbols: S=Start, T=Target, ░░=Reachable, ▲▲=Elevated
     */
    void displayMap() const;

    /**
     * @brief Display the battle map with a highlighted path
     * @param path Vector of positions representing the path to highlight
     */
    void displayMapWithPath(const std::vector<Position> &path) const;

    /**
     * @brief Scan map and populate start/target position vectors
     *
     * Searches the entire map grid for tiles marked as start (0) and target (8)
     * positions, populating the allStartPositions and allTargetPositions vectors.
     */
    void findAllStartAndTargetPositions();

    /**
     * @brief Get the number of start positions found on the map
     * @return Number of start positions
     */
    int getStartPositionCount() const { return allStartPositions.size(); }

    /**
     * @brief Get the number of target positions found on the map
     * @return Number of target positions
     */
    int getTargetPositionCount() const { return allTargetPositions.size(); }

    /**
     * @brief Get a specific start position by index
     * @param index Index of the start position (0-based)
     * @return Position object, or (-1,-1) if index is invalid
     */
    Position getStartPosition(int index) const;

    /**
     * @brief Get a specific target position by index
     * @param index Index of the target position (0-based)
     * @return Position object, or (-1,-1) if index is invalid
     */
    Position getTargetPosition(int index) const;
};

/**
 * @brief Advanced pathfinding engine for tactical battle map navigation
 *
 * The PathFinder class provides multiple pathfinding algorithms (A*, BFS, DFS) with
 * configurable movement direction orders. Supports both single-unit pathfinding and
 * serves as base class for multi-unit scenarios.
 *
 * @par Key Features:
 * - Multiple pathfinding algorithms: A* (optimal), BFS (shortest), DFS (exploratory)
 * - Configurable movement direction orders (e.g., "rdlu", "uldr")
 * - Battle map validation and analysis
 * - Path validation and visualization
 * - Multi-unit support (via inheritance)
 *
 * @par Movement Direction Orders:
 * Movement orders control the priority of direction exploration during pathfinding:
 * - "rdlu": Right, Down, Left, Up (default)
 * - "uldr": Up, Left, Down, Right
 * - "ldru": Left, Down, Right, Up
 * - "dlur": Down, Left, Up, Right
 *
 * @par Terrain Types:
 * - -1: Reachable ground
 * - 0: Start position
 * - 8: Target position
 * - 3: Elevated/blocked terrain
 *
 * @par Usage Example:
 * @code
 * PathFinder pathfinder("uldr");  // Up-Left-Down-Right movement order
 * pathfinder.loadMapFromGrid(battleGrid);
 * std::vector<Position> path = pathfinder.findPathAStar();
 * pathfinder.displayPath(path);
 * @endcode
 */
class PathFinder
{
private:
    /**
     * @brief Internal node structure for pathfinding algorithms
     *
     * Represents a single node in the pathfinding search tree, containing
     * position, cost information, and parent linkage for path reconstruction.
     */
    struct Node
    {
        Position pos;                 ///< Position of this node
        double gCost;                 ///< Distance from start (actual cost)
        double hCost;                 ///< Heuristic distance to target (estimated cost)
        double fCost;                 ///< Total cost (g + h) for A* algorithm
        std::shared_ptr<Node> parent; ///< Parent node for path reconstruction

        /**
         * @brief Constructor for pathfinding node
         * @param position Position coordinates
         * @param g G-cost (distance from start)
         * @param h H-cost (heuristic to target)
         * @param p Parent node pointer (optional)
         */
        Node(Position position, double g, double h, std::shared_ptr<Node> p = nullptr)
            : pos(position), gCost(g), hCost(h), fCost(g + h), parent(p) {}
    };

    /**
     * @brief Comparator for priority queue in A* algorithm
     *
     * Implements min-heap behavior based on f-cost, with h-cost as tiebreaker.
     */
    struct NodeComparator
    {
        /**
         * @brief Comparison operator for priority queue
         * @param a First node
         * @param b Second node
         * @return true if a should be ordered after b in priority queue
         */
        bool operator()(const std::shared_ptr<Node> &a, const std::shared_ptr<Node> &b) const
        {
            if (a->fCost == b->fCost)
            {
                return a->hCost > b->hCost; // Prefer lower heuristic if f-costs are equal
            }
            return a->fCost > b->fCost; // Min-heap based on f-cost
        }
    };

protected:
    BattleMap battleMap;                             ///< The loaded battle map
    std::vector<std::pair<int, int>> moveDirections; ///< Current movement direction order
    std::string currentMoveOrder;                    ///< String representation of move order

    /**
     * @brief Calculate Manhattan distance heuristic
     * @param from Starting position
     * @param to Target position
     * @return Manhattan distance between positions
     *
     * Uses Manhattan distance since only orthogonal movement is allowed.
     */
    double calculateHeuristic(const Position &from, const Position &to) const;

    /**
     * @brief Get valid neighboring positions from current position
     * @param pos Current position
     * @return Vector of valid neighbor positions
     *
     * Returns neighbors in the order specified by current move direction order.
     */
    std::vector<Position> getNeighbors(const Position &pos) const;

    /**
     * @brief Get neighbors while checking for occupied positions (multi-unit support)
     * @param pos Current position
     * @param currentTime Current time step
     * @param occupiedPositions Map of time steps to occupied positions
     * @return Vector of valid, unoccupied neighbor positions
     *
     * Used by multi-unit pathfinding to avoid collisions with other units.
     */
    std::vector<Position> getNeighborsWithOccupiedCheck(
        const Position &pos,
        int currentTime,
        const std::map<int, std::set<Position, PositionComparator>> &occupiedPositions) const;

    /**
     * @brief Reconstruct path from goal node to start
     * @param node Goal node containing parent chain
     * @return Vector of positions from start to goal
     */
    std::vector<Position> reconstructPath(std::shared_ptr<Node> node) const;

    /**
     * @brief Check if position exists in unordered set
     * @param pos Position to check
     * @param posSet Set to search in
     * @return true if position is found in set
     */
    bool isPositionInSet(const Position &pos, const std::unordered_set<Position, PositionHash> &posSet) const;

    /**
     * @brief Parse and validate movement order string
     * @param moveOrder String containing direction characters (r,d,l,u)
     * @return true if move order is valid and applied successfully
     */
    bool parseMoveOrder(const std::string &moveOrder);

    /**
     * @brief Set default movement order to "rdlu"
     */
    void setDefaultMoveOrder();

public:
    /**
     * @brief Default constructor with standard movement order
     *
     * Initializes PathFinder with default "rdlu" movement order.
     */
    PathFinder();

    /**
     * @brief Constructor with custom movement order
     * @param moveOrder String specifying movement direction priority (e.g., "uldr")
     *
     * @warning If moveOrder is invalid, defaults to "rdlu" with warning message
     */
    PathFinder(const std::string &moveOrder);

    /**
     * @brief Load battle map from 2D grid
     * @param grid 2D vector representing the battle map
     * @return true if map loaded successfully
     *
     * Grid values: -1=reachable, 0=start, 8=target, 3=blocked
     */
    bool loadMapFromGrid(const std::vector<std::vector<int>> &grid);

    /**
     * @brief Load battle map from flat array with dimensions
     * @param data Flat array of map data
     * @param width Map width in tiles
     * @param height Map height in tiles
     * @return true if map loaded successfully
     */
    bool loadMapFromData(const std::vector<int> &data, int width, int height);

    /**
     * @brief Set movement direction order
     * @param moveOrder String with 4 unique direction characters (r,d,l,u)
     * @return true if move order is valid and applied
     *
     * Valid examples: "rdlu", "uldr", "ldru", "dlur"
     * Each character (r=right, d=down, l=left, u=up) must appear exactly once.
     */
    bool setMoveOrder(const std::string &moveOrder);

    /**
     * @brief Get current movement order string
     * @return Current movement order (e.g., "rdlu")
     */
    std::string getMoveOrder() const;

    /**
     * @brief Print current movement order to console
     *
     * Displays both the string representation and full direction names.
     */
    void printMoveOrder() const;

    /**
     * @brief A* pathfinding using default start/target positions
     * @return Vector of positions representing optimal path, empty if no path found
     *
     * Uses A* algorithm with Manhattan distance heuristic for optimal pathfinding.
     * Requires map to be loaded with valid start and target positions.
     */
    std::vector<Position> findPathAStar();

    /**
     * @brief Breadth-First Search pathfinding using default positions
     * @return Vector of positions representing shortest path, empty if no path found
     *
     * Guarantees shortest path but may be slower than A* on large maps.
     */
    std::vector<Position> findPathBFS();

    /**
     * @brief Depth-First Search pathfinding using default positions
     * @return Vector of positions representing a path, empty if no path found
     *
     * @warning Does not guarantee optimal path. Primarily for testing and comparison.
     */
    std::vector<Position> findPathDFS();

    /**
     * @brief A* pathfinding with custom start and target positions
     * @param start Starting position
     * @param target Target position
     * @return Vector of positions representing optimal path, empty if no path found
     */
    std::vector<Position> findPathAStar(const Position &start, const Position &target);

    /**
     * @brief BFS pathfinding with custom start and target positions
     * @param start Starting position
     * @param target Target position
     * @return Vector of positions representing shortest path, empty if no path found
     */
    std::vector<Position> findPathBFS(const Position &start, const Position &target);

    /**
     * @brief DFS pathfinding with custom start and target positions
     * @param start Starting position
     * @param target Target position
     * @return Vector of positions representing a path, empty if no path found
     */
    std::vector<Position> findPathDFS(const Position &start, const Position &target);

    /**
     * @brief Check if a battle map is currently loaded
     * @return true if map is loaded and valid
     */
    bool isMapLoaded() const;

    /**
     * @brief Get reference to the loaded battle map
     * @return Const reference to BattleMap object
     */
    const BattleMap &getBattleMap() const;

    /**
     * @brief Display comprehensive map information to console
     *
     * Shows dimensions, start/target positions, movement order, and terrain analysis.
     */
    void displayMapInfo() const;

    /**
     * @brief Validate loaded map for pathfinding requirements
     *
     * Checks for presence of start/target positions and adequate map size.
     */
    void validateMap() const;

    /**
     * @brief Validate that a path is valid for the given map
     * @param path Vector of positions to validate
     * @param map Battle map to validate against
     * @return true if path is valid (all positions reachable and connected)
     *
     * Checks that all positions are reachable and consecutive positions are adjacent.
     */
    static bool validatePath(const std::vector<Position> &path, const BattleMap &map);

    /**
     * @brief Display detailed path information to console
     * @param path Vector of positions representing the path
     *
     * Shows step-by-step coordinates and total path length.
     */
    static void displayPath(const std::vector<Position> &path);

    /**
     * @brief Calculate total length of a path
     * @param path Vector of positions
     * @return Length of path in movement units
     *
     * Since only orthogonal movement is allowed, each step has cost 1.
     */
    static int calculatePathLength(const std::vector<Position> &path);

    /**
     * @brief Validate movement order string format
     * @param moveOrder String to validate
     * @return true if moveOrder contains exactly one each of 'r', 'd', 'l', 'u'
     *
     * Valid examples: "rdlu", "uldr", "LDRU" (case insensitive)
     */
    static bool isValidMoveOrder(const std::string &moveOrder);
};

#endif // PATHFINDER_H