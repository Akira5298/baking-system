# Makefile for Banking System Application
# COMP1312 Programming I Coursework

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
TARGET = banking_system

# Source files
SRC = main.c menu.c account.c transaction.c utils.c

# Object files
OBJ = $(SRC:.c=.o)

# Header files
HEADERS = types.h menu.h account.h transaction.h utils.h

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Compile source files to object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJ)
	rm -rf database

# Run the program
run: $(TARGET)
	./$(TARGET)

# Create a fresh database
reset:
	rm -rf database
	mkdir -p database

.PHONY: all clean run reset
