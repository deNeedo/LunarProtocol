CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))
# Exclude main.o for tests
LIB_OBJ = $(filter-out build/main.o, $(OBJ))

TEST_SRC = $(wildcard tests/*.c)
TEST_BIN = $(patsubst tests/%.c, build/%, $(TEST_SRC))

TARGET = build/comms_v1.0

# Default rule: run tests, then build
all: test $(TARGET)

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)
	@echo "✅ Build successful: $(TARGET)"

# Build and run each test
test: $(TEST_BIN)
	@echo "Running tests..."
	@for t in $(TEST_BIN); do \
		echo "→ $$t"; \
		./$$t || { echo "❌ Test failed: $$t"; exit 1; }; \
	done
	@echo "✅ All tests passed."

# Compile test binaries (without main.o)
build/%: tests/%.c $(LIB_OBJ)
	mkdir -p build
	$(CC) $(CFLAGS) $< $(LIB_OBJ) -o $@

run: all
	./$(TARGET)

clean:
	rm -rf build $(TARGET)
