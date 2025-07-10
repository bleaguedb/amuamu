CXX := g++
CXXFLAGS := -std=c++20 -Wall

TARGET_DIR := bin
TARGET := $(TARGET_DIR)/main

SRC_DIR := src
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

OBJ_DIR := objs
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET_DIR) $(OBJ_DIR)