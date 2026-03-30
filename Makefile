CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

SRC_DIR = src
OBJ_DIR = build
TARGET = app
TEST_TARGET = test

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRCS = $(wildcard tests/*.cpp)

# Build artifacts
APP_SRCS = $(filter-out $(SRC_DIR)/main.cpp,$(SRCS))
APP_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(APP_SRCS))
MAIN_OBJ = $(OBJ_DIR)/main.o

# Convert src/file.cpp -> build/file.o
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TEST_OBJS = $(patsubst tests/%.cpp,$(OBJ_DIR)/%.o,$(TEST_SRCS))

# Default rule
all: $(TARGET)

# Link object files for the main app
$(TARGET): $(APP_OBJS) $(MAIN_OBJ)
	$(CXX) $(APP_OBJS) $(MAIN_OBJ) -o $(TARGET)

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test source files into object files
$(OBJ_DIR)/%.o: tests/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test target
$(TEST_TARGET): $(APP_OBJS) $(TEST_OBJS)
	$(CXX) $(APP_OBJS) $(TEST_OBJS) -o $(TEST_TARGET)

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(TEST_TARGET)

.PHONY: all clean