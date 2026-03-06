CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

SRC_DIR = src
OBJ_DIR = build
TARGET = app

SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Convert src/file.cpp -> build/file.o
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Default rule
all: $(TARGET)

# Link object files
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean