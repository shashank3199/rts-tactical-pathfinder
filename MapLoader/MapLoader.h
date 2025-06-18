/**
 * @file MapLoader.h
 * @brief JSON Battle Map Loader for C++ - Header file
 * @author Shashank Goyal
 * @version 1.0
 * @date 2025
 *
 * This file contains the header declarations for the MapLoader library,
 * which provides functionality to load, validate, and display JSON battle map files
 * for real-time strategy (RTS) games and pathfinding applications.
 *
 * @copyright Copyright (c) 2025. All rights reserved.
 */

#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <string>
#include <vector>
#include <climits>
#include <jsoncpp/json/json.h>

/**
 * @brief Represents a tileset used for rendering battle map tiles
 *
 * A tileset defines the visual properties and dimensions for map tiles,
 * including the source image and tile dimensions for proper rendering.
 */
struct Tileset
{
    std::string name;  ///< Name identifier for the tileset
    std::string image; ///< Path to the tileset image file
    int imageWidth;    ///< Width of the tileset image in pixels
    int imageHeight;   ///< Height of the tileset image in pixels
    int tileWidth;     ///< Width of individual tiles in pixels
    int tileHeight;    ///< Height of individual tiles in pixels
};

/**
 * @brief Represents a map layer containing battle terrain data
 *
 * A layer contains the actual battle map data as an array of integers,
 * where each integer represents a different type of terrain or game element.
 *
 * @details Tile values and their meanings:
 * - -1: Reachable terrain (units can move here)
 * - 0: Starting position for battle units
 * - 8: Target position for objectives
 * - 3: Elevated terrain (obstacles/impassable)
 * - Other values: Custom terrain types
 */
struct Layer
{
    std::string name;      ///< Descriptive name for this layer
    std::string tileset;   ///< Name of the associated tileset
    std::vector<int> data; ///< Battle map data array (row-major order)
    int width;             ///< Width of the layer in tiles
    int height;            ///< Height of the layer in tiles
};

/**
 * @brief Represents the canvas dimensions for the entire battle map
 *
 * The canvas defines the total pixel dimensions of the battle map,
 * which is used to calculate layer dimensions when combined with tileset information.
 */
struct Canvas
{
    int width;  ///< Total canvas width in pixels
    int height; ///< Total canvas height in pixels
};

/**
 * @brief Main class for loading and managing JSON battle map files
 *
 * The MapLoader class provides comprehensive functionality for loading battle maps
 * from JSON files, validating their structure, and providing access to map data
 * for pathfinding algorithms and game logic.
 *
 * @details Key Features:
 * - JSON parsing and validation
 * - Battle map visualization
 * - Terrain analysis and statistics
 * - Error handling and reporting
 * - Multiple output formats for integration
 *
 * @note This class requires the jsoncpp library for JSON parsing
 *
 * @warning Always check isMapLoaded() before accessing map data
 *
 * Example usage:
 * @code
 * MapLoader loader;
 * if (loader.loadFromFile("battle_map.json")) {
 *     loader.displayMapInfo();
 *     loader.displayMap(0);  // Display first layer
 *
 *     // Access specific tile
 *     int tile = loader.getTileAt(0, 5, 10);
 *     if (tile != MapLoader::INVALID_TILE) {
 *         // Process tile data
 *     }
 * }
 * @endcode
 */
class MapLoader
{
private:
    std::vector<Layer> layers;     ///< All map layers loaded from JSON
    std::vector<Tileset> tilesets; ///< All tilesets referenced by layers
    Canvas canvas;                 ///< Canvas dimensions for the map
    bool isLoaded;                 ///< Flag indicating successful map loading

    /**
     * @brief Parse and validate the root JSON object
     * @param root The root JSON value to parse
     * @return true if parsing succeeds, false otherwise
     *
     * This method orchestrates the parsing of all JSON sections and performs
     * comprehensive validation of the battle map structure.
     */
    bool parseJson(const Json::Value &root);

    /**
     * @brief Validate and parse tileset definitions from JSON
     * @param tilesets JSON array containing tileset definitions
     * @return true if all tilesets are valid, false otherwise
     *
     * Validates required fields and ensures all tileset dimensions are positive.
     */
    bool validateTilesets(const Json::Value &tilesets);

    /**
     * @brief Validate and parse layer definitions from JSON
     * @param layers JSON array containing layer definitions
     * @return true if all layers are valid, false otherwise
     *
     * Validates layer structure, data arrays, and ensures data size matches
     * calculated dimensions based on canvas and tileset information.
     */
    bool validateLayers(const Json::Value &layers);

    /**
     * @brief Validate and parse canvas dimensions from JSON
     * @param canvas JSON object containing canvas width and height
     * @return true if canvas is valid, false otherwise
     *
     * Ensures canvas dimensions are positive and properly formatted.
     */
    bool validateCanvas(const Json::Value &canvas);

    /**
     * @brief Find a tileset by its name identifier
     * @param name The name of the tileset to find
     * @return Pointer to the tileset if found, nullptr otherwise
     *
     * Used internally to resolve tileset references in layer definitions.
     */
    const Tileset *findTilesetByName(const std::string &name) const;

public:
    /**
     * @brief Error constant returned by getTileAt() for invalid positions
     *
     * This constant is returned when attempting to access tiles at invalid
     * coordinates or when no map is loaded.
     */
    static const int INVALID_TILE = INT_MIN;

    /**
     * @brief Default constructor
     *
     * Initializes an empty MapLoader instance. Use loadFromFile() or
     * loadFromString() to load actual battle map data.
     */
    MapLoader();

    // ==================== Main Functionality ====================

    /**
     * @brief Load battle map from a JSON file
     * @param filename Path to the JSON battle map file
     * @return true if loading succeeds, false otherwise
     *
     * Loads and validates a complete battle map from the specified JSON file.
     * The file must contain valid JSON with the expected battle map structure.
     *
     * @note Error messages are printed to std::cerr on failure
     *
     * @see loadFromString() for loading from string data
     */
    bool loadFromFile(const std::string &filename);

    /**
     * @brief Load battle map from a JSON string
     * @param jsonString JSON string containing battle map data
     * @return true if loading succeeds, false otherwise
     *
     * Parses and validates battle map data from a JSON string. Useful for
     * loading maps from embedded data or network sources.
     *
     * @see loadFromFile() for loading from files
     */
    bool loadFromString(const std::string &jsonString);

    /**
     * @brief Check if a battle map has been successfully loaded
     * @return true if a valid map is loaded, false otherwise
     *
     * Always check this before accessing map data to avoid errors.
     */
    bool isMapLoaded() const;

    // ==================== Data Access Methods ====================

    /**
     * @brief Get all map layers
     * @return Const reference to vector of all layers
     *
     * Provides direct access to layer data for advanced processing.
     *
     * @warning Only call after verifying isMapLoaded() returns true
     */
    const std::vector<Layer> &getLayers() const;

    /**
     * @brief Get all tileset definitions
     * @return Const reference to vector of all tilesets
     *
     * Provides access to tileset information for rendering purposes.
     *
     * @warning Only call after verifying isMapLoaded() returns true
     */
    const std::vector<Tileset> &getTilesets() const;

    /**
     * @brief Get canvas dimensions
     * @return Const reference to canvas structure
     *
     * Returns the pixel dimensions of the entire battle map canvas.
     *
     * @warning Only call after verifying isMapLoaded() returns true
     */
    const Canvas &getCanvas() const;

    // ==================== Display Methods ====================

    /**
     * @brief Display ASCII visualization of the battle map
     * @param layerIndex Index of the layer to display (default: 0)
     *
     * Renders an ASCII representation of the battle map in the terminal,
     * using different characters to represent various terrain types:
     * - "░░": Reachable terrain (-1)
     * - "S ": Starting positions (0)
     * - "T ": Target positions (8)
     * - "▲▲": Elevated terrain (3)
     * - Numbers: Other tile values
     *
     * @note Requires a terminal that supports Unicode characters
     *
     * @warning Prints error to stderr if no map is loaded or invalid layer index
     */
    void displayMap(int layerIndex = 0) const;

    /**
     * @brief Display comprehensive information about the loaded battle map
     *
     * Shows detailed statistics including:
     * - Canvas dimensions
     * - Number of layers and tilesets
     * - Tileset details (image paths, dimensions)
     * - Layer information (names, dimensions, data points)
     *
     * Useful for debugging and understanding map structure.
     *
     * @warning Prints error to stderr if no map is loaded
     */
    void displayMapInfo() const;

    // ==================== Utility Methods ====================

    /**
     * @brief Get the tile value at specific coordinates
     * @param layerIndex Index of the layer to query
     * @param x X-coordinate (0-based, left to right)
     * @param y Y-coordinate (0-based, top to bottom)
     * @return Tile value at the specified position, or INVALID_TILE if invalid
     *
     * Provides safe access to individual tile values with bounds checking.
     * Always check return value against INVALID_TILE before using.
     *
     * @note Coordinates are in tile units, not pixels
     */
    int getTileAt(int layerIndex, int x, int y) const;

    /**
     * @brief Get the width of the battle map in tiles
     * @return Width in tiles, or 0 if no map is loaded
     *
     * Returns the width of the first layer, which should be consistent
     * across all layers in a properly formatted battle map.
     */
    int getMapWidth() const;

    /**
     * @brief Get the height of the battle map in tiles
     * @return Height in tiles, or 0 if no map is loaded
     *
     * Returns the height of the first layer, which should be consistent
     * across all layers in a properly formatted battle map.
     */
    int getMapHeight() const;

    /**
     * @brief Clear all loaded map data and reset to initial state
     *
     * Removes all layers, tilesets, and canvas data, returning the
     * MapLoader to its initial unloaded state. Use this to prepare
     * for loading a different battle map.
     */
    void clear();
};

#endif // MAPLOADER_H