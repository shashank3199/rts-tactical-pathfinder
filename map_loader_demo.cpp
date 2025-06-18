/**
 * @file map_loader_demo.cpp
 * @brief Demonstration program for the JSON Battle Map Loader functionality
 * @author Shashank Goyal
 * @date 2025
 * @version 1.0
 *
 * This program demonstrates how to use the MapLoader class to load, validate,
 * and analyze JSON battle map files. It provides comprehensive analysis of
 * map structure, terrain distribution, and strategic positions.
 *
 * @copyright Copyright (c) 2025. All rights reserved.
 */

#include "MapLoader/MapLoader.h"
#include <iostream>
#include <string>

/**
 * @brief Prints usage information for the map loader demo program
 *
 * Displays the correct command-line syntax and provides usage examples
 * for running the map loader demonstration.
 *
 * @param programName The name of the executable program
 */
void printUsage(const char *programName)
{
    std::cout << "Usage: " << programName << " <map_file.json>" << std::endl;
    std::cout << "Example: " << programName << " battle_map.json" << std::endl;
}

/**
 * @brief Main entry point for the map loader demonstration program
 *
 * This function demonstrates the complete workflow of loading and analyzing
 * a JSON battle map file. It performs the following operations:
 * 1. Validates command-line arguments
 * 2. Loads the specified JSON map file
 * 3. Displays comprehensive map information and statistics
 * 4. Performs terrain analysis and strategic position detection
 * 5. Provides detailed coverage and element distribution analysis
 *
 * The program serves as both a demonstration of the MapLoader capabilities
 * and a utility for validating and analyzing battle map files.
 *
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return int Exit code (0 for success, 1 for failure)
 *
 * @note Requires exactly one argument: the path to a JSON map file
 * @see MapLoader class for detailed map loading functionality
 */
int main(int argc, char *argv[])
{
    // Validate command-line arguments
    if (argc != 2)
    {
        std::cerr << "Error: Map file argument required." << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    // Initialize map loader and extract filename
    MapLoader mapLoader;
    std::string filename = argv[1];

    std::cout << "=== JSON Battle Map Loader ===" << std::endl;
    std::cout << "Loading battle map from file: " << filename << std::endl;

    // Attempt to load the map file
    bool success = mapLoader.loadFromFile(filename);

    if (!success)
    {
        std::cerr << "Failed to load battle map from file: " << filename << std::endl;
        return 1;
    }

    // Display comprehensive map information
    mapLoader.displayMapInfo();

    // Display the battle map with legend
    std::cout << "\n=== Battle Map Visualization ===\n";
    std::cout << "Legend:\n";
    std::cout << "  '░░' = Reachable positions (-1)\n";
    std::cout << "  'S ' = Starting position for battle unit (0)\n";
    std::cout << "  'T ' = Target position (8)\n";
    std::cout << "  '▲▲' = Elevated terrain (3)\n";
    std::cout << "  Numbers = Other battle elements\n\n";

    mapLoader.displayMap(0);

    // Perform detailed battle map analysis
    std::cout << "\n=== Battle Map Analysis ===\n";
    std::cout << "Map dimensions: " << mapLoader.getMapWidth() << "x" << mapLoader.getMapHeight() << std::endl;

    // Analyze terrain distribution and element statistics
    const std::vector<Layer> &layers = mapLoader.getLayers();
    if (!layers.empty())
    {
        const Layer &layer = layers[0];
        int reachableCount = 0, startCount = 0, targetCount = 0, elevatedCount = 0, otherCount = 0;

        // Count different battle map elements
        for (int tile : layer.data)
        {
            if (tile == -1)
                reachableCount++;
            else if (tile == 0)
                startCount++;
            else if (tile == 8)
                targetCount++;
            else if (tile == 3)
                elevatedCount++;
            else
                otherCount++;
        }

        // Display element statistics
        std::cout << "Battle Map Element Statistics:\n";
        std::cout << "  Reachable positions: " << reachableCount << std::endl;
        std::cout << "  Starting positions: " << startCount << std::endl;
        std::cout << "  Target positions: " << targetCount << std::endl;
        std::cout << "  Elevated terrain: " << elevatedCount << std::endl;
        std::cout << "  Other elements: " << otherCount << std::endl;

        // Calculate and display coverage percentages
        double totalTiles = layer.data.size();
        std::cout << "Coverage Analysis:\n";
        std::cout << "  Reachable area: " << (reachableCount / totalTiles * 100) << "%" << std::endl;
        std::cout << "  Elevated terrain: " << (elevatedCount / totalTiles * 100) << "%" << std::endl;
        std::cout << "  Strategic positions: " << ((startCount + targetCount) / totalTiles * 100) << "%" << std::endl;
    }

    // Perform strategic position analysis
    std::cout << "\n=== Strategic Position Analysis ===\n";
    const std::vector<Layer> &mapLayers = mapLoader.getLayers();
    if (!mapLayers.empty())
    {
        const Layer &layer = mapLayers[0];
        bool foundStart = false, foundTarget = false;

        // Search for strategic positions in the map
        for (int y = 0; y < layer.height && (!foundStart || !foundTarget); ++y)
        {
            for (int x = 0; x < layer.width && (!foundStart || !foundTarget); ++x)
            {
                int tile = mapLoader.getTileAt(0, x, y);
                if (tile == 0 && !foundStart)
                {
                    std::cout << "Starting position found at coordinates (" << x << "," << y << ")" << std::endl;
                    foundStart = true;
                }
                else if ((tile == 8) && !foundTarget)
                {
                    std::cout << "Target position found at coordinates (" << x << "," << y << ")" << std::endl;
                    foundTarget = true;
                }
            }
        }

        // Report missing strategic positions
        if (!foundStart)
            std::cout << "No starting position (0) found in the map" << std::endl;
        if (!foundTarget)
            std::cout << "No target position (8) found in the map" << std::endl;
    }

    std::cout << "\n=== Battle map loading completed successfully! ===\n";

    return 0;
}