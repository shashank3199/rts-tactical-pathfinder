/**
 * @file PathAnimator.cpp
 * @brief Terminal-based path animation library for single and multi-unit pathfinding visualization - Implementation File
 * @author Shashank Goyal
 * @date 2025
 * @version 1.0
 *
 * This file contains the implementation of the PathAnimator class, which provides
 * comprehensive functionality for animating pathfinding results in terminal environments.
 * It supports both single-unit and multi-unit scenarios with various animation styles,
 * speeds, and interactive controls.
 *
 * @copyright Copyright (c) 2025. All rights reserved.
 */

#include "PathAnimator.h"
#include "../PathFinder/PathFinder.h"
#include "../MultiUnitPathFinder/MultiUnitPathFinder.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <algorithm>
#include <unordered_set>
#include <cstdio>

// ANSI escape codes
#define CLEAR_SCREEN "\033[2J"
#define MOVE_CURSOR_HOME "\033[H"
#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"
#define RESET_COLOR "\033[0m"

PathAnimator::PathAnimator()
{
    // Use default configuration
}

PathAnimator::PathAnimator(const AnimationConfig &config) : config(config)
{
}

// Terminal control methods
void PathAnimator::clearScreen() const
{
    std::cout << CLEAR_SCREEN;
    std::cout.flush();
}

void PathAnimator::moveCursorToTop() const
{
    std::cout << MOVE_CURSOR_HOME;
    std::cout.flush();
}

void PathAnimator::hideCursor() const
{
    std::cout << HIDE_CURSOR;
    std::cout.flush();
}

void PathAnimator::showCursor() const
{
    std::cout << SHOW_CURSOR;
    std::cout.flush();
}

void PathAnimator::setColor(int colorCode) const
{
    std::cout << "\033[" << colorCode << "m";
}

void PathAnimator::resetColor() const
{
    std::cout << RESET_COLOR;
}

void PathAnimator::sleep(int milliseconds) const
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

// Configuration methods
void PathAnimator::setAnimationStyle(AnimationStyle style)
{
    config.style = style;
}

void PathAnimator::setAnimationSpeed(AnimationSpeed speed)
{
    config.speed = speed;
}

void PathAnimator::setUnitSymbol(char symbol)
{
    config.unitSymbol = symbol;
}

void PathAnimator::setTrailSymbol(char symbol)
{
    config.trailSymbol = symbol;
}

void PathAnimator::setShowProgress(bool show)
{
    config.showProgress = show;
}

void PathAnimator::setConfig(const AnimationConfig &newConfig)
{
    config = newConfig;
}

const AnimationConfig &PathAnimator::getConfig() const
{
    return config;
}

bool PathAnimator::validateAnimationInputs(const BattleMap &battleMap, const std::vector<Position> &path) const
{
    if (path.empty())
    {
        std::cerr << "Error: Cannot animate empty path" << std::endl;
        return false;
    }

    if (battleMap.width <= 0 || battleMap.height <= 0)
    {
        std::cerr << "Error: Invalid battle map dimensions" << std::endl;
        return false;
    }

    // Validate path positions are within map bounds
    for (const Position &pos : path)
    {
        if (pos.x < 0 || pos.x >= battleMap.width || pos.y < 0 || pos.y >= battleMap.height)
        {
            std::cerr << "Error: Path contains position outside map bounds: ("
                      << pos.x << "," << pos.y << ")" << std::endl;
            return false;
        }
    }

    return true;
}

bool PathAnimator::isUnitAtTarget(const PathfindingResult &result, int unitIndex, const Position &pos) const
{
    if (unitIndex >= 0 && unitIndex < static_cast<int>(result.units.size()))
    {
        return result.units[unitIndex].targetPos == pos;
    }
    return false;
}

void PathAnimator::renderMapWithAnimation(const BattleMap &battleMap, const std::vector<Position> &path, size_t currentStep) const
{
    // Create sets for quick lookup
    std::unordered_set<Position, PositionHash> pathSet(path.begin(), path.end());
    Position currentPos = (currentStep < path.size()) ? path[currentStep] : Position(-1, -1);

    // Render the map
    for (int y = 0; y < battleMap.height; ++y)
    {
        for (int x = 0; x < battleMap.width; ++x)
        {
            Position pos(x, y);
            double tile = battleMap.grid[y][x];

            // Determine what to display at this position
            if (pos == currentPos)
            {
                // Current unit position
                setColor(Colors::BRIGHT_GREEN);
                std::cout << config.unitSymbol << " ";
                resetColor();
            }
            else if (pos == battleMap.startPos && currentStep == 0)
            {
                // Start position (before movement begins)
                setColor(Colors::BRIGHT_BLUE);
                std::cout << "S ";
                resetColor();
            }
            else if (pos == battleMap.targetPos && currentStep >= path.size())
            {
                // Target position (after reaching target)
                setColor(Colors::BRIGHT_MAGENTA);
                std::cout << "T ";
                resetColor();
            }
            else
            {
                // Check if this position is part of the path trail
                bool showTrail = false;
                if (config.style == AnimationStyle::WITH_TRAIL || config.style == AnimationStyle::NUMBERED_STEPS)
                {
                    // Find if this position is in visited part of path
                    for (size_t i = 0; i < currentStep && i < path.size(); ++i)
                    {
                        if (path[i] == pos)
                        {
                            showTrail = true;
                            if (config.style == AnimationStyle::NUMBERED_STEPS && i < 100)
                            {
                                if (i < 10)
                                {
                                    setColor(Colors::YELLOW);
                                    std::cout << i << " ";
                                }
                                else
                                {
                                    setColor(Colors::YELLOW);
                                    std::cout << std::setw(2) << i;
                                }
                                resetColor();
                            }
                            else
                            {
                                setColor(Colors::CYAN);
                                std::cout << config.trailSymbol << " ";
                                resetColor();
                            }
                            break;
                        }
                    }
                }

                if (!showTrail)
                {
                    // Check if we should highlight the entire path
                    bool isInPath = (pathSet.find(pos) != pathSet.end());

                    if (config.style == AnimationStyle::HIGHLIGHT_PATH && isInPath)
                    {
                        setColor(Colors::BRIGHT_YELLOW);
                        std::cout << config.pathSymbol << " ";
                        resetColor();
                    }
                    else
                    {
                        // Display normal terrain
                        if (pos == battleMap.startPos)
                        {
                            setColor(Colors::BLUE);
                            std::cout << "S "; // Start position
                            resetColor();
                        }
                        else if (pos == battleMap.targetPos)
                        {
                            setColor(Colors::MAGENTA);
                            std::cout << "T "; // Target position
                            resetColor();
                        }
                        else if (tile == -1)
                        {
                            std::cout << "░░"; // Reachable
                        }
                        else if (tile == 3)
                        {
                            setColor(Colors::RED);
                            std::cout << "▲▲"; // Elevated
                            resetColor();
                        }
                        else
                        {
                            std::cout << std::setw(2) << std::fixed << std::setprecision(0) << tile;
                        }
                    }
                }
            }
        }
        std::cout << std::endl;
    }
}

void PathAnimator::displayProgressBar(size_t currentStep, size_t totalSteps) const
{
    if (!config.showProgress || totalSteps == 0)
        return;

    const int barWidth = 40;

    // Add 1 to currentStep for display purposes to show 100% on final step
    float progress = static_cast<float>(currentStep + 1) / totalSteps;
    int filledWidth = static_cast<int>(progress * barWidth);

    std::cout << "\n[";
    setColor(Colors::GREEN);
    for (int i = 0; i < filledWidth; ++i)
    {
        std::cout << "=";
    }
    resetColor();
    for (int i = filledWidth; i < barWidth; ++i)
    {
        std::cout << " ";
    }
    std::cout << "] " << std::setw(3) << static_cast<int>(progress * 100) << "% ";

    // Show currentStep + 1 for 1-based display
    std::cout << "(" << (currentStep + 1) << "/" << totalSteps << ")";
}

void PathAnimator::displayAnimationInfo(const std::vector<Position> &path, size_t currentStep) const
{
    std::cout << "\n";
    setColor(Colors::BRIGHT_BLUE);
    std::cout << "=== Path Animation ===";
    resetColor();

    if (currentStep < path.size())
    {
        std::cout << "\nCurrent Position: (" << path[currentStep].x << "," << path[currentStep].y << ")";
        std::cout << " | Step: " << (currentStep + 1) << "/" << path.size();
    }
    else
    {
        setColor(Colors::BRIGHT_GREEN);
        std::cout << "\n*** TARGET REACHED! ***";
        resetColor();
    }

    double totalDistance = PathFinder::calculatePathLength(path);
    std::cout << " | Total Distance: " << totalDistance << " units";
}

void PathAnimator::displayFrame(const BattleMap &battleMap, const std::vector<Position> &path, size_t currentStep) const
{
    if (config.clearScreenBetweenFrames)
    {
        clearScreen();
    }
    else
    {
        moveCursorToTop();
    }

    // Display animation info
    displayAnimationInfo(path, currentStep);
    std::cout << "\n\n";

    // Render the map with current animation state
    renderMapWithAnimation(battleMap, path, currentStep);

    // Display progress bar
    displayProgressBar(currentStep, path.size());

    std::cout << std::endl;
    std::cout.flush();
}

bool PathAnimator::animatePath(const BattleMap &battleMap, const std::vector<Position> &path)
{
    if (!validateAnimationInputs(battleMap, path))
    {
        return false;
    }

    std::cout << "\n=== Starting Path Animation ===\n";
    std::cout << "Animation Style: ";

    switch (config.style)
    {
    case AnimationStyle::SIMPLE:
        std::cout << "Simple Movement";
        break;
    case AnimationStyle::WITH_TRAIL:
        std::cout << "With Trail";
        break;
    case AnimationStyle::NUMBERED_STEPS:
        std::cout << "Numbered Steps";
        break;
    case AnimationStyle::HIGHLIGHT_PATH:
        std::cout << "Highlight Path";
        break;
    }

    std::cout << " | Speed: " << static_cast<int>(config.speed) << "ms per step\n";
    std::cout << "Press Enter to start animation (Ctrl+C to stop)...\n";

    // Wait for Enter key to start
    std::string input;
    std::getline(std::cin, input);

    hideCursor();

    try
    {
        std::vector<bool> visitedSteps(path.size(), false);

        // Show initial state
        displayFrame(battleMap, path, 0);
        sleep(static_cast<int>(config.speed));

        // Animate movement along the path
        for (size_t step = 0; step < path.size(); ++step)
        {
            visitedSteps[step] = true;
            displayFrame(battleMap, path, step);

            if (step < path.size() - 1)
            { // Don't sleep after last step
                sleep(static_cast<int>(config.speed));
            }
        }

        // Wait for Enter key to continue after animation completes
        showCursor();
        std::cout << "\nAnimation complete! Press Enter to continue...";
        std::getline(std::cin, input);
        hideCursor();

        // Display completion message
        std::cout << "\n";
        setColor(Colors::BRIGHT_GREEN);
        std::cout << "=== Animation Complete! ===";
        resetColor();
        std::cout << "\nPath successfully animated from start to target.\n";
    }
    catch (...)
    {
        showCursor();
        std::cerr << "\nAnimation interrupted!" << std::endl;
        return false;
    }

    showCursor();
    return true;
}

bool PathAnimator::animatePathStepByStep(const BattleMap &battleMap, const std::vector<Position> &path)
{
    if (!validateAnimationInputs(battleMap, path))
    {
        return false;
    }

    std::cout << "\n=== Step-by-Step Path Animation ===\n";
    std::cout << "Press ENTER to advance to next step, 'q' to quit...\n\n";

    hideCursor();

    try
    {
        std::vector<bool> visitedSteps(path.size(), false);

        for (size_t step = 0; step < path.size(); ++step)
        {
            visitedSteps[step] = true;
            displayFrame(battleMap, path, step);

            // Wait for user input
            std::cout << "\nPress ENTER for next step (or 'q' to quit): ";
            showCursor();

            std::string input;
            std::getline(std::cin, input);

            if (!input.empty() && (input[0] == 'q' || input[0] == 'Q'))
            {
                std::cout << "Animation stopped by user.\n";
                return true;
            }

            hideCursor();
        }

        std::cout << "\n";
        setColor(Colors::BRIGHT_GREEN);
        std::cout << "=== Step-by-Step Animation Complete! ===";
        resetColor();
        std::cout << std::endl;
    }
    catch (...)
    {
        showCursor();
        std::cerr << "\nStep-by-step animation interrupted!" << std::endl;
        return false;
    }

    showCursor();
    return true;
}

bool PathAnimator::animateWithControls(const BattleMap &battleMap, const std::vector<Position> &path)
{
    // @todo: Implement non-blocking keyboard input for controls
    // This is a placeholder implementation that will not work as intended
    // without proper non-blocking input handling, which may be platform-specific.

    if (!validateAnimationInputs(battleMap, path))
    {
        return false;
    }

    std::cout << "\n=== Interactive Path Animation ===\n";
    std::cout << "Controls:\n";
    std::cout << "  SPACE - Pause/Resume\n";
    std::cout << "  ENTER - Next step (when paused)\n";
    std::cout << "  'f' - Toggle fast mode\n";
    std::cout << "  'q' - Quit\n\n";
    std::cout << "Animation will start in 3 seconds...\n";

    sleep(3000);

    // This is a simplified version - full implementation would require
    // non-blocking keyboard input which is platform-specific
    return animatePath(battleMap, path);
}

void PathAnimator::displayStaticPathOverview(const BattleMap &battleMap, const std::vector<Position> &path) const
{
    if (!validateAnimationInputs(battleMap, path))
    {
        return;
    }

    std::cout << "\n=== Static Path Overview ===\n";

    // Create path set for quick lookup
    std::unordered_set<Position, PositionHash> pathSet(path.begin(), path.end());

    for (int y = 0; y < battleMap.height; ++y)
    {
        for (int x = 0; x < battleMap.width; ++x)
        {
            Position pos(x, y);
            double tile = battleMap.grid[y][x];

            if (pos == battleMap.startPos)
            {
                setColor(Colors::BRIGHT_BLUE);
                std::cout << "S ";
                resetColor();
            }
            else if (pos == battleMap.targetPos)
            {
                setColor(Colors::BRIGHT_MAGENTA);
                std::cout << "T ";
                resetColor();
            }
            else if (pathSet.find(pos) != pathSet.end())
            {
                setColor(Colors::BRIGHT_GREEN);
                std::cout << "##";
                resetColor();
            }
            else if (tile == -1)
            {
                std::cout << "░░"; // Reachable
            }
            else if (tile == 3)
            {
                setColor(Colors::RED);
                std::cout << "▲▲"; // Elevated
                resetColor();
            }
            else
            {
                std::cout << std::setw(2) << std::fixed << std::setprecision(0) << tile;
            }
        }
        std::cout << std::endl;
    }

    std::cout << "\nPath Length: " << path.size() << " steps" << std::endl;
    std::cout << "Total Distance: " << PathFinder::calculatePathLength(path) << " units" << std::endl;
}

void PathAnimator::printAnimationLegend() const
{
    std::cout << "\n=== Animation Legend ===\n";

    setColor(Colors::BRIGHT_GREEN);
    std::cout << config.unitSymbol << " ";
    resetColor();
    std::cout << "- Battle Unit (current position)\n";

    setColor(Colors::BRIGHT_BLUE);
    std::cout << "S ";
    resetColor();
    std::cout << "- Start Position\n";

    setColor(Colors::BRIGHT_MAGENTA);
    std::cout << "T ";
    resetColor();
    std::cout << "- Target Position\n";

    if (config.style == AnimationStyle::WITH_TRAIL)
    {
        setColor(Colors::CYAN);
        std::cout << config.trailSymbol << " ";
        resetColor();
        std::cout << "- Path Trail\n";
    }

    if (config.style == AnimationStyle::NUMBERED_STEPS)
    {
        setColor(Colors::YELLOW);
        std::cout << "1 ";
        resetColor();
        std::cout << "- Step Numbers\n";
    }

    if (config.style == AnimationStyle::HIGHLIGHT_PATH)
    {
        setColor(Colors::BRIGHT_YELLOW);
        std::cout << config.pathSymbol << " ";
        resetColor();
        std::cout << "- Path Highlight\n";
    }

    std::cout << ".. - Reachable Terrain\n";
    setColor(Colors::RED);
    std::cout << "▲▲";
    resetColor();
    std::cout << " - Elevated Terrain (Obstacle)\n";
}

void PathAnimator::printAvailableStyles()
{
    std::cout << "\n=== Available Animation Styles ===\n";
    std::cout << "1. SIMPLE         - Basic unit movement\n";
    std::cout << "2. WITH_TRAIL     - Show path trail behind unit\n";
    std::cout << "3. NUMBERED_STEPS - Show step numbers\n";
    std::cout << "4. HIGHLIGHT_PATH - Highlight entire path\n";
}

void PathAnimator::printAvailableSpeeds()
{
    std::cout << "\n=== Available Animation Speeds ===\n";
    std::cout << "1. VERY_SLOW - 1000ms per step\n";
    std::cout << "2. SLOW      - 500ms per step\n";
    std::cout << "3. NORMAL    - 250ms per step\n";
    std::cout << "4. FAST      - 100ms per step\n";
    std::cout << "5. VERY_FAST - 50ms per step\n";
}

AnimationConfig PathAnimator::createCustomConfig(AnimationStyle style, AnimationSpeed speed,
                                                 char unitSymbol, char trailSymbol)
{
    AnimationConfig config;
    config.style = style;
    config.speed = speed;
    config.unitSymbol = unitSymbol;
    config.trailSymbol = trailSymbol;
    return config;
}

void PathAnimator::setMultiUnitConfig(const MultiUnitAnimationConfig &newConfig)
{
    multiConfig = newConfig;
}

const MultiUnitAnimationConfig &PathAnimator::getMultiUnitConfig() const
{
    return multiConfig;
}

char PathAnimator::getUnitSymbol(int unitIndex) const
{
    if (unitIndex >= 0 && unitIndex < static_cast<int>(multiConfig.unitSymbols.size()))
    {
        return multiConfig.unitSymbols[unitIndex];
    }
    return '?'; // Unknown unit
}

std::map<Position, std::vector<int>> PathAnimator::getUnitsAtPositions(const PathfindingResult &result, size_t timeStep) const
{
    std::map<Position, std::vector<int>> unitsAtPos;

    if (timeStep < result.stepByStepPositions.size())
    {
        const auto &positions = result.stepByStepPositions[timeStep];
        for (size_t unitIndex = 0; unitIndex < positions.size() && unitIndex < result.units.size(); ++unitIndex)
        {
            if (result.units[unitIndex].pathFound)
            {
                Position pos = positions[unitIndex];
                unitsAtPos[pos].push_back(static_cast<int>(unitIndex));
            }
        }
    }

    return unitsAtPos;
}

void PathAnimator::renderMapWithMultiUnitAnimation(const BattleMap &battleMap, const PathfindingResult &result,
                                                   size_t currentTimeStep) const
{
    // Get current unit positions
    auto unitsAtPositions = getUnitsAtPositions(result, currentTimeStep);

    // Create trail positions if enabled
    std::map<Position, std::vector<int>> trailPositions;
    if (multiConfig.showTrails && currentTimeStep > 0)
    {
        for (size_t t = 0; t < currentTimeStep; ++t)
        {
            if (t < result.stepByStepPositions.size())
            {
                const auto &positions = result.stepByStepPositions[t];
                for (size_t unitIndex = 0; unitIndex < positions.size() && unitIndex < result.units.size(); ++unitIndex)
                {
                    if (result.units[unitIndex].pathFound)
                    {
                        Position pos = positions[unitIndex];
                        // Only add to trail if no unit is currently there
                        if (unitsAtPositions.find(pos) == unitsAtPositions.end())
                        {
                            trailPositions[pos].push_back(static_cast<int>(unitIndex));
                        }
                    }
                }
            }
        }
    }

    // Render the map
    for (int y = 0; y < battleMap.height; ++y)
    {
        for (int x = 0; x < battleMap.width; ++x)
        {
            Position pos(x, y);
            int tile = battleMap.grid[y][x];

            // Check if units are at this position
            auto unitIt = unitsAtPositions.find(pos);
            if (unitIt != unitsAtPositions.end())
            {
                const auto &units = unitIt->second;
                if (units.size() == 1)
                {
                    // Single unit
                    setColor(Colors::BRIGHT_GREEN);
                    if (multiConfig.showUnitIds)
                    {
                        std::cout << getUnitSymbol(units[0]) << " ";
                    }
                    else
                    {
                        std::cout << "U ";
                    }
                    resetColor();
                }
                else
                {
                    // Multiple units - check if they're all at their target positions
                    bool allAtTarget = true;
                    for (int unitIndex : units)
                    {
                        if (!isUnitAtTarget(result, unitIndex, pos))
                        {
                            allAtTarget = false;
                            break;
                        }
                    }

                    if (allAtTarget)
                    {
                        // All units at target - show as success, not collision
                        setColor(Colors::BRIGHT_MAGENTA);
                        std::cout << "T" << units.size();
                        resetColor();
                    }
                    else
                    {
                        // Actual collision (not all at target)
                        if (multiConfig.highlightCollisions)
                        {
                            setColor(Colors::BRIGHT_RED);
                            std::cout << multiConfig.collisionSymbol << units.size();
                            resetColor();
                        }
                        else
                        {
                            setColor(Colors::BRIGHT_YELLOW);
                            std::cout << units.size() << " ";
                            resetColor();
                        }
                    }
                }
            }
            // Check for trail positions
            else if (multiConfig.showTrails)
            {
                auto trailIt = trailPositions.find(pos);
                if (trailIt != trailPositions.end())
                {
                    setColor(Colors::CYAN);
                    std::cout << multiConfig.trailSymbol << " ";
                    resetColor();
                }
                else
                {
                    // Display normal terrain
                    if (tile == 0)
                    {
                        setColor(Colors::BLUE);
                        std::cout << "S ";
                        resetColor();
                    }
                    else if (tile == 8)
                    {
                        setColor(Colors::MAGENTA);
                        std::cout << "T ";
                        resetColor();
                    }
                    else if (tile == -1)
                    {
                        std::cout << "░░";
                    }
                    else if (tile == 3)
                    {
                        setColor(Colors::RED);
                        std::cout << "▲▲";
                        resetColor();
                    }
                    else
                    {
                        std::cout << std::setw(2) << std::fixed << std::setprecision(0) << tile;
                    }
                }
            }
            else
            {
                // Display normal terrain
                if (tile == 0)
                {
                    setColor(Colors::BLUE);
                    std::cout << "S ";
                    resetColor();
                }
                else if (tile == 8)
                {
                    setColor(Colors::MAGENTA);
                    std::cout << "T ";
                    resetColor();
                }
                else if (tile == -1)
                {
                    std::cout << "░░";
                }
                else if (tile == 3)
                {
                    setColor(Colors::RED);
                    std::cout << "▲▲";
                    resetColor();
                }
                else
                {
                    std::cout << std::setw(2) << std::fixed << std::setprecision(0) << tile;
                }
            }
        }
        std::cout << std::endl;
    }
}

void PathAnimator::displayMultiUnitAnimationInfo(const PathfindingResult &result, size_t currentTimeStep) const
{
    std::cout << "\n";
    setColor(Colors::BRIGHT_BLUE);
    std::cout << "=== Multi-Unit Path Animation ===";
    resetColor();

    std::cout << "\nTime Step: " << (currentTimeStep + 1) << "/" << result.totalSteps;
    std::cout << " | Units: " << result.units.size();

    if (currentTimeStep < result.stepByStepPositions.size())
    {
        // Show current positions of all units
        std::cout << "\nCurrent Positions:";
        const auto &positions = result.stepByStepPositions[currentTimeStep];
        for (size_t i = 0; i < positions.size() && i < result.units.size(); ++i)
        {
            if (result.units[i].pathFound)
            {
                std::cout << " Unit" << result.units[i].id << ":(" << positions[i].x << "," << positions[i].y << ")";
            }
        }
    }

    // Check for actual collisions (not units at target)
    auto unitsAtPos = getUnitsAtPositions(result, currentTimeStep);
    int collisionCount = 0;
    for (const auto &entry : unitsAtPos)
    {
        if (entry.second.size() > 1)
        {
            // Check if all units at this position are at their targets
            bool allAtTarget = true;
            for (int unitIndex : entry.second)
            {
                if (!isUnitAtTarget(result, unitIndex, entry.first))
                {
                    allAtTarget = false;
                    break;
                }
            }

            // Only count as collision if not all units are at their targets
            if (!allAtTarget)
            {
                collisionCount++;
            }
        }
    }

    if (collisionCount > 0)
    {
        setColor(Colors::BRIGHT_RED);
        std::cout << " | COLLISIONS: " << collisionCount;
        resetColor();
    }
}

void PathAnimator::displayMultiUnitProgressBar(size_t currentStep, size_t totalSteps) const
{
    if (!multiConfig.showProgress || totalSteps == 0)
        return;

    const int barWidth = 40;

    // Add 1 to currentStep for display purposes to show 100% on final step
    float progress = static_cast<float>(currentStep + 1) / totalSteps;
    int filledWidth = static_cast<int>(progress * barWidth);

    std::cout << "\n[";
    setColor(Colors::GREEN);
    for (int i = 0; i < filledWidth; ++i)
    {
        std::cout << "=";
    }
    resetColor();
    for (int i = filledWidth; i < barWidth; ++i)
    {
        std::cout << " ";
    }
    std::cout << "] " << std::setw(3) << static_cast<int>(progress * 100) << "% ";

    // Show currentStep + 1 for 1-based display
    std::cout << "(" << (currentStep + 1) << "/" << totalSteps << ")";
}

void PathAnimator::displayMultiUnitFrame(const BattleMap &battleMap, const PathfindingResult &result,
                                         size_t currentTimeStep) const
{
    if (multiConfig.clearScreenBetweenFrames)
    {
        clearScreen();
    }
    else
    {
        moveCursorToTop();
    }

    // Display animation info
    displayMultiUnitAnimationInfo(result, currentTimeStep);
    std::cout << "\n\n";

    // Render the map with current animation state
    renderMapWithMultiUnitAnimation(battleMap, result, currentTimeStep);

    // Display progress bar
    displayMultiUnitProgressBar(currentTimeStep, result.totalSteps);

    std::cout << std::endl;
    std::cout.flush();
}

bool PathAnimator::validateMultiUnitAnimationInputs(const BattleMap &battleMap, const PathfindingResult &result) const
{
    if (!result.allPathsFound)
    {
        std::cerr << "Error: Cannot animate - not all paths found" << std::endl;
        return false;
    }

    if (result.stepByStepPositions.empty())
    {
        std::cerr << "Error: Cannot animate - no step-by-step positions available" << std::endl;
        return false;
    }

    if (battleMap.width <= 0 || battleMap.height <= 0)
    {
        std::cerr << "Error: Invalid battle map dimensions" << std::endl;
        return false;
    }

    return true;
}

bool PathAnimator::animateMultiUnitPaths(const BattleMap &battleMap, const PathfindingResult &result)
{
    if (!validateMultiUnitAnimationInputs(battleMap, result))
    {
        return false;
    }

    std::cout << "\n=== Starting Multi-Unit Path Animation ===\n";
    std::cout << "Units: " << result.units.size() << " | Total time steps: " << result.totalSteps << "\n";
    std::cout << "Animation Style: ";

    switch (multiConfig.style)
    {
    case AnimationStyle::SIMPLE:
        std::cout << "Simple Movement";
        break;
    case AnimationStyle::WITH_TRAIL:
        std::cout << "With Trail";
        break;
    case AnimationStyle::NUMBERED_STEPS:
        std::cout << "Numbered Steps";
        break;
    case AnimationStyle::HIGHLIGHT_PATH:
        std::cout << "Highlight Path";
        break;
    }

    std::cout << " | Speed: " << static_cast<int>(multiConfig.speed) << "ms per step\n";
    std::cout << "Press Enter to start animation (Ctrl+C to stop)...\n";

    // Wait for Enter key to start
    std::string input;
    std::getline(std::cin, input);

    hideCursor();

    try
    {
        // Animate through all time steps
        for (size_t timeStep = 0; timeStep < result.stepByStepPositions.size(); ++timeStep)
        {
            displayMultiUnitFrame(battleMap, result, timeStep);

            if (timeStep < result.stepByStepPositions.size() - 1)
            {
                sleep(static_cast<int>(multiConfig.speed));
            }
        }

        // Show final state for a bit longer
        sleep(1500);

        // Display completion message
        std::cout << "\n";
        setColor(Colors::BRIGHT_GREEN);
        std::cout << "=== Multi-Unit Animation Complete! ===";
        resetColor();
        std::cout << "\nAll units have reached their targets.\n";
    }
    catch (...)
    {
        showCursor();
        std::cerr << "\nMulti-unit animation interrupted!" << std::endl;
        return false;
    }

    showCursor();
    return true;
}

bool PathAnimator::animateMultiUnitPathsStepByStep(const BattleMap &battleMap, const PathfindingResult &result)
{
    if (!validateMultiUnitAnimationInputs(battleMap, result))
    {
        return false;
    }

    std::cout << "\n=== Step-by-Step Multi-Unit Animation ===\n";
    std::cout << "Press ENTER to advance to next step, 'q' to quit...\n\n";

    hideCursor();

    try
    {
        for (size_t timeStep = 0; timeStep < result.stepByStepPositions.size(); ++timeStep)
        {
            displayMultiUnitFrame(battleMap, result, timeStep);

            // Wait for user input
            std::cout << "\nPress ENTER for next step (or 'q' to quit): ";
            showCursor();

            std::string input;
            std::getline(std::cin, input);

            if (!input.empty() && (input[0] == 'q' || input[0] == 'Q'))
            {
                std::cout << "Multi-unit animation stopped by user.\n";
                return true;
            }

            hideCursor();
        }

        std::cout << "\n";
        setColor(Colors::BRIGHT_GREEN);
        std::cout << "=== Step-by-Step Multi-Unit Animation Complete! ===";
        resetColor();
        std::cout << std::endl;
    }
    catch (...)
    {
        showCursor();
        std::cerr << "\nStep-by-step multi-unit animation interrupted!" << std::endl;
        return false;
    }

    showCursor();
    return true;
}

void PathAnimator::displayStaticMultiUnitOverview(const BattleMap &battleMap, const PathfindingResult &result) const
{
    if (!validateMultiUnitAnimationInputs(battleMap, result))
    {
        return;
    }

    std::cout << "\n=== Static Multi-Unit Path Overview ===\n";

    // Create a map to track which units use each position
    std::map<Position, std::vector<int>> unitPaths;

    for (size_t unitIndex = 0; unitIndex < result.units.size(); ++unitIndex)
    {
        if (result.units[unitIndex].pathFound)
        {
            for (const Position &pos : result.units[unitIndex].path)
            {
                unitPaths[pos].push_back(result.units[unitIndex].id);
            }
        }
    }

    for (int y = 0; y < battleMap.height; ++y)
    {
        for (int x = 0; x < battleMap.width; ++x)
        {
            Position pos(x, y);
            int tile = battleMap.grid[y][x];

            auto pathIt = unitPaths.find(pos);
            if (pathIt != unitPaths.end())
            {
                if (pathIt->second.size() == 1)
                {
                    setColor(Colors::BRIGHT_GREEN);
                    std::cout << pathIt->second[0] << " ";
                    resetColor();
                }
                else
                {
                    setColor(Colors::BRIGHT_YELLOW);
                    std::cout << "*" << pathIt->second.size();
                    resetColor();
                }
            }
            else if (tile == 0)
            {
                setColor(Colors::BRIGHT_BLUE);
                std::cout << "S ";
                resetColor();
            }
            else if (tile == 8)
            {
                setColor(Colors::BRIGHT_MAGENTA);
                std::cout << "T ";
                resetColor();
            }
            else if (tile == -1)
            {
                std::cout << "░░";
            }
            else if (tile == 3)
            {
                setColor(Colors::RED);
                std::cout << "▲▲";
                resetColor();
            }
            else
            {
                std::cout << std::setw(2) << std::fixed << std::setprecision(0) << tile;
            }
        }
        std::cout << std::endl;
    }

    std::cout << "\nTotal units: " << result.units.size() << std::endl;
    std::cout << "Total time steps: " << result.totalSteps << std::endl;
}

void PathAnimator::printMultiUnitAnimationLegend() const
{
    std::cout << "\n=== Multi-Unit Animation Legend ===\n";

    setColor(Colors::BRIGHT_GREEN);
    std::cout << "1-9,A-F ";
    resetColor();
    std::cout << "- Unit IDs (current positions)\n";

    setColor(Colors::BRIGHT_RED);
    std::cout << multiConfig.collisionSymbol << "N ";
    resetColor();
    std::cout << "- Collision (N = number of units)\n";

    setColor(Colors::BRIGHT_BLUE);
    std::cout << "S ";
    resetColor();
    std::cout << "- Start Positions\n";

    setColor(Colors::BRIGHT_MAGENTA);
    std::cout << "T ";
    resetColor();
    std::cout << "- Target Positions\n";

    if (multiConfig.showTrails)
    {
        setColor(Colors::CYAN);
        std::cout << multiConfig.trailSymbol << " ";
        resetColor();
        std::cout << "- Path Trails\n";
    }

    std::cout << ".. - Reachable Terrain\n";

    setColor(Colors::RED);
    std::cout << "▲▲";
    resetColor();
    std::cout << " - Elevated Terrain (Obstacles)\n";
}

MultiUnitAnimationConfig PathAnimator::createCustomMultiUnitConfig(AnimationStyle style, AnimationSpeed speed)
{
    MultiUnitAnimationConfig config;
    config.style = style;
    config.speed = speed;
    return config;
}

bool PathAnimator::validatePartialMultiUnitAnimationInputs(const BattleMap &battleMap, const PathfindingResult &result) const
{
    if (result.stepByStepPositions.empty())
    {
        std::cerr << "Error: Cannot animate - no step-by-step positions available" << std::endl;
        return false;
    }

    if (battleMap.width <= 0 || battleMap.height <= 0)
    {
        std::cerr << "Error: Invalid battle map dimensions" << std::endl;
        return false;
    }

    // Check if at least one unit has a path
    bool hasAnyPath = false;
    for (const auto &unit : result.units)
    {
        if (unit.pathFound)
        {
            hasAnyPath = true;
            break;
        }
    }

    if (!hasAnyPath)
    {
        std::cerr << "Error: Cannot animate - no units found paths" << std::endl;
        return false;
    }

    return true;
}

bool PathAnimator::animatePartialMultiUnitPaths(const BattleMap &battleMap, const PathfindingResult &result)
{
    if (!validatePartialMultiUnitAnimationInputs(battleMap, result))
    {
        return false;
    }

    // Count successful units
    int successfulUnits = 0;
    for (const auto &unit : result.units)
    {
        if (unit.pathFound)
            successfulUnits++;
    }

    std::cout << "\n=== Starting Partial Multi-Unit Path Animation ===\n";
    std::cout << "Successful units: " << successfulUnits << "/" << result.units.size()
              << " | Total time steps: " << result.totalSteps << "\n";
    std::cout << "Animation Style: ";

    switch (multiConfig.style)
    {
    case AnimationStyle::SIMPLE:
        std::cout << "Simple Movement";
        break;
    case AnimationStyle::WITH_TRAIL:
        std::cout << "With Trail";
        break;
    case AnimationStyle::NUMBERED_STEPS:
        std::cout << "Numbered Steps";
        break;
    case AnimationStyle::HIGHLIGHT_PATH:
        std::cout << "Highlight Path";
        break;
    }

    std::cout << " | Speed: " << static_cast<int>(multiConfig.speed) << "ms per step\n";
    std::cout << "Press Enter to start animation (Ctrl+C to stop)...\n";

    // Wait for Enter key to start
    std::string input;
    std::getline(std::cin, input);

    hideCursor();

    try
    {
        // Animate through all time steps
        for (size_t timeStep = 0; timeStep < result.stepByStepPositions.size(); ++timeStep)
        {
            displayMultiUnitFrame(battleMap, result, timeStep);

            if (timeStep < result.stepByStepPositions.size() - 1)
            {
                sleep(static_cast<int>(multiConfig.speed));
            }
        }

        // Show final state for a bit longer
        sleep(1500);

        // Display completion message
        std::cout << "\n";
        setColor(Colors::BRIGHT_GREEN);
        std::cout << "=== Partial Multi-Unit Animation Complete! ===";
        resetColor();
        std::cout << "\n"
                  << successfulUnits << " units successfully animated.\n";
    }
    catch (...)
    {
        showCursor();
        std::cerr << "\nPartial multi-unit animation interrupted!" << std::endl;
        return false;
    }

    showCursor();
    return true;
}

bool PathAnimator::animatePartialMultiUnitPathsStepByStep(const BattleMap &battleMap, const PathfindingResult &result)
{
    if (!validatePartialMultiUnitAnimationInputs(battleMap, result))
    {
        return false;
    }

    // Count successful units
    int successfulUnits = 0;
    for (const auto &unit : result.units)
    {
        if (unit.pathFound)
            successfulUnits++;
    }

    std::cout << "\n=== Step-by-Step Partial Multi-Unit Animation ===\n";
    std::cout << "Successful units: " << successfulUnits << "/" << result.units.size() << "\n";
    std::cout << "Press ENTER to advance to next step, 'q' to quit...\n\n";

    hideCursor();

    try
    {
        for (size_t timeStep = 0; timeStep < result.stepByStepPositions.size(); ++timeStep)
        {
            displayMultiUnitFrame(battleMap, result, timeStep);

            // Wait for user input
            std::cout << "\nPress ENTER for next step (or 'q' to quit): ";
            showCursor();

            std::string input;
            std::getline(std::cin, input);

            if (!input.empty() && (input[0] == 'q' || input[0] == 'Q'))
            {
                std::cout << "Partial multi-unit animation stopped by user.\n";
                return true;
            }

            hideCursor();
        }

        std::cout << "\n";
        setColor(Colors::BRIGHT_GREEN);
        std::cout << "=== Step-by-Step Partial Multi-Unit Animation Complete! ===";
        resetColor();
        std::cout << std::endl;
    }
    catch (...)
    {
        showCursor();
        std::cerr << "\nStep-by-step partial multi-unit animation interrupted!" << std::endl;
        return false;
    }

    showCursor();
    return true;
}

AnimationStyle PathAnimator::parseAnimationStyle(const std::string &styleStr)
{
    if (styleStr == "simple")
        return AnimationStyle::SIMPLE;
    else if (styleStr == "trail" || styleStr == "with_trail")
        return AnimationStyle::WITH_TRAIL;
    else if (styleStr == "numbered" || styleStr == "numbered_steps")
        return AnimationStyle::NUMBERED_STEPS;
    else if (styleStr == "highlight" || styleStr == "highlight_path")
        return AnimationStyle::HIGHLIGHT_PATH;
    else
    {
        std::cerr << "Warning: Unknown animation style '" << styleStr << "', using WITH_TRAIL" << std::endl;
        return AnimationStyle::WITH_TRAIL;
    }
}

AnimationSpeed PathAnimator::parseAnimationSpeed(const std::string &speedStr)
{
    if (speedStr == "very_slow" || speedStr == "veryslow")
        return AnimationSpeed::VERY_SLOW;
    else if (speedStr == "slow")
        return AnimationSpeed::SLOW;
    else if (speedStr == "normal" || speedStr == "medium")
        return AnimationSpeed::NORMAL;
    else if (speedStr == "fast")
        return AnimationSpeed::FAST;
    else if (speedStr == "very_fast" || speedStr == "veryfast")
        return AnimationSpeed::VERY_FAST;
    else
    {
        // Try to parse as number (milliseconds per step)
        try
        {
            int ms = std::stoi(speedStr);
            if (ms >= 1000)
                return AnimationSpeed::VERY_SLOW;
            else if (ms >= 500)
                return AnimationSpeed::SLOW;
            else if (ms >= 250)
                return AnimationSpeed::NORMAL;
            else if (ms >= 100)
                return AnimationSpeed::FAST;
            else
                return AnimationSpeed::VERY_FAST;
        }
        catch (...)
        {
            std::cerr << "Warning: Unknown animation speed '" << speedStr << "', using NORMAL" << std::endl;
            return AnimationSpeed::NORMAL;
        }
    }
}
