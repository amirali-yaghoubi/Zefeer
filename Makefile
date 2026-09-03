C       = gcc
CFLAGS   = -Wall -Wextra -Iinclude -g
LDFLAGS  =
DEPFLAGS = -MMD -MP

SRC_DIR   = src
OBJ_DIR   = build/obj
BIN_DIR   = build
TEST_DIR  = test

# Collect all .c files from src/ except main.c
SOURCES  = $(shell find $(SRC_DIR) -name "*.c" ! -name "main.c")
OBJS     = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# Main program
MAIN_OBJ = $(OBJ_DIR)/main.o
TARGET   = $(BIN_DIR)/zfc

# Test binaries (all *_test.c in test/)
TESTS    = $(wildcard $(TEST_DIR)/*_test.c)
TEST_TARGETS = $(patsubst $(TEST_DIR)/%.c, $(BIN_DIR)/%, $(TESTS))

# ------------------------------------------------------------
# Explicit targets
# ------------------------------------------------------------
.PHONY: all tests main lexer_test parser_test sa_test ir_test arm64_test re clean

all: main tests

tests: $(TEST_TARGETS)

main: $(TARGET)

lexer_test: $(BIN_DIR)/lexer_test
parser_test: $(BIN_DIR)/parser_test
sa_test: $(BIN_DIR)/semantic_analyzer_test
ir_test: $(BIN_DIR)/ir_test
arm64_test: $(BIN_DIR)/arm64_test

re: clean all

# ------------------------------------------------------------
# Build rules
# ------------------------------------------------------------
# Include generated dependency files
DEPS = $(OBJS:.o=.d) $(MAIN_OBJ:.o=.d)
-include $(DEPS)

# Link main executable
$(TARGET): $(MAIN_OBJ) $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Compile any .c file from src/ (including main.c)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Build any test binary (links with all .o files except main.o)
$(BIN_DIR)/%: $(TEST_DIR)/%.c $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@

clean:
	rm -rf build
