SRC_FILES = $(wildcard src/*.cpp)
OBJ_FILES = $(patsubst src/%.cpp, objects/%.o, $(SRC_FILES))  # Change the object file directory


OBJ_NAME = index.js
BUILD_DIR = build
ASSETS_DIR = assets

CC = em++
COMPILER_FLAGS = -O2 -std=c++17 -Isrc -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 --preload-file $(ASSETS_DIR)
LINKER_FLAGS = -s ALLOW_MEMORY_GROWTH=1 -s WASM=1

# Ensure build and objects directories are created before building anything
all: $(BUILD_DIR) objects $(OBJ_FILES) $(BUILD_DIR)/index.html 
	$(CC) $(OBJ_FILES) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(BUILD_DIR)/$(OBJ_NAME)

# Rule to copy template.html to build directory and rename it to index.html
$(BUILD_DIR)/index.html: template.html | $(BUILD_DIR)
	cp $< $@

# Update the object file rule to point to the 'objects' directory
objects/%.o: src/%.cpp | objects
	$(CC) $(COMPILER_FLAGS) -c $< -o $@

# Create the 'build' directory and 'objects' directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

objects:
	mkdir -p objects

# Run the server after building everything
run: serve

serve: all
	cd $(BUILD_DIR) && npx serve .

# Clean up build artifacts
clean:
	rm -f $(BUILD_DIR)/$(OBJ_NAME)
	rm -rf $(BUILD_DIR)
	rm -rf objects
