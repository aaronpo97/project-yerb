# OBJS specifies all .cpp files in the src directory
OBJS = $(wildcard src/*.cpp)

# CC specifies which compiler we're using
CC = g++

# Compiler flags: -O3 for optimization, -std=c++17 for the C++ standard
# Adding -Isrc to specify the directory for header files
COMPILER_FLAGS = -O3 -std=c++17 -Isrc

# LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2

# Build directory
BUILD_DIR = build

# OBJ_NAME specifies the name of our executable
OBJ_NAME = app.out

# This is the target that compiles our executable
all: $(BUILD_DIR) $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(BUILD_DIR)/$(OBJ_NAME)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Run the executable after building
run: all
	./$(BUILD_DIR)/$(OBJ_NAME)

# Clean command to remove the executable and the build directory
clean:
	rm -f $(BUILD_DIR)/$(OBJ_NAME)
	rm -rf $(BUILD_DIR)
