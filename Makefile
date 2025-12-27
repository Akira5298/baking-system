# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -Iinclude

# Output executable name
TARGET = banking_system

# Directories
SRC_DIR = srs
BUILD_DIR = build

# All source files (.c files)
SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/menu.c $(SRC_DIR)/account.c $(SRC_DIR)/transaction.c $(SRC_DIR)/utils.c

# Object files (.o files)
OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/menu.o $(BUILD_DIR)/account.o $(BUILD_DIR)/transaction.o $(BUILD_DIR)/utils.o

# Header files (.h files)
HEADERS = include/types.h include/menu.h include/account.h include/transaction.h include/utils.h

# Default target: compile everything
all: $(BUILD_DIR) $(TARGET)

# Create build directory if it doesn't exist yet
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link all object files into the final executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Compile each .c file into a .o file
# $< means the first dependency (the .c file)
# $@ means the target (the .o file)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Remove compiled files and build directory
clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -rf $(BUILD_DIR)

# Compile and run the program
run: $(TARGET)
	./$(TARGET)

# Delete all accounts and start fresh
reset:
	rm -rf database
	mkdir -p database

# These targets don't create files, they just run commands
.PHONY: all clean run reset
