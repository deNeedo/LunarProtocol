CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lrt

SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

# Auto-discover programs from src directory
PROGRAMS = $(patsubst $(SRC_DIR)/%.c,%,$(wildcard $(SRC_DIR)/*.c))
# Auto-discover tests from tests directory
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
TEST_NAMES = $(patsubst $(TEST_DIR)/%.c,%,$(TEST_SOURCES))

# Automatically map program names to build outputs
TARGETS = $(addprefix $(BUILD_DIR)/, $(PROGRAMS))
TEST_TARGETS = $(addprefix $(BUILD_DIR)/test_, $(TEST_NAMES))

# .c → .o for all source files
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.test.o, $(TEST_SOURCES))

# Marker file to track test success
TEST_SUCCESS_MARKER = $(BUILD_DIR)/.tests_passed

# Main target: build binaries only after tests pass, then clean .o files
all: $(TARGETS)
	@echo "[CLEANUP] Removing object files..."
	@rm -f $(OBJECTS) $(TEST_OBJECTS) $(TEST_SUCCESS_MARKER)
	@echo "[SUCCESS] Build completed successfully!"

# Binaries depend on test success marker and their object files
$(BUILD_DIR)/%: $(BUILD_DIR)/%.o $(TEST_SUCCESS_MARKER)
	@set -e; \
	$(CC) $< -o $@ $(LDFLAGS) || (echo "[ERROR] Linking failed! Cleaning build directory..."; rm -rf $(BUILD_DIR); exit 1)
	@echo "[BUILD] Created binary: $@"

# Generic .o compilation rule for source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@set -e; \
	$(CC) $(CFLAGS) -c $< -o $@ || (echo "[ERROR] Compilation failed! Cleaning build directory..."; rm -rf $(BUILD_DIR); exit 1)
	@echo "[COMPILE] Built object: $@"

# Tests: build objects and binaries with distinct suffix to avoid clashes
$(BUILD_DIR)/%.test.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	@set -e; \
	$(CC) $(CFLAGS) -c $< -o $@ || (echo "[ERROR] Test compilation failed! Cleaning build directory..."; rm -rf $(BUILD_DIR); exit 1)
	@echo "[TEST] Compiled test object: $@"

$(BUILD_DIR)/test_%: $(BUILD_DIR)/%.test.o
	@set -e; \
	$(CC) $^ -o $@ $(LDFLAGS) || (echo "[ERROR] Test linking failed! Cleaning build directory..."; rm -rf $(BUILD_DIR); exit 1)
	@echo "[TEST] Built test binary: $@"

# Test success marker depends on all tests passing
$(TEST_SUCCESS_MARKER): $(TEST_TARGETS) $(OBJECTS) | $(BUILD_DIR)
	@echo "[TEST] Running all tests..."
	@set -e; \
	trap 'echo "[ERROR] Build failed! Cleaning build directory..."; rm -rf $(BUILD_DIR); exit 1' ERR; \
	for t in $(TEST_TARGETS); do \
		echo "[TEST] Running $$t"; \
		$$t || (echo "[TEST] Test $$t failed!" && rm -rf $(BUILD_DIR) && exit 1); \
	done
	@touch $@
	@echo "[TEST] All tests passed!"

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Run tests manually (rebuilds and runs)
test: $(TEST_SUCCESS_MARKER)

clean:
	rm -rf $(BUILD_DIR)
