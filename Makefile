# Makefile for Banking System Application
# COMP1312 Programming I Coursework

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -Iinclude
TARGET = banking_system
SRC_DIR = srs
BUILD_DIR = build

# Source files
SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/menu.c $(SRC_DIR)/account.c $(SRC_DIR)/transaction.c $(SRC_DIR)/utils.c

# Object files
OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/menu.o $(BUILD_DIR)/account.o $(BUILD_DIR)/transaction.o $(BUILD_DIR)/utils.o

# Header files
HEADERS = include/types.h include/menu.h include/account.h include/transaction.h include/utils.h

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -rf $(BUILD_DIR)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Create a fresh database
reset:
	rm -rf database
	mkdir -p database

.PHONY: all clean run reset
