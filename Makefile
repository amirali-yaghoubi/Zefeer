# Compiler and flags
CC          = clang
CFLAGS      = -Wall -Wextra -std=c11 -g -I include
LDFLAGS     =

# Main directories
SRC_DIR     = src
BUILD_DIR   = build
INC_DIR     = include

# Final output name
TARGET      = $(BUILD_DIR)/main

# Find all .c files in src and its subdirectories (back, front, middle, ...)
SRCS        = $(shell find $(SRC_DIR) -type f -name "*.c")

# Convert .c paths to .o paths inside build folder (folder structure is preserved)
OBJS        = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Generate .d files for header dependencies (placed next to each .o file)
DEPS        = $(OBJS:.o=.d)

# Default target (everything)
all: $(TARGET)

# Link all object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "✅ Build successful! Run ./$(TARGET)"

# Compile each .c file to .o inside build/ and create necessary subdirectories
# -MMD and -MP automatically create .d files that track #include dependencies
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Quick execution (for testing)
run: $(TARGET)
	./$(TARGET)

# Full cleanup: remove object files, dependency files, and output
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Clean and rebuild from scratch
rebuild: clean all

# Include generated dependency files (so header changes are tracked)
-include $(DEPS)

# Phony targets (not actual files)
.PHONY: all clean rebuild run
