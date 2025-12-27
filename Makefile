# Makefile for Banking System Application
# COMP1312 Programming I Coursework

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
TARGET = banking_system
SRC = main.c

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean build artifacts
clean:
	rm -f $(TARGET)
	rm -rf database

# Run the program
run: $(TARGET)
	./$(TARGET)

# Create a fresh database
reset:
	rm -rf database
	mkdir -p database

.PHONY: all clean run reset
