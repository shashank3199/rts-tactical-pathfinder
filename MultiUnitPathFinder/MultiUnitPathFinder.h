/**
 * @file MultiUnitPathFinder.h
 * @brief Multi-unit pathfinding library for coordinated movement on battle maps - Header File
 * @author Shashank Goyal
 * @date 2025
 * @version 1.0
 *
 * This header defines the MultiUnitPathFinder class and related structures for
 * handling pathfinding of multiple units simultaneously while avoiding collisions.
 * The library extends the PathFinder base class with sophisticated conflict
 * resolution strategies and temporal pathfinding capabilities.
 *
 * @copyright Copyright (c) 2025. All rights reserved.
 */

#ifndef MULTIUNITPATHFINDER_H
#define MULTIUNITPATHFINDER_H

#include "../PathFinder/PathFinder.h"
#include <map>
#include <set>
#include <functional>
#include <unordered_set>
#include <string>
#include <memory>
#include <unordered_map>
#include <random>

//==============================================================================
// FORWARD DECLARATIONS AND ENUMERATIONS
//==============================================================================

/**
 * @enum ConflictResolutionStrategy
 * @brief Defines different strategies for resolving conflicts between multiple units
 *
 * These strategies determine how the pathfinder handles situations where
 * multiple units might collide or interfere with each other's paths.
 */
enum class ConflictResolutionStrategy
{
    SEQUENTIAL,     ///< Find paths sequentially, later units avoid earlier paths
    PRIORITY_BASED, ///< Higher priority units get preference in pathfinding
    COOPERATIVE,    ///< Try to find mutually non-conflicting paths through multiple attempts
    WAIT_AND_RETRY  ///< Units can wait in place when blocked by other units
};

//==============================================================================
// DATA STRUCTURES
//==============================================================================

/**
 * @struct Unit
 * @brief Represents a single unit in multi-unit pathfinding
 *
 * Contains all information needed to track a unit including its identifier,
 * start and target positions, calculated path, and pathfinding status.
 */
struct Unit
{
    int id;                     ///< Unique unit identifier
    Position startPos;          ///< Starting position on the map
    Position targetPos;         ///< Target position on the map
    std::vector<Position> path; ///< Calculated path from start to target
    bool pathFound;             ///< Whether a valid path was found for this unit

    /**
     * @brief Default constructor
     *
     * Initializes a unit with invalid ID and no path found.
     */
    Unit() : id(-1), pathFound(false) {}

    /**
     * @brief Parameterized constructor
     * @param unitId Unique identifier for the unit
     * @param start Starting position
     * @param target Target position
     */
    Unit(int unitId, const Position &start, const Position &target)
        : id(unitId), startPos(start), targetPos(target), pathFound(false) {}
};

/**
 * @struct PathfindingResult
 * @brief Contains the complete results of a multi-unit pathfinding operation
 *
 * This structure encapsulates all the information about the pathfinding
 * results including individual unit paths, success status, and step-by-step
 * movement data for animation and analysis.
 */
struct PathfindingResult
{
    std::vector<Unit> units;                                ///< All units with their calculated paths
    bool allPathsFound;                                     ///< True if all units found valid paths
    int totalSteps;                                         ///< Total number of time steps required
    std::vector<std::vector<Position>> stepByStepPositions; ///< Unit positions at each time step

    /**
     * @brief Default constructor
     *
     * Initializes result with no paths found and zero total steps.
     */
    PathfindingResult() : allPathsFound(false), totalSteps(0) {}
};

//==============================================================================
// MAIN CLASS DECLARATION
//==============================================================================

/**
 * @class MultiUnitPathFinder
 * @brief Advanced pathfinding class for multiple units with conflict resolution
 *
 * The MultiUnitPathFinder extends the PathFinder base class to handle multiple
 * units simultaneously. It provides various conflict resolution strategies to
 * prevent units from colliding while finding efficient paths to their targets.
 *
 * Key features:
 * - Multiple conflict resolution strategies
 * - Temporal pathfinding with time-aware collision avoidance
 * - Priority-based unit processing
 * - Automatic unit setup from map data
 * - Integration with animation systems
 * - Comprehensive debugging and visualization tools
 *
 * @see PathFinder
 * @see PathAnimator
 */
class MultiUnitPathFinder : public PathFinder
{
private:
    //==========================================================================
    // INTERNAL DATA STRUCTURES
    //==========================================================================

    /**
     * @struct PathNode
     * @brief Internal node structure for temporal A* pathfinding
     *
     * Extends the basic pathfinding node to include time information,
     * enabling time-aware pathfinding that considers when units will
     * be at specific positions.
     */
    struct PathNode
    {
        Position pos;                     ///< Position in the map
        int time;                         ///< Time step when unit reaches this position
        double gCost;                     ///< Distance from start (actual cost)
        double hCost;                     ///< Heuristic distance to target
        double fCost;                     ///< Total cost (g + h)
        std::shared_ptr<PathNode> parent; ///< Parent node for path reconstruction

        /**
         * @brief PathNode constructor
         * @param position Map position
         * @param t Time step
         * @param g Actual cost from start
         * @param h Heuristic cost to target
         * @param p Parent node (optional)
         */
        PathNode(Position position, int t, double g, double h, std::shared_ptr<PathNode> p = nullptr)
            : pos(position), time(t), gCost(g), hCost(h), fCost(g + h), parent(p) {}
    };

    /**
     * @struct PathNodeComparator
     * @brief Comparator for PathNode priority queue
     *
     * Provides comparison logic for the A* algorithm priority queue,
     * prioritizing nodes with lower f-cost and using h-cost as a tiebreaker.
     */
    struct PathNodeComparator
    {
        /**
         * @brief Compare two PathNode pointers
         * @param a First node
         * @param b Second node
         * @return true if a has higher priority than b
         */
        bool operator()(const std::shared_ptr<PathNode> &a, const std::shared_ptr<PathNode> &b) const
        {
            if (std::abs(a->fCost - b->fCost) < 1e-9) // Use epsilon comparison for doubles
            {
                return a->hCost > b->hCost; // Prefer lower heuristic if f-costs are equal
            }
            return a->fCost > b->fCost; // Min-heap based on f-cost
        }
    };

    //==========================================================================
    // MEMBER VARIABLES
    //==========================================================================

    std::vector<Unit> units;             ///< Collection of all units to pathfind
    ConflictResolutionStrategy strategy; ///< Current conflict resolution strategy
    std::map<int, int> unitPriorities;   ///< Unit ID to priority mapping

    /// Temporal conflict tracking: time_step -> set of occupied positions
    std::map<int, std::set<Position, PositionComparator>> occupiedPositionsAtTime;

    //==========================================================================
    // PRIVATE HELPER METHODS
    //==========================================================================

    /**
     * @brief Check if two paths have any conflicts
     * @param path1 First unit's path
     * @param path2 Second unit's path
     * @return true if paths conflict at any time step
     */
    bool hasConflict(const std::vector<Position> &path1, const std::vector<Position> &path2) const;

    /**
     * @brief Check if a new path conflicts with existing occupied positions
     * @param newPath Path to check for conflicts
     * @param startTime Starting time for the path
     * @return true if path conflicts with occupied positions
     */
    bool hasTemporalConflict(const std::vector<Position> &newPath, int startTime) const;

    /**
     * @brief Update occupied positions with a unit's path
     * @param path Unit's path to add to occupied positions
     * @param startTime Starting time for the path
     */
    void updateOccupiedPositions(const std::vector<Position> &path, int startTime);

    /**
     * @brief Clear all occupied position data
     */
    void clearOccupiedPositions();

    /**
     * @brief Sequential pathfinding strategy implementation
     * @return Pathfinding results for all units
     * @details Processes units one by one, with each subsequent unit avoiding
     *          the paths of previously processed units.
     */
    PathfindingResult findPathsSequential();

    /**
     * @brief Priority-based pathfinding strategy implementation
     * @return Pathfinding results for all units
     * @details Sorts units by priority and then uses sequential pathfinding,
     *          ensuring higher priority units get better paths.
     */
    PathfindingResult findPathsPriorityBased();

    /**
     * @brief Cooperative pathfinding strategy implementation
     * @return Pathfinding results for all units
     * @details Attempts multiple different unit orderings to find mutually
     *          compatible paths for all units.
     */
    PathfindingResult findPathsCooperative();

    /**
     * @brief Wait-and-retry pathfinding strategy implementation
     * @return Pathfinding results for all units
     * @details Allows units to wait in place when blocked, potentially
     *          resolving conflicts through temporal coordination.
     */
    PathfindingResult findPathsWithWaiting();

    /**
     * @brief Add wait steps to a path
     * @param originalPath Original path without wait steps
     * @param waitAtSteps Set of step indices where unit should wait
     * @return Modified path with wait steps inserted
     */
    std::vector<Position> addWaitSteps(const std::vector<Position> &originalPath, const std::set<int> &waitAtSteps) const;

    /**
     * @brief Check if a unit can wait at a position at a given time
     * @param pos Position to check
     * @param timeStep Time step to check
     * @return true if position is available for waiting
     */
    bool canWaitAtPosition(const Position &pos, int timeStep) const;

    /**
     * @brief Extend all paths to the same length
     * @param paths Vector of paths to extend
     * @details Extends shorter paths by keeping units at their final positions
     */
    void extendPathsToSameLength(std::vector<std::vector<Position>> &paths) const;

    /**
     * @brief A* pathfinding with occupied position checking
     * @param start Starting position
     * @param target Target position
     * @return Path from start to target avoiding occupied positions
     * @details Uses temporal A* algorithm that considers occupied positions
     *          at different time steps to avoid collisions.
     */
    std::vector<Position> findPathAStarWithOccupiedCheck(const Position &start, const Position &target);

    /**
     * @brief Generate a unique key for position-time combination
     * @param pos Position
     * @param time Time step
     * @return String key combining position and time
     */
    std::string positionTimeKey(const Position &pos, int time) const;

    /**
     * @brief Reconstruct path from final PathNode
     * @param node Final node in the path
     * @return Complete path from start to target
     */
    std::vector<Position> reconstructPathFromNode(std::shared_ptr<PathNode> node) const;

public:
    //==========================================================================
    // CONSTRUCTORS AND DESTRUCTOR
    //==========================================================================

    /**
     * @brief Default constructor
     *
     * Initializes MultiUnitPathFinder with default settings and sequential
     * conflict resolution strategy.
     */
    MultiUnitPathFinder();

    /**
     * @brief Constructor with custom move order
     * @param moveOrder String defining movement direction order (e.g., "rdlu")
     *
     * Initializes the pathfinder with a custom movement direction order
     * which affects the path selection when multiple optimal paths exist.
     */
    MultiUnitPathFinder(const std::string &moveOrder);

    //==========================================================================
    // UNIT MANAGEMENT
    //==========================================================================

    /**
     * @brief Add a unit to the pathfinding system
     * @param unitId Unique identifier for the unit
     * @param startPos Starting position on the map
     * @param targetPos Target position on the map
     *
     * If a unit with the same ID already exists, its positions will be updated.
     */
    void addUnit(int unitId, const Position &startPos, const Position &targetPos);

    /**
     * @brief Add a unit using Unit structure
     * @param unit Unit object containing all necessary information
     */
    void addUnit(const Unit &unit);

    /**
     * @brief Remove a unit from the system
     * @param unitId ID of the unit to remove
     */
    void removeUnit(int unitId);

    /**
     * @brief Clear all units from the system
     *
     * Removes all units and clears occupied position data.
     */
    void clearUnits();

    /**
     * @brief Automatically setup units from map start and target positions
     * @return true if units were successfully created from map data
     *
     * Scans the loaded map for start positions (tile value 0) and target
     * positions (tile value 8) and automatically creates units with appropriate
     * start-target pairings.
     */
    bool autoSetupUnitsFromMap();

    //==========================================================================
    // PRIORITY MANAGEMENT
    //==========================================================================

    /**
     * @brief Set priority for a specific unit
     * @param unitId ID of the unit
     * @param priority Priority value (higher values = higher priority)
     *
     * Priorities are used in priority-based conflict resolution strategy.
     */
    void setUnitPriority(int unitId, int priority);

    /**
     * @brief Get priority for a specific unit
     * @param unitId ID of the unit
     * @return Priority value (0 if not set)
     */
    int getUnitPriority(int unitId) const;

    //==========================================================================
    // CONFIGURATION
    //==========================================================================

    /**
     * @brief Set the conflict resolution strategy
     * @param newStrategy Strategy to use for resolving unit conflicts
     */
    void setConflictResolutionStrategy(ConflictResolutionStrategy newStrategy);

    /**
     * @brief Get the current conflict resolution strategy
     * @return Currently active strategy
     */
    ConflictResolutionStrategy getConflictResolutionStrategy() const;

    //==========================================================================
    // PATHFINDING OPERATIONS
    //==========================================================================

    /**
     * @brief Find paths for all units using the current strategy
     * @return Complete pathfinding results including all unit paths
     *
     * This is the main pathfinding method that coordinates finding paths
     * for all units while avoiding conflicts using the selected strategy.
     */
    PathfindingResult findPathsForAllUnits();

    //==========================================================================
    // INFORMATION AND QUERIES
    //==========================================================================

    /**
     * @brief Get copy of all units
     * @return Vector containing all units
     */
    std::vector<Unit> getUnits() const;

    /**
     * @brief Get the number of units in the system
     * @return Count of units
     */
    int getUnitCount() const;

    /**
     * @brief Validate that unit paths don't have conflicts
     * @param result Pathfinding result to validate
     * @return true if all paths are valid and conflict-free
     */
    bool validateUnitPaths(const PathfindingResult &result) const;

    //==========================================================================
    // DISPLAY AND VISUALIZATION
    //==========================================================================

    /**
     * @brief Display information about all units
     *
     * Shows unit IDs, start/target positions, priorities, and path status.
     */
    void displayUnits() const;

    /**
     * @brief Display comprehensive pathfinding results
     * @param result Results to display
     *
     * Shows success status, conflicts, timing, and individual unit results.
     */
    void displayPathfindingResult(const PathfindingResult &result) const;

    /**
     * @brief Display step-by-step unit movements
     * @param result Results containing step-by-step data
     *
     * Shows the position of each unit at every time step.
     */
    void displayStepByStep(const PathfindingResult &result) const;

    /**
     * @brief Display map with all unit paths overlaid
     * @param result Results containing unit paths
     *
     * Shows the battle map with unit paths visualized using unit IDs
     * and conflict indicators.
     */
    void displayMapWithAllPaths(const PathfindingResult &result) const;

    //==========================================================================
    // MAP LOADING
    //==========================================================================

    /**
     * @brief Load map and units simultaneously
     * @param grid 2D grid representing the map
     * @param mapUnits Vector of units to add to the system
     * @return true if map and units were loaded successfully
     */
    bool loadMapWithUnits(const std::vector<std::vector<int>> &grid, const std::vector<Unit> &mapUnits);

    //==========================================================================
    // STATIC UTILITY METHODS
    //==========================================================================

    /**
     * @brief Check if units collide at a specific time step
     * @param stepByStepPositions Unit positions at each time step
     * @param timeStep Time step to check for collisions
     * @return true if any units occupy the same position at the given time
     */
    static bool hasCollision(const std::vector<std::vector<Position>> &stepByStepPositions, int timeStep);

    /**
     * @brief Find all collisions in step-by-step movement data
     * @param stepByStepPositions Unit positions at each time step
     * @return Vector of (time_step, unit_index) pairs indicating conflicts
     */
    static std::vector<std::pair<int, int>> findCollisions(const std::vector<std::vector<Position>> &stepByStepPositions);

    /**
     * @brief Generate step-by-step position data from unit paths
     * @param unitsWithPaths Vector of units with calculated paths
     * @return Step-by-step position data suitable for animation
     *
     * Converts individual unit paths into a time-synchronized format
     * where each time step shows the position of all units.
     */
    static std::vector<std::vector<Position>> generateStepByStepPositions(const std::vector<Unit> &unitsWithPaths);

    /**
     * @brief Print information about available conflict resolution strategies
     *
     * Displays detailed descriptions of all available strategies to help
     * users choose the most appropriate one for their use case.
     */
    static void printConflictResolutionStrategies();
};

#endif // MULTIUNITPATHFINDER_H
