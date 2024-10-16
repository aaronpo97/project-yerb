SRC_FILES = $(wildcard src/*.cpp)
OBJ_FILES = $(patsubst src/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

OBJ_NAME = wasm.out.js
BUILD_DIR = build
OBJ_DIR = objects
ASSETS_DIR = assets
CONFIG_DIR = config

CC = em++
COMPILER_FLAGS = -O2 -std=c++17 -Isrc -Ideps -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 --preload-file $(ASSETS_DIR) --preload-file $(CONFIG_DIR)
LINKER_FLAGS = -s ALLOW_MEMORY_GROWTH=1 -s WASM=1

# Build all targets
all: $(BUILD_DIR) $(OBJ_DIR) $(OBJ_FILES) $(BUILD_DIR)/index.html $(BUILD_DIR)/style.css $(BUILD_DIR)/script.js
	$(CC) $(OBJ_FILES) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(BUILD_DIR)/$(OBJ_NAME)

# Copy HTML, CSS, and JS files to build directory
$(BUILD_DIR)/index.html: template/index.html | $(BUILD_DIR)
	cp $< $@

$(BUILD_DIR)/style.css: template/style.css | $(BUILD_DIR)
	cp $< $@

$(BUILD_DIR)/script.js: template/script.js | $(BUILD_DIR)
	cp $< $@

# Compile C++ source files to object files in the 'objects' directory
$(OBJ_DIR)/%.o: src/%.cpp | $(OBJ_DIR)
	$(CC) $(COMPILER_FLAGS) -c $< -o $@

# Create build and objects directories if they don't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Run the server after building everything
run: serve

serve: all
	cd $(BUILD_DIR) && npx serve .

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR) $(OBJ_DIR)
