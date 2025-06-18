/**
 * @file PathAnimator.h
 * @brief Terminal-based path animation library for single and multi-unit pathfinding visualization - Header File
 * @author Shashank Goyal
 * @date 2025
 * @version 1.0
 *
 * This header defines the PathAnimator class and related structures for animating
 * pathfinding results in terminal environments. Supports both single-unit and
 * multi-unit scenarios with various animation styles and speeds.
 *
 * @copyright Copyright (c) 2025. All rights reserved.
 */

#ifndef PATHANIMATOR_H
#define PATHANIMATOR_H

#include <vector>
#include <string>
#include <chrono>
#include <map>

// Forward declarations
struct Position;
struct BattleMap;
struct Unit;
struct PathfindingResult;

/**
 * @namespace Colors
 * @brief ANSI color codes for terminal output formatting
 *
 * Provides standard and bright color codes for enhancing terminal visualization
 * of animated paths, units, and terrain features.
 */
namespace Colors
{
    const int BLACK = 30;          ///< Standard black color
    const int RED = 31;            ///< Standard red color
    const int GREEN = 32;          ///< Standard green color
    const int YELLOW = 33;         ///< Standard yellow color
    const int BLUE = 34;           ///< Standard blue color
    const int MAGENTA = 35;        ///< Standard magenta color
    const int CYAN = 36;           ///< Standard cyan color
    const int WHITE = 37;          ///< Standard white color
    const int BRIGHT_GREEN = 92;   ///< Bright green color for highlights
    const int BRIGHT_YELLOW = 93;  ///< Bright yellow color for highlights
    const int BRIGHT_BLUE = 94;    ///< Bright blue color for highlights
    const int BRIGHT_MAGENTA = 95; ///< Bright magenta color for highlights
    const int BRIGHT_RED = 91;     ///< Bright red color for warnings/errors
    const int BRIGHT_CYAN = 96;    ///< Bright cyan color for highlights
}

/**
 * @enum AnimationStyle
 * @brief Defines different visual styles for path animation
 *
 * Controls how the animated path is displayed, from simple unit movement
 * to complex trail and highlighting effects.
 */
enum class AnimationStyle
{
    SIMPLE,         ///< Basic unit movement without trail effects
    WITH_TRAIL,     ///< Show path trail behind moving unit
    NUMBERED_STEPS, ///< Display step numbers on the path
    HIGHLIGHT_PATH  ///< Highlight the entire path and show unit moving along it
};

/**
 * @enum AnimationSpeed
 * @brief Predefined animation speeds in milliseconds per step
 *
 * Provides common speed settings for animations, from very slow for
 * detailed observation to very fast for quick overviews.
 */
enum class AnimationSpeed
{
    VERY_SLOW = 1000, ///< 1000ms per step - Very slow for detailed analysis
    SLOW = 500,       ///< 500ms per step - Slow pace for observation
    NORMAL = 250,     ///< 250ms per step - Standard comfortable viewing speed
    FAST = 100,       ///< 100ms per step - Fast pace for quick overview
    VERY_FAST = 50    ///< 50ms per step - Very fast for rapid demonstration
};

/**
 * @struct AnimationConfig
 * @brief Configuration settings for single-unit path animation
 *
 * Contains all parameters needed to customize the appearance and behavior
 * of single-unit path animations, including visual symbols, timing, and
 * display options.
 */
struct AnimationConfig
{
    AnimationStyle style;          ///< Visual style of the animation
    AnimationSpeed speed;          ///< Animation speed in milliseconds per step
    char unitSymbol;               ///< Character representing the moving unit
    char trailSymbol;              ///< Character used for path trail display
    char pathSymbol;               ///< Character used for path highlighting
    bool showStepNumbers;          ///< Whether to display step numbers
    bool clearScreenBetweenFrames; ///< Whether to clear screen between animation frames
    bool showProgress;             ///< Whether to display progress bar

    /**
     * @brief Default constructor with standard configuration
     *
     * Initializes animation config with commonly used defaults:
     * - Style: WITH_TRAIL for visual feedback
     * - Speed: NORMAL for comfortable viewing
     * - Unit symbol: 'U' for unit representation
     * - Trail symbol: '.' for subtle trail marking
     * - Path symbol: '#' for clear path highlighting
     * - Progress bar enabled for user feedback
     */
    AnimationConfig()
        : style(AnimationStyle::WITH_TRAIL), speed(AnimationSpeed::NORMAL), unitSymbol('U'), trailSymbol('.'),
          pathSymbol('#'), showStepNumbers(false), clearScreenBetweenFrames(true), showProgress(true)
    {
    }
};

/**
 * @struct MultiUnitAnimationConfig
 * @brief Configuration settings for multi-unit path animation
 *
 * Extended configuration for animating multiple units simultaneously,
 * including collision detection, individual unit identification, and
 * enhanced visual feedback for complex scenarios.
 */
struct MultiUnitAnimationConfig
{
    AnimationStyle style;          ///< Visual style of the animation
    AnimationSpeed speed;          ///< Animation speed in milliseconds per step
    std::vector<char> unitSymbols; ///< Different symbols for each unit (up to 15 units)
    char trailSymbol;              ///< Character used for path trail display
    char pathSymbol;               ///< Character used for path highlighting
    char collisionSymbol;          ///< Symbol displayed when units occupy same position
    bool showUnitIds;              ///< Whether to show unit IDs instead of generic symbols
    bool showTrails;               ///< Whether to display path trails
    bool showProgress;             ///< Whether to display progress bar
    bool clearScreenBetweenFrames; ///< Whether to clear screen between animation frames
    bool highlightCollisions;      ///< Whether to visually highlight unit collisions

    /**
     * @brief Default constructor with multi-unit optimized settings
     *
     * Initializes config with settings optimized for multi-unit scenarios:
     * - Unit symbols: '1'-'9', 'A'-'F' for up to 15 distinguishable units
     * - Collision symbol: 'X' for clear collision indication
     * - All visual aids enabled by default for maximum clarity
     */
    MultiUnitAnimationConfig()
        : style(AnimationStyle::WITH_TRAIL), speed(AnimationSpeed::NORMAL),
          unitSymbols({'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}),
          trailSymbol('.'), pathSymbol('#'), collisionSymbol('X'),
          showUnitIds(true), showTrails(true), showProgress(true),
          clearScreenBetweenFrames(true), highlightCollisions(true)
    {
    }
};

/**
 * @class PathAnimator
 * @brief Main class for terminal-based pathfinding animation
 *
 * The PathAnimator class provides comprehensive functionality for visualizing
 * pathfinding results in terminal environments. It supports both single-unit
 * and multi-unit scenarios with various animation styles, speeds, and
 * interactive controls.
 *
 * Key features:
 * - Multiple animation styles (simple, trail, numbered, highlighted)
 * - Configurable animation speeds
 * - Single and multi-unit support
 * - Interactive step-by-step mode
 * - Collision detection and visualization
 * - Progress tracking and visual feedback
 * - Static path overview generation
 *
 * @note This class uses ANSI escape codes for terminal control and may not
 *       work correctly in all terminal environments.
 *
 * @example
 * @code
 * PathAnimator animator;
 * AnimationConfig config;
 * config.style = AnimationStyle::WITH_TRAIL;
 * config.speed = AnimationSpeed::NORMAL;
 * animator.setConfig(config);
 *
 * // Animate a single unit path
 * std::vector<Position> path = findPath();
 * animator.animatePath(battleMap, path);
 * @endcode
 */
class PathAnimator
{
private:
    // ==========================
    // Member Variables
    // ==========================

    AnimationConfig config;               ///< Configuration for single-unit animations
    MultiUnitAnimationConfig multiConfig; ///< Configuration for multi-unit animations

    // ==========================
    // Terminal Control Methods
    // ==========================

    /**
     * @brief Clears the entire terminal screen
     * @note Uses ANSI escape sequence for screen clearing
     */
    void clearScreen() const;

    /**
     * @brief Moves cursor to top-left corner of terminal
     * @note Uses ANSI escape sequence for cursor positioning
     */
    void moveCursorToTop() const;

    /**
     * @brief Hides the terminal cursor during animation
     * @note Uses ANSI escape sequence for cursor control
     */
    void hideCursor() const;

    /**
     * @brief Shows the terminal cursor after animation
     * @note Uses ANSI escape sequence for cursor control
     */
    void showCursor() const;

    /**
     * @brief Sets terminal text color using ANSI codes
     * @param colorCode ANSI color code from Colors namespace
     */
    void setColor(int colorCode) const;

    /**
     * @brief Resets terminal color to default
     * @note Uses ANSI escape sequence for color reset
     */
    void resetColor() const;

    // ==========================
    // Animation Helper Methods
    // ==========================

    /**
     * @brief Pauses execution for specified duration
     * @param milliseconds Duration to sleep in milliseconds
     * @note Uses std::this_thread::sleep_for for cross-platform compatibility
     */
    void sleep(int milliseconds) const;

    /**
     * @brief Displays a complete animation frame for single-unit path
     * @param battleMap The map being animated on
     * @param path The path being animated
     * @param currentStep Current step in the animation (0-based)
     * @note Combines map rendering, progress display, and information output
     */
    void displayFrame(const BattleMap &battleMap, const std::vector<Position> &path, size_t currentStep) const;

    /**
     * @brief Displays progress bar showing animation completion
     * @param currentStep Current step in animation (0-based)
     * @param totalSteps Total number of steps in animation
     * @note Only displays if showProgress is enabled in config
     */
    void displayProgressBar(size_t currentStep, size_t totalSteps) const;

    /**
     * @brief Displays animation information header
     * @param path The path being animated
     * @param currentStep Current step in the animation (0-based)
     * @note Shows current position, step count, and total distance
     */
    void displayAnimationInfo(const std::vector<Position> &path, size_t currentStep) const;

    // ==========================
    // Single-Unit Rendering Methods
    // ==========================

    /**
     * @brief Renders the battle map with single-unit animation state
     * @param battleMap The map to render
     * @param path The path being animated
     * @param currentStep Current step in the animation (0-based)
     * @note Handles trail rendering, path highlighting, and unit positioning
     */
    void renderMapWithAnimation(const BattleMap &battleMap, const std::vector<Position> &path, size_t currentStep) const;

    // ==========================
    // Multi-Unit Helper Methods
    // ==========================

    /**
     * @brief Displays a complete animation frame for multi-unit paths
     * @param battleMap The map being animated on
     * @param result Pathfinding result containing all unit paths
     * @param currentTimeStep Current time step in the animation (0-based)
     * @note Coordinates display of multiple units and collision detection
     */
    void displayMultiUnitFrame(const BattleMap &battleMap, const PathfindingResult &result, size_t currentTimeStep) const;

    /**
     * @brief Renders the battle map with multi-unit animation state
     * @param battleMap The map to render
     * @param result Pathfinding result containing all unit paths
     * @param currentTimeStep Current time step in the animation (0-based)
     * @note Handles unit positioning, trail rendering, and collision visualization
     */
    void renderMapWithMultiUnitAnimation(const BattleMap &battleMap, const PathfindingResult &result, size_t currentTimeStep) const;

    /**
     * @brief Displays multi-unit animation information header
     * @param result Pathfinding result containing all unit paths
     * @param currentTimeStep Current time step in the animation (0-based)
     * @note Shows time step, unit positions, and collision warnings
     */
    void displayMultiUnitAnimationInfo(const PathfindingResult &result, size_t currentTimeStep) const;

    /**
     * @brief Displays progress bar for multi-unit animation
     * @param currentStep Current step in animation (0-based)
     * @param totalSteps Total number of steps in animation
     * @note Adapted for multi-unit time step progression
     */
    void displayMultiUnitProgressBar(size_t currentStep, size_t totalSteps) const;

    /**
     * @brief Gets the appropriate symbol for a unit based on its index
     * @param unitIndex Index of the unit (0-based)
     * @return Character symbol for the unit, or '?' if index out of range
     * @note Uses symbols from multiConfig.unitSymbols vector
     */
    char getUnitSymbol(int unitIndex) const;

    /**
     * @brief Maps positions to unit lists at a specific time step
     * @param result Pathfinding result containing all unit paths
     * @param timeStep Time step to analyze (0-based)
     * @return Map of positions to vectors of unit indices at those positions
     * @note Used for collision detection and visualization
     */
    std::map<Position, std::vector<int>> getUnitsAtPositions(const PathfindingResult &result, size_t timeStep) const;

    /**
     * @brief Checks if a unit is at its target position
     * @param result Pathfinding result containing unit information
     * @param unitIndex Index of the unit to check (0-based)
     * @param pos Position to check against unit's target
     * @return True if the unit is at its target position, false otherwise
     * @note Used to distinguish between collisions and successful target reaching
     */
    bool isUnitAtTarget(const PathfindingResult &result, int unitIndex, const Position &pos) const;

public:
    // ==========================
    // Constructors
    // ==========================

    /**
     * @brief Default constructor with standard configuration
     * @note Initializes with default AnimationConfig settings
     */
    PathAnimator();

    /**
     * @brief Constructor with custom single-unit configuration
     * @param config Custom animation configuration for single-unit animations
     */
    PathAnimator(const AnimationConfig &config);

    // ==========================
    // Configuration Methods
    // ==========================

    /**
     * @brief Sets the animation style for single-unit animations
     * @param style New animation style to use
     * @see AnimationStyle for available options
     */
    void setAnimationStyle(AnimationStyle style);

    /**
     * @brief Sets the animation speed for single-unit animations
     * @param speed New animation speed to use
     * @see AnimationSpeed for available options
     */
    void setAnimationSpeed(AnimationSpeed speed);

    /**
     * @brief Sets the symbol used to represent units in single-unit animations
     * @param symbol Character to use for unit representation
     * @note Common choices: 'U', '*', '@', 'o'
     */
    void setUnitSymbol(char symbol);

    /**
     * @brief Sets the symbol used for path trails in animations
     * @param symbol Character to use for trail marking
     * @note Common choices: '.', '-', '~', '*'
     */
    void setTrailSymbol(char symbol);

    /**
     * @brief Enables or disables progress bar display
     * @param show True to show progress bar, false to hide
     */
    void setShowProgress(bool show);

    /**
     * @brief Sets complete configuration for single-unit animations
     * @param newConfig New configuration to apply
     * @note Replaces all current single-unit settings
     */
    void setConfig(const AnimationConfig &newConfig);

    /**
     * @brief Gets current single-unit animation configuration
     * @return Current AnimationConfig settings
     * @note Returns a const reference to prevent accidental modification
     */
    const AnimationConfig &getConfig() const;

    /**
     * @brief Sets complete configuration for multi-unit animations
     * @param newConfig New multi-unit configuration to apply
     * @note Replaces all current multi-unit settings
     */
    void setMultiUnitConfig(const MultiUnitAnimationConfig &newConfig);

    /**
     * @brief Gets current multi-unit animation configuration
     * @return Current MultiUnitAnimationConfig settings
     * @note Returns a const reference to prevent accidental modification
     */
    const MultiUnitAnimationConfig &getMultiUnitConfig() const;

    // ==========================
    // Single-Unit Animation Methods
    // ==========================

    /**
     * @brief Animates a single unit path with automatic timing
     * @param battleMap The map to animate on
     * @param path The path to animate
     * @return True if animation completed successfully, false if interrupted
     * @note Uses configured animation speed and style
     * @warning Requires terminal that supports ANSI escape codes
     */
    bool animatePath(const BattleMap &battleMap, const std::vector<Position> &path);

    /**
     * @brief Animates a single unit path with manual step control
     * @param battleMap The map to animate on
     * @param path The path to animate
     * @return True if animation completed successfully, false if interrupted
     * @note Waits for user input (ENTER) between each step
     * @note User can quit by typing 'q' and pressing ENTER
     */
    bool animatePathStepByStep(const BattleMap &battleMap, const std::vector<Position> &path);

    // ==========================
    // Multi-Unit Animation Methods
    // ==========================

    /**
     * @brief Animates multiple unit paths simultaneously with automatic timing
     * @param battleMap The map to animate on
     * @param result Pathfinding result containing all unit paths
     * @return True if animation completed successfully, false if interrupted
     * @note Requires all paths to be found (result.allPathsFound == true)
     * @note Displays collisions and coordinates multiple unit movements
     */
    bool animateMultiUnitPaths(const BattleMap &battleMap, const PathfindingResult &result);

    /**
     * @brief Animates multiple unit paths with manual step control
     * @param battleMap The map to animate on
     * @param result Pathfinding result containing all unit paths
     * @return True if animation completed successfully, false if interrupted
     * @note Waits for user input (ENTER) between each time step
     * @note User can quit by typing 'q' and pressing ENTER
     */
    bool animateMultiUnitPathsStepByStep(const BattleMap &battleMap, const PathfindingResult &result);

    /**
     * @brief Animates partial multi-unit results with automatic timing
     * @param battleMap The map to animate on
     * @param result Pathfinding result (may contain some failed paths)
     * @return True if animation completed successfully, false if interrupted
     * @note Animates only units that found successful paths
     * @note More lenient than animateMultiUnitPaths regarding path requirements
     */
    bool animatePartialMultiUnitPaths(const BattleMap &battleMap, const PathfindingResult &result);

    /**
     * @brief Animates partial multi-unit results with manual step control
     * @param battleMap The map to animate on
     * @param result Pathfinding result (may contain some failed paths)
     * @return True if animation completed successfully, false if interrupted
     * @note Combines partial animation with step-by-step control
     */
    bool animatePartialMultiUnitPathsStepByStep(const BattleMap &battleMap, const PathfindingResult &result);

    // ==========================
    // Utility Methods
    // ==========================

    /**
     * @brief Displays a static overview of a single-unit path
     * @param battleMap The map to display
     * @param path The path to visualize
     * @note Shows entire path at once without animation
     * @note Useful for quick path verification or documentation
     */
    void displayStaticPathOverview(const BattleMap &battleMap, const std::vector<Position> &path) const;

    /**
     * @brief Validates inputs for single-unit animation
     * @param battleMap The map to validate
     * @param path The path to validate
     * @return True if inputs are valid for animation, false otherwise
     * @note Checks for empty path, invalid map dimensions, out-of-bounds positions
     */
    bool validateAnimationInputs(const BattleMap &battleMap, const std::vector<Position> &path) const;

    /**
     * @brief Displays legend explaining animation symbols
     * @note Shows meaning of all symbols used in current animation configuration
     * @note Adapts to current animation style settings
     */
    void printAnimationLegend() const;

    /**
     * @brief Displays a static overview of multi-unit paths
     * @param battleMap The map to display
     * @param result Pathfinding result containing all unit paths
     * @note Shows all unit paths simultaneously without animation
     * @note Useful for analyzing multi-unit conflicts and path efficiency
     */
    void displayStaticMultiUnitOverview(const BattleMap &battleMap, const PathfindingResult &result) const;

    /**
     * @brief Validates inputs for multi-unit animation (strict validation)
     * @param battleMap The map to validate
     * @param result The pathfinding result to validate
     * @return True if inputs are valid for animation, false otherwise
     * @note Requires all paths to be found for animation
     * @note Checks for valid step-by-step positions and map dimensions
     */
    bool validateMultiUnitAnimationInputs(const BattleMap &battleMap, const PathfindingResult &result) const;

    /**
     * @brief Validates inputs for partial multi-unit animation (lenient validation)
     * @param battleMap The map to validate
     * @param result The pathfinding result to validate
     * @return True if inputs are valid for partial animation, false otherwise
     * @note Accepts partial results where some units failed to find paths
     * @note Requires at least one successful path for animation
     */
    bool validatePartialMultiUnitAnimationInputs(const BattleMap &battleMap, const PathfindingResult &result) const;

    /**
     * @brief Displays legend explaining multi-unit animation symbols
     * @note Shows meaning of all symbols used in multi-unit animations
     * @note Includes collision indicators and trail symbols
     */
    void printMultiUnitAnimationLegend() const;

    // ==========================
    // Interactive Animation Control
    // ==========================

    /**
     * @brief Animates path with interactive controls (simplified implementation)
     * @param battleMap The map to animate on
     * @param path The path to animate
     * @return True if animation completed successfully, false if interrupted
     * @note Currently falls back to standard animation
     * @note Future versions may support pause/resume, speed control
     */
    bool animateWithControls(const BattleMap &battleMap, const std::vector<Position> &path);

    // ==========================
    // Static Utility Methods
    // ==========================

    /**
     * @brief Prints all available animation styles with descriptions
     * @note Static method for documentation and help display
     */
    static void printAvailableStyles();

    /**
     * @brief Prints all available animation speeds with descriptions
     * @note Static method for documentation and help display
     */
    static void printAvailableSpeeds();

    /**
     * @brief Creates a custom single-unit animation configuration
     * @param style Animation style to use
     * @param speed Animation speed to use
     * @param unitSymbol Character for unit representation (default: 'U')
     * @param trailSymbol Character for trail marking (default: '.')
     * @return Configured AnimationConfig object
     * @note Static factory method for convenient config creation
     */
    static AnimationConfig createCustomConfig(AnimationStyle style, AnimationSpeed speed,
                                              char unitSymbol = 'U', char trailSymbol = '.');

    /**
     * @brief Creates a custom multi-unit animation configuration
     * @param style Animation style to use
     * @param speed Animation speed to use
     * @return Configured MultiUnitAnimationConfig object
     * @note Static factory method with default multi-unit settings
     */
    static MultiUnitAnimationConfig createCustomMultiUnitConfig(AnimationStyle style, AnimationSpeed speed);

    // ==========================
    // Static Parsing Methods
    // ==========================

    /**
     * @brief Parses animation style from string representation
     * @param styleStr String representation of animation style
     * @return Corresponding AnimationStyle enum value
     * @note Accepts: "simple", "trail"/"with_trail", "numbered"/"numbered_steps", "highlight"/"highlight_path"
     * @note Returns WITH_TRAIL as default for unknown strings
     */
    static AnimationStyle parseAnimationStyle(const std::string &styleStr);

    /**
     * @brief Parses animation speed from string representation
     * @param speedStr String representation of animation speed
     * @return Corresponding AnimationSpeed enum value
     * @note Accepts: "very_slow", "slow", "normal"/"medium", "fast", "very_fast"
     * @note Also accepts numeric values as milliseconds per step
     * @note Returns NORMAL as default for unknown strings
     */
    static AnimationSpeed parseAnimationSpeed(const std::string &speedStr);
};

#endif // PATHANIMATOR_H