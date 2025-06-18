/**
 * @file MapLoader.cpp
 * @brief JSON Battle Map Loader for C++ - Implementation file
 * @author Shashank Goyal
 * @version 1.0
 * @date 2025
 *
 * This file contains the implementation of the MapLoader class, providing
 * comprehensive functionality for loading, validating, and displaying JSON
 * battle map files for RTS games and pathfinding applications.
 *
 * @copyright Copyright (c) 2025. All rights reserved.
 */

#include "MapLoader.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>

/**
 * @brief Default constructor implementation
 *
 * Initializes the MapLoader with default values and empty containers.
 * The canvas dimensions are set to 0 and isLoaded flag is set to false.
 */
MapLoader::MapLoader() : isLoaded(false)
{
    canvas.width = 0;
    canvas.height = 0;
}

/**
 * @brief Load battle map from JSON file implementation
 *
 * @param filename Path to the JSON file to load
 * @return true if file loading and parsing succeed, false otherwise
 *
 * @details Implementation steps:
 * 1. Opens the file using std::ifstream
 * 2. Uses jsoncpp CharReaderBuilder for parsing
 * 3. Delegates actual parsing to parseJson()
 *
 * @note Errors are logged to std::cerr with descriptive messages
 */
bool MapLoader::loadFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errs;

    if (!Json::parseFromStream(reader, file, &root, &errs))
    {
        std::cerr << "Error parsing JSON: " << errs << std::endl;
        return false;
    }

    return parseJson(root);
}

/**
 * @brief Load battle map from JSON string implementation
 *
 * @param jsonString JSON string containing map data
 * @return true if parsing succeeds, false otherwise
 *
 * @details Uses std::istringstream to create a stream from the string,
 * then follows the same parsing process as loadFromFile().
 */
bool MapLoader::loadFromString(const std::string &jsonString)
{
    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errs;
    std::istringstream stream(jsonString);

    if (!Json::parseFromStream(reader, stream, &root, &errs))
    {
        std::cerr << "Error parsing JSON: " << errs << std::endl;
        return false;
    }

    return parseJson(root);
}

/**
 * @brief Main JSON parsing and validation orchestrator
 *
 * @param root The root JSON value parsed from file or string
 * @return true if all validation passes, false otherwise
 *
 * @details Processing order (important for dependencies):
 * 1. Clear existing data
 * 2. Validate required top-level fields exist
 * 3. Parse canvas (needed for layer dimension calculations)
 * 4. Parse tilesets (needed for layer validation)
 * 5. Parse layers (depends on canvas and tileset data)
 * 6. Set loaded flag on success
 *
 * @note The order of validation is critical as layers depend on
 * both canvas and tileset information for dimension calculations.
 */
bool MapLoader::parseJson(const Json::Value &root)
{
    clear();

    // Validate required fields
    if (!root.isMember("layers") || !root.isMember("tilesets") || !root.isMember("canvas"))
    {
        std::cerr << "Error: Missing required fields (layers, tilesets, or canvas)" << std::endl;
        return false;
    }

    // Parse and validate each section - order matters: canvas first, then tilesets, then layers
    if (!validateCanvas(root["canvas"]))
    {
        return false;
    }

    if (!validateTilesets(root["tilesets"]))
    {
        return false;
    }

    if (!validateLayers(root["layers"]))
    {
        return false;
    }

    isLoaded = true;
    std::cout << "Map loaded successfully!" << std::endl;
    return true;
}

/**
 * @brief Validate canvas dimensions from JSON
 *
 * @param canvasJson JSON object containing canvas definition
 * @return true if canvas is valid, false otherwise
 *
 * @details Validation checks:
 * - Presence of required "width" and "height" fields
 * - Both dimensions must be positive integers
 *
 * Canvas dimensions are stored in pixel units and used later
 * to calculate layer dimensions in combination with tileset data.
 */
bool MapLoader::validateCanvas(const Json::Value &canvasJson)
{
    if (!canvasJson.isMember("width") || !canvasJson.isMember("height"))
    {
        std::cerr << "Error: Canvas missing width or height" << std::endl;
        return false;
    }

    canvas.width = canvasJson["width"].asInt();
    canvas.height = canvasJson["height"].asInt();

    if (canvas.width <= 0 || canvas.height <= 0)
    {
        std::cerr << "Error: Canvas dimensions must be positive" << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief Validate tileset definitions from JSON array
 *
 * @param tilesetsJson JSON array containing tileset objects
 * @return true if all tilesets are valid, false otherwise
 *
 * @details For each tileset, validates:
 * - Required fields: name, image, imagewidth, imageheight, tilewidth, tileheight
 * - All dimension values must be positive
 * - Creates and stores Tileset struct for each valid definition
 *
 * Tilesets define how the raw tile data should be rendered and
 * provide the tile dimensions needed for layer size calculations.
 */
bool MapLoader::validateTilesets(const Json::Value &tilesetsJson)
{
    if (!tilesetsJson.isArray())
    {
        std::cerr << "Error: Tilesets must be an array" << std::endl;
        return false;
    }

    for (const auto &tilesetJson : tilesetsJson)
    {
        Tileset tileset;

        if (!tilesetJson.isMember("name") || !tilesetJson.isMember("image") ||
            !tilesetJson.isMember("imagewidth") || !tilesetJson.isMember("imageheight") ||
            !tilesetJson.isMember("tilewidth") || !tilesetJson.isMember("tileheight"))
        {
            std::cerr << "Error: Tileset missing required fields" << std::endl;
            return false;
        }

        tileset.name = tilesetJson["name"].asString();
        tileset.image = tilesetJson["image"].asString();
        tileset.imageWidth = tilesetJson["imagewidth"].asInt();
        tileset.imageHeight = tilesetJson["imageheight"].asInt();
        tileset.tileWidth = tilesetJson["tilewidth"].asInt();
        tileset.tileHeight = tilesetJson["tileheight"].asInt();

        if (tileset.imageWidth <= 0 || tileset.imageHeight <= 0 ||
            tileset.tileWidth <= 0 || tileset.tileHeight <= 0)
        {
            std::cerr << "Error: Tileset dimensions must be positive" << std::endl;
            return false;
        }

        tilesets.push_back(tileset);
    }

    return true;
}

/**
 * @brief Validate layer definitions and battle map data
 *
 * @param layersJson JSON array containing layer objects
 * @return true if all layers are valid, false otherwise
 *
 * @details Complex validation process for each layer:
 * 1. Validate required fields: name, tileset, data
 * 2. Parse data array of integers (battle map values)
 * 3. Resolve tileset reference by name
 * 4. Calculate expected layer dimensions using canvas and tileset info
 * 5. Verify data array size matches calculated dimensions
 *
 * The data array contains the actual battle map information where:
 * - -1: Reachable terrain
 * - 0: Starting positions
 * - 8: Target positions
 * - 3: Elevated terrain/obstacles
 * - Other values: Custom terrain types
 *
 * @warning Layer dimensions are calculated as: canvas_size / tile_size
 * This assumes the canvas exactly fits an integer number of tiles.
 */
bool MapLoader::validateLayers(const Json::Value &layersJson)
{
    if (!layersJson.isArray())
    {
        std::cerr << "Error: Layers must be an array" << std::endl;
        return false;
    }

    for (const auto &layerJson : layersJson)
    {
        Layer layer;

        if (!layerJson.isMember("name") || !layerJson.isMember("tileset") ||
            !layerJson.isMember("data"))
        {
            std::cerr << "Error: Layer missing required fields" << std::endl;
            return false;
        }

        layer.name = layerJson["name"].asString();
        layer.tileset = layerJson["tileset"].asString();

        // Parse data array
        const Json::Value &dataJson = layerJson["data"];
        if (!dataJson.isArray())
        {
            std::cerr << "Error: Layer data must be an array" << std::endl;
            return false;
        }

        for (const auto &value : dataJson)
        {
            layer.data.push_back(value.asInt());
        }

        if (layer.data.empty())
        {
            std::cerr << "Error: Layer data cannot be empty" << std::endl;
            return false;
        }

        // Calculate layer dimensions using tileset information
        const Tileset *tileset = findTilesetByName(layer.tileset);
        if (tileset == nullptr)
        {
            std::cerr << "Error: Could not find tileset '" << layer.tileset << "' for layer '" << layer.name << "'" << std::endl;
            return false;
        }

        // Calculate map dimensions in tiles
        layer.width = canvas.width / tileset->tileWidth;
        layer.height = canvas.height / tileset->tileHeight;

        // Validate that the data size matches the calculated dimensions
        int expectedDataSize = layer.width * layer.height;
        if (static_cast<int>(layer.data.size()) != expectedDataSize)
        {
            std::cerr << "Error: Layer data size (" << layer.data.size()
                      << ") doesn't match calculated dimensions (" << layer.width
                      << "x" << layer.height << " = " << expectedDataSize << ")" << std::endl;
            std::cerr << "Canvas: " << canvas.width << "x" << canvas.height
                      << ", Tile size: " << tileset->tileWidth << "x" << tileset->tileHeight << std::endl;
            return false;
        }

        layers.push_back(layer);
    }

    return true;
}

/**
 * @brief Find tileset by name for layer validation
 *
 * @param name Name of the tileset to find
 * @return Pointer to found tileset, or nullptr if not found
 *
 * @details Linear search through the tilesets vector to find
 * a tileset with matching name. Used during layer validation
 * to resolve tileset references.
 *
 * @note Returns nullptr for invalid names - caller must check
 */
const Tileset *MapLoader::findTilesetByName(const std::string &name) const
{
    for (const auto &tileset : tilesets)
    {
        if (tileset.name == name)
        {
            return &tileset;
        }
    }
    return nullptr;
}

/**
 * @brief Check if map is successfully loaded
 *
 * @return Current loading status
 *
 * Simple getter for the isLoaded flag, which is set to true
 * only after successful completion of all parsing and validation.
 */
bool MapLoader::isMapLoaded() const
{
    return isLoaded;
}

/**
 * @brief Get reference to all layers
 *
 * @return Const reference to layers vector
 *
 * Provides direct access to layer data for advanced processing.
 * The vector contains all successfully loaded and validated layers.
 */
const std::vector<Layer> &MapLoader::getLayers() const
{
    return layers;
}

/**
 * @brief Get reference to all tilesets
 *
 * @return Const reference to tilesets vector
 *
 * Provides access to tileset definitions for rendering purposes.
 * All tilesets referenced by layers are guaranteed to be present.
 */
const std::vector<Tileset> &MapLoader::getTilesets() const
{
    return tilesets;
}

/**
 * @brief Get reference to canvas dimensions
 *
 * @return Const reference to canvas structure
 *
 * Returns the pixel dimensions that define the total map area.
 */
const Canvas &MapLoader::getCanvas() const
{
    return canvas;
}

/**
 * @brief Display ASCII battle map visualization
 *
 * @param layerIndex Index of layer to display (0-based)
 *
 * @details Renders the battle map using Unicode characters:
 * - "░░" (U+2591): Reachable positions (tile value -1)
 * - "S ": Starting positions (tile value 0)
 * - "T ": Target positions (tile value 8)
 * - "▲▲" (U+25B2): Elevated terrain/obstacles (tile value 3)
 * - Numbers: Other tile values displayed directly
 *
 * The output format is designed for terminal display with each
 * tile represented by 2 characters for consistent alignment.
 *
 * @warning Requires terminal with Unicode support for proper display
 */
void MapLoader::displayMap(int layerIndex) const
{
    if (!isLoaded)
    {
        std::cerr << "Error: No map loaded" << std::endl;
        return;
    }

    if (layerIndex < 0 || layerIndex >= static_cast<int>(layers.size()))
    {
        std::cerr << "Error: Invalid layer index" << std::endl;
        return;
    }

    const Layer &layer = layers[layerIndex];
    std::cout << "\n=== Displaying Battle Map Layer: " << layer.name << " ===\n";
    std::cout << "Dimensions: " << layer.width << "x" << layer.height << "\n\n";

    for (int y = 0; y < layer.height; ++y)
    {
        for (int x = 0; x < layer.width; ++x)
        {
            int tile = layer.data[y * layer.width + x];

            // Display different characters for battle map elements
            if (tile == -1)
            {
                std::cout << "░░"; // Reachable positions
            }
            else if (tile == 0)
            {
                std::cout << "S "; // Starting position for battle unit
            }
            else if (tile == 8)
            {
                std::cout << "T "; // Target position
            }
            else if (tile == 3)
            {
                std::cout << "▲▲"; // Elevated terrain
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

/**
 * @brief Display comprehensive map information and statistics
 *
 * @details Output includes:
 * - Canvas pixel dimensions
 * - Count of layers and tilesets
 * - Detailed tileset information (names, images, dimensions)
 * - Layer details (names, associated tilesets, tile dimensions, data size)
 *
 * This method is invaluable for debugging map loading issues and
 * understanding the structure of complex battle maps.
 */
void MapLoader::displayMapInfo() const
{
    if (!isLoaded)
    {
        std::cerr << "Error: No map loaded" << std::endl;
        return;
    }

    std::cout << "\n=== Map Information ===\n";
    std::cout << "Canvas Size: " << canvas.width << "x" << canvas.height << "\n";
    std::cout << "Number of Layers: " << layers.size() << "\n";
    std::cout << "Number of Tilesets: " << tilesets.size() << "\n\n";

    std::cout << "=== Tilesets ===\n";
    for (size_t i = 0; i < tilesets.size(); ++i)
    {
        const Tileset &ts = tilesets[i];
        std::cout << i << ". " << ts.name << "\n";
        std::cout << "   Image: " << ts.image << "\n";
        std::cout << "   Image Size: " << ts.imageWidth << "x" << ts.imageHeight << "\n";
        std::cout << "   Tile Size: " << ts.tileWidth << "x" << ts.tileHeight << "\n\n";
    }

    std::cout << "=== Layers ===\n";
    for (size_t i = 0; i < layers.size(); ++i)
    {
        const Layer &layer = layers[i];
        std::cout << i << ". " << layer.name << "\n";
        std::cout << "   Tileset: " << layer.tileset << "\n";
        std::cout << "   Dimensions: " << layer.width << "x" << layer.height << "\n";
        std::cout << "   Data Points: " << layer.data.size() << "\n\n";
    }
}

/**
 * @brief Get tile value at specific coordinates with bounds checking
 *
 * @param layerIndex Index of the layer (0-based)
 * @param x X-coordinate in tiles (0-based, left to right)
 * @param y Y-coordinate in tiles (0-based, top to bottom)
 * @return Tile value, or INVALID_TILE if position is invalid
 *
 * @details Comprehensive bounds checking:
 * - Verifies map is loaded
 * - Validates layer index
 * - Validates x,y coordinates within layer bounds
 * - Uses row-major order: index = y * width + x
 *
 * @note Always check return value against INVALID_TILE before use
 */
int MapLoader::getTileAt(int layerIndex, int x, int y) const
{
    if (!isLoaded || layerIndex < 0 || layerIndex >= static_cast<int>(layers.size()))
    {
        return INVALID_TILE;
    }

    const Layer &layer = layers[layerIndex];
    if (x < 0 || x >= layer.width || y < 0 || y >= layer.height)
    {
        return INVALID_TILE;
    }

    return layer.data[y * layer.width + x];
}

/**
 * @brief Get map width in tiles from first layer
 *
 * @return Width in tiles, or 0 if no map loaded
 *
 * Uses the first layer's width, which should be consistent across
 * all layers in a properly formatted battle map.
 */
int MapLoader::getMapWidth() const
{
    if (!isLoaded || layers.empty())
    {
        return 0;
    }
    return layers[0].width;
}

/**
 * @brief Get map height in tiles from first layer
 *
 * @return Height in tiles, or 0 if no map loaded
 *
 * Uses the first layer's height, which should be consistent across
 * all layers in a properly formatted battle map.
 */
int MapLoader::getMapHeight() const
{
    if (!isLoaded || layers.empty())
    {
        return 0;
    }
    return layers[0].height;
}

/**
 * @brief Clear all map data and reset to unloaded state
 *
 * @details Performs complete cleanup:
 * - Clears layers vector
 * - Clears tilesets vector
 * - Resets canvas dimensions to 0
 * - Sets isLoaded flag to false
 *
 * After calling this method, the MapLoader is ready to load
 * a new battle map without any residual data from previous maps.
 */
void MapLoader::clear()
{
    layers.clear();
    tilesets.clear();
    canvas.width = 0;
    canvas.height = 0;
    isLoaded = false;
}