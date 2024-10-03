OBJS = $(wildcard src/*.cpp)
CC = em++


COMPILER_FLAGS = -O2 -std=c++17 -Isrc -s USE_SDL=2 -s USE_SDL_IMAGE=2
LINKER_FLAGS = -s ALLOW_MEMORY_GROWTH=1 -s WASM=1

# PRELOAD_FILES = --preload-file assets

# Output HTML file for the game
OBJ_NAME = index.html

# Build directory
BUILD_DIR = build

# This is the target that compiles our executable for WASM
all: $(BUILD_DIR) $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(PRELOAD_FILES) -o $(BUILD_DIR)/$(OBJ_NAME)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Serve command to start a local server for testing

run: serve

serve: all
	cd $(BUILD_DIR) && npx serve .

# Clean command to remove the build directory
clean:
	rm -f $(BUILD_DIR)/$(OBJ_NAME)
	rm -rf $(BUILD_DIR)
