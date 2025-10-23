# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++20 -pthread -Wall -Wextra -O2 -Isrc -Wno-unused-parameter -Wno-reorder
LDFLAGS := -lws2_32 -lmswsock
CLIENT_LIBS := -lsdl2gui -lsdl2 -lsdl2_ttf -lsdl2_image

# Directories
SRC_DIR := src
CORE_DIR := $(SRC_DIR)/CoreImpl
CLIENT_DIR := $(SRC_DIR)/Client
SERVER_DIR := $(SRC_DIR)/Server
TESTS_DIR := tests
BUILD_DIR := build
CLIENT_BUILD_DIR := $(BUILD_DIR)/client
SERVER_BUILD_DIR := $(BUILD_DIR)/server
TESTS_BUILD_DIR := $(BUILD_DIR)/tests

# Output binaries
CLIENT_BIN := $(CLIENT_BUILD_DIR)/client
SERVER_BIN := $(SERVER_BUILD_DIR)/server

# Source files
CORE_SRCS := $(wildcard $(CORE_DIR)/*.cpp)
CLIENT_SRCS := $(wildcard $(CLIENT_DIR)/*.cpp)
SERVER_SRCS := $(wildcard $(SERVER_DIR)/*.cpp)
TEST_SRCS := $(wildcard $(TESTS_DIR)/*.cpp)

# Object files
CORE_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CORE_SRCS))
CLIENT_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(CLIENT_BUILD_DIR)/%.o, $(CLIENT_SRCS))
SERVER_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(SERVER_BUILD_DIR)/%.o, $(SERVER_SRCS))
TEST_OBJS := $(patsubst $(TESTS_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(TEST_SRCS))

# Test executables
TEST_BINS := $(patsubst $(TESTS_DIR)/%.cpp, $(TESTS_BUILD_DIR)/%, $(TEST_SRCS))

# Targets
all: build_client build_server build_tests

build_client: $(CLIENT_BIN)

build_server: $(SERVER_BIN)

build_tests: $(TEST_BINS)

$(CLIENT_BIN): $(CORE_OBJS) $(CLIENT_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(CLIENT_LIBS)

$(SERVER_BIN): $(CORE_OBJS) $(SERVER_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(TESTS_BUILD_DIR)/%: $(BUILD_DIR)/%.o $(CORE_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(CLIENT_BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SERVER_BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TESTS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all build_client build_server build_tests clean