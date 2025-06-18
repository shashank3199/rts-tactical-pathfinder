# ==============================================================================
# JSON Battle Map Loader and Pathfinding Solver - Makefile
# ==============================================================================
#
# This Makefile provides comprehensive build and testing capabilities for the
# RTS Battle Unit Pathfinding System. It supports both map loading
# demonstrations and advanced pathfinding with multi-unit coordination.
#
# Project Structure:
#   - maploader: Simple map loading and analysis tool
#   - pathfinder: Advanced pathfinding solver with multi-unit support
#
# Dependencies:
#   - g++ compiler with C++11 support
#   - libjsoncpp-dev (JSON parsing library)
#   - build-essential (compilation tools)
#
# Author: Shashank Goyal
# Version: 2.0
# ==============================================================================

# ------------------------------------------------------------------------------
# Compiler Configuration
# ------------------------------------------------------------------------------

# C++ compiler (GNU g++)
CXX = g++

# Compiler flags for optimal performance and debugging
# -std=c++11        : Use C++11 standard
# -Wall -Wextra     : Enable comprehensive warning messages
# -O2               : Optimize for performance
# -I<dir>           : Add include directories for each module
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -IMapLoader -IPathFinder -IPathAnimator -IMultiUnitPathFinder

# External libraries required for linking
# -ljsoncpp         : JSON parsing and manipulation library
LIBS = -ljsoncpp

# Build directory for intermediate object files
BUILD_DIR = build

# ------------------------------------------------------------------------------
# Target Executable Names
# ------------------------------------------------------------------------------

# Map loader demonstration executable
MAPLOADER_TARGET = maploader

# Advanced pathfinding solver executable
PATHFINDER_TARGET = pathfinder

# ------------------------------------------------------------------------------
# Source File Organization
# ------------------------------------------------------------------------------

# Source files for the map loader demonstration
MAPLOADER_SOURCES = map_loader_demo.cpp MapLoader/MapLoader.cpp

# Source files for the advanced pathfinding solver
PATHFINDER_SOURCES = main.cpp MapLoader/MapLoader.cpp PathFinder/PathFinder.cpp PathAnimator/PathAnimator.cpp MultiUnitPathFinder/MultiUnitPathFinder.cpp

# ------------------------------------------------------------------------------
# Object File Configuration
# ------------------------------------------------------------------------------

# Object files for map loader (placed in build directory)
MAPLOADER_OBJECTS = $(MAPLOADER_SOURCES:%.cpp=$(BUILD_DIR)/%.o)

# Object files for pathfinder (placed in build directory)
PATHFINDER_OBJECTS = $(PATHFINDER_SOURCES:%.cpp=$(BUILD_DIR)/%.o)

# ------------------------------------------------------------------------------
# Header File Dependencies
# ------------------------------------------------------------------------------

# All header files that may trigger recompilation
HEADERS = MapLoader/MapLoader.h PathFinder/PathFinder.h PathAnimator/PathAnimator.h MultiUnitPathFinder/MultiUnitPathFinder.h

# ==============================================================================
# Primary Build Targets
# ==============================================================================

# Default target: Build both executables
all: $(MAPLOADER_TARGET) $(PATHFINDER_TARGET)

# Create build directory structure
$(BUILD_DIR):
	@echo "Creating build directory structure..."
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/MapLoader
	mkdir -p $(BUILD_DIR)/PathFinder
	mkdir -p $(BUILD_DIR)/PathAnimator
	mkdir -p $(BUILD_DIR)/MultiUnitPathFinder

# Build the map loader demonstration executable
$(MAPLOADER_TARGET): $(MAPLOADER_OBJECTS)
	@echo "Linking map loader executable..."
	$(CXX) $(MAPLOADER_OBJECTS) -o $(MAPLOADER_TARGET) $(LIBS)
	@echo "Map loader build complete: $(MAPLOADER_TARGET)"

# Build the advanced pathfinding solver executable
$(PATHFINDER_TARGET): $(PATHFINDER_OBJECTS)
	@echo "Linking pathfinder executable..."
	$(CXX) $(PATHFINDER_OBJECTS) -o $(PATHFINDER_TARGET) $(LIBS)
	@echo "Pathfinder build complete: $(PATHFINDER_TARGET)"

# Compile source files to object files with dependency tracking
$(BUILD_DIR)/%.o: %.cpp $(HEADERS) | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ==============================================================================
# Utility Targets
# ==============================================================================

# Remove all build artifacts and executables
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR) $(MAPLOADER_TARGET) $(PATHFINDER_TARGET)
	@echo "Clean complete."

# Install required dependencies on Ubuntu/Debian systems
install-deps:
	@echo "Installing required dependencies..."
	sudo apt update
	sudo apt install -y libjsoncpp-dev build-essential
	@echo "Dependencies installed successfully."

# ==============================================================================
# Execution Targets
# ==============================================================================

# Run the map loader with a specified map file
# Usage: make run-maploader FILE=your_map.json
run-maploader:
	@echo "Usage: make run-maploader FILE=your_map.json"
	@if [ -n "$(FILE)" ]; then \
		echo "Running map loader with file: $(FILE)"; \
		./$(MAPLOADER_TARGET) $(FILE); \
	else \
		echo "Please specify a map file: make run-maploader FILE=map.json"; \
	fi

# Run the pathfinder with configuration options
# Usage: make run-pathfinder FILE=map.json [ALGO=astar] [MOVE=rdlu] [ANIMATE=yes] [MULTI=yes] [STRATEGY=sequential]
run-pathfinder:
	@echo "Usage: make run-pathfinder FILE=your_map.json [ALGO=astar|bfs|dfs|all] [MOVE=rdlu|uldr|etc] [ANIMATE=yes|step] [MULTI=yes] [STRATEGY=sequential|priority|cooperative|wait]"
	@if [ -n "$(FILE)" ]; then \
		CMD="./$(PATHFINDER_TARGET) $(FILE)"; \
		if [ -n "$(ALGO)" ]; then CMD="$$CMD --algorithm $(ALGO)"; fi; \
		if [ -n "$(MOVE)" ]; then CMD="$$CMD --move-order $(MOVE)"; fi; \
		if [ "$(MULTI)" = "yes" ]; then CMD="$$CMD --multi-unit"; fi; \
		if [ -n "$(STRATEGY)" ]; then CMD="$$CMD --strategy $(STRATEGY)"; fi; \
		if [ "$(ANIMATE)" = "yes" ]; then CMD="$$CMD --animate"; fi; \
		if [ "$(ANIMATE)" = "step" ]; then CMD="$$CMD --step-by-step"; fi; \
		echo "Executing: $$CMD"; \
		eval $$CMD; \
	else \
		echo "Please specify a map file: make run-pathfinder FILE=map.json"; \
	fi

# ==============================================================================
# Testing and Analysis Targets
# ==============================================================================

# Test different movement direction orders with a single algorithm
# Usage: make test-move-orders FILE=map.json [ALGO=dfs] [ANIMATE=yes|step]
test-move-orders:
	@echo "Usage: make test-move-orders FILE=your_map.json [ALGO=dfs] [ANIMATE=yes|step]"
	@if [ -n "$(FILE)" ]; then \
		echo "Testing different move orders with $(FILE)..."; \
		for order in rdlu uldr ldru dlur; do \
			echo "Testing move order: $$order"; \
			CMD="./$(PATHFINDER_TARGET) $(FILE) --move-order $$order"; \
			if [ -n "$(ALGO)" ]; then CMD="$$CMD --algorithm $(ALGO)"; fi; \
			if [ "$(ANIMATE)" = "yes" ]; then CMD="$$CMD --animate"; fi; \
			if [ "$(ANIMATE)" = "step" ]; then CMD="$$CMD --step-by-step"; fi; \
			eval $$CMD; \
			echo ""; \
		done; \
	else \
		echo "Please specify: make test-move-orders FILE=map.json"; \
	fi

# Test multi-unit pathfinding with specified strategy
# Usage: make test-multi-unit FILE=map.json [STRATEGY=sequential|priority|cooperative|wait] [ANIMATE=yes|step]
test-multi-unit:
	@echo "Usage: make test-multi-unit FILE=your_map.json [STRATEGY=sequential|priority|cooperative|wait] [ANIMATE=yes|step]"
	@if [ -n "$(FILE)" ]; then \
		CMD="./$(PATHFINDER_TARGET) $(FILE) --multi-unit"; \
		if [ -n "$(STRATEGY)" ]; then CMD="$$CMD --strategy $(STRATEGY)"; fi; \
		if [ "$(ANIMATE)" = "yes" ]; then CMD="$$CMD --animate"; fi; \
		if [ "$(ANIMATE)" = "step" ]; then CMD="$$CMD --step-by-step"; fi; \
		echo "Executing: $$CMD"; \
		eval $$CMD; \
	else \
		echo "Please specify: make test-multi-unit FILE=map.json"; \
	fi

# Test all multi-unit conflict resolution strategies
# Usage: make test-all-strategies FILE=map.json [ANIMATE=yes|step]
test-all-strategies:
	@echo "Usage: make test-all-strategies FILE=your_map.json [ANIMATE=yes|step]"
	@if [ -n "$(FILE)" ]; then \
		echo "Testing all multi-unit strategies with $(FILE)..."; \
		for strategy in sequential priority cooperative wait; do \
			echo "Testing strategy: $$strategy"; \
			CMD="./$(PATHFINDER_TARGET) $(FILE) --multi-unit --speed fast --strategy $$strategy"; \
			if [ "$(ANIMATE)" = "yes" ]; then CMD="$$CMD --animate"; fi; \
			if [ "$(ANIMATE)" = "step" ]; then CMD="$$CMD --step-by-step"; fi; \
			eval $$CMD; \
			echo ""; \
		done; \
	else \
		echo "Please specify: make test-all-strategies FILE=map.json"; \
	fi

# ==============================================================================
# Algorithm Testing Targets
# ==============================================================================

# Test A* pathfinding algorithm
# Usage: make test-astar FILE=map.json [MOVE=rdlu] [ANIMATE=yes|step]
test-astar:
	@echo "Usage: make test-astar FILE=your_map.json [MOVE=rdlu] [ANIMATE=yes|step]"
	@if [ -n "$(FILE)" ]; then \
		CMD="./$(PATHFINDER_TARGET) $(FILE) --algorithm astar"; \
		if [ -n "$(MOVE)" ]; then CMD="$$CMD --move-order $(MOVE)"; fi; \
		if [ "$(ANIMATE)" = "yes" ]; then CMD="$$CMD --animate"; fi; \
		if [ "$(ANIMATE)" = "step" ]; then CMD="$$CMD --step-by-step"; fi; \
		eval $$CMD; \
	else \
		echo "Please specify: make test-astar FILE=map.json"; \
	fi

# Test BFS pathfinding algorithm
# Usage: make test-bfs FILE=map.json [MOVE=rdlu] [ANIMATE=yes|step]
test-bfs:
	@echo "Usage: make test-bfs FILE=your_map.json [MOVE=rdlu] [ANIMATE=yes|step]"
	@if [ -n "$(FILE)" ]; then \
		CMD="./$(PATHFINDER_TARGET) $(FILE) --algorithm bfs"; \
		if [ -n "$(MOVE)" ]; then CMD="$$CMD --move-order $(MOVE)"; fi; \
		if [ "$(ANIMATE)" = "yes" ]; then CMD="$$CMD --animate"; fi; \
		if [ "$(ANIMATE)" = "step" ]; then CMD="$$CMD --step-by-step"; fi; \
		eval $$CMD; \
	else \
		echo "Please specify: make test-bfs FILE=map.json"; \
	fi

# Test DFS pathfinding algorithm
# Usage: make test-dfs FILE=map.json [MOVE=rdlu] [ANIMATE=yes|step]
test-dfs:
	@echo "Usage: make test-dfs FILE=your_map.json [MOVE=rdlu] [ANIMATE=yes|step]"
	@if [ -n "$(FILE)" ]; then \
		CMD="./$(PATHFINDER_TARGET) $(FILE) --algorithm dfs"; \
		if [ -n "$(MOVE)" ]; then CMD="$$CMD --move-order $(MOVE)"; fi; \
		if [ "$(ANIMATE)" = "yes" ]; then CMD="$$CMD --animate"; fi; \
		if [ "$(ANIMATE)" = "step" ]; then CMD="$$CMD --step-by-step"; fi; \
		eval $$CMD; \
	else \
		echo "Please specify: make test-dfs FILE=map.json"; \
	fi

# Test all pathfinding algorithms and compare results
# Usage: make test-all FILE=map.json [MOVE=rdlu] [ANIMATE=yes|step]
test-all:
	@echo "Usage: make test-all FILE=your_map.json [MOVE=rdlu] [ANIMATE=yes|step]"
	@if [ -n "$(FILE)" ]; then \
		CMD="./$(PATHFINDER_TARGET) $(FILE) --algorithm all"; \
		if [ -n "$(MOVE)" ]; then CMD="$$CMD --move-order $(MOVE)"; fi; \
		if [ "$(ANIMATE)" = "yes" ]; then CMD="$$CMD --animate"; fi; \
		if [ "$(ANIMATE)" = "step" ]; then CMD="$$CMD --step-by-step"; fi; \
		eval $$CMD; \
	else \
		echo "Please specify: make test-all FILE=map.json"; \
	fi

# ==============================================================================
# Documentation and Help
# ==============================================================================

# Display comprehensive help information
help:
	@echo "=== RTS Battle Map Loader and Pathfinding Solver ==="
	@echo ""
	@echo "Available targets:"
	@echo "  all                  - Build both maploader and pathfinder executables"
	@echo "  maploader           - Build only the map loader"
	@echo "  pathfinder          - Build only the pathfinder"
	@echo "  clean               - Remove all build artifacts"
	@echo "  install-deps        - Install required dependencies"
	@echo ""
	@echo "Running programs:"
	@echo "  run-maploader       - Run map loader (requires FILE=...)"
	@echo "  run-pathfinder      - Run pathfinder with options"
	@echo ""
	@echo "Features:"
	@echo "  test-move-orders    - Test different movement direction orders"
	@echo "  test-multi-unit     - Test multi-unit pathfinding"
	@echo "  test-all-strategies - Test all multi-unit conflict resolution strategies"
	@echo ""
	@echo "Quick pathfinding tests:"
	@echo "  test-astar          - Run A* algorithm"
	@echo "  test-bfs            - Run BFS algorithm"
	@echo "  test-dfs            - Run DFS algorithm"
	@echo "  test-all            - Run all algorithms and compare"
	@echo ""
	@echo "Parameters:"
	@echo "  FILE=filename       - Map file to use"
	@echo "  ALGO=algorithm      - Algorithm: astar, bfs, dfs, all"
	@echo "  MOVE=order          - Move direction order: rdlu, uldr, ldru, dlur, etc."
	@echo "  MULTI=yes           - Enable multi-unit mode"
	@echo "  STRATEGY=strategy   - Multi-unit strategy: sequential, priority, cooperative, wait"
	@echo "  ANIMATE=yes/step    - Animation: yes (auto), step (manual), or omit for none"
	@echo ""
	@echo "Usage Examples:"
	@echo "  make run-pathfinder FILE=map.json ALGO=astar MOVE=uldr ANIMATE=yes"
	@echo "  make test-move-orders FILE=map.json ALGO=bfs"
	@echo "  make test-multi-unit FILE=map.json STRATEGY=priority ANIMATE=step"
	@echo "  make test-all-strategies FILE=map.json"
	@echo ""
	@echo "Move Direction Orders:"
	@echo "  rdlu - Right, Down, Left, Up (default)"
	@echo "  uldr - Up, Left, Down, Right"
	@echo "  ldru - Left, Down, Right, Up"
	@echo "  dlur - Down, Left, Up, Right"
	@echo "  (Any permutation of r,d,l,u is valid)"
	@echo ""
	@echo "Multi-Unit Conflict Resolution Strategies:"
	@echo "  sequential  - Find paths one by one, avoiding previous paths"
	@echo "  priority    - Process units by priority (higher priority first)"
	@echo "  cooperative - Attempt to find mutually compatible paths"
	@echo "  wait        - Allow units to wait in place when blocked"
	@echo ""
	@echo "Project Structure:"
	@echo "  rts-tactical-pathfinder/"
	@echo "  ├── main.cpp                     # Pathfinding solver"
	@echo "  ├── map_loader_demo.cpp          # Map loader demonstration"
	@echo "  ├── Makefile"
	@echo "  ├── build/                       # Object files (.o)"
	@echo "  ├── MapLoader/"
	@echo "  │   ├── MapLoader.cpp            # Map loader"
	@echo "  │   └── MapLoader.h"
	@echo "  ├── PathFinder/"
	@echo "  │   ├── PathFinder.cpp           # Pathfinder with move orders"
	@echo "  │   └── PathFinder.h"
	@echo "  ├── PathAnimator/"
	@echo "  │   ├── PathAnimator.cpp"
	@echo "  │   └── PathAnimator.h"
	@echo "  └── MultiUnitPathFinder/"
	@echo "      ├── MultiUnitPathFinder.cpp  # Multi-unit pathfinding implementation"
	@echo "      └── MultiUnitPathFinder.h    # Multi-unit pathfinding header"

# ==============================================================================
# Individual Target Aliases
# ==============================================================================

# Build only the map loader
maploader: $(MAPLOADER_TARGET)

# Build only the pathfinder
pathfinder: $(PATHFINDER_TARGET)

# ==============================================================================
# Phony Target Declaration
# ==============================================================================

# Declare phony targets (targets that don't create files)
.PHONY: all clean install-deps run-maploader run-pathfinder test-move-orders test-multi-unit test-all-strategies test-astar test-bfs test-dfs test-all help maploader pathfinder

# ==============================================================================
# End of Makefile
# ==============================================================================