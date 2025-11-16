CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lrt

SRC_DIR = src
BUILD_DIR = build

PROGRAMS = hub rover

# Automatically map program names to build outputs
TARGETS = $(addprefix $(BUILD_DIR)/, $(PROGRAMS))

# .c → .o for all source files
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))

all: $(TARGETS)

# Generic build rule: build/<program> from build/<program>.o
$(BUILD_DIR)/%: $(BUILD_DIR)/%.o
	$(CC) $^ -o $@ $(LDFLAGS)

# Generic .o compilation rule
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
