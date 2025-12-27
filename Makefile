CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -Iinclude
TARGET = banking_system
SRC_DIR = srs
BUILD_DIR = build

SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/menu.c $(SRC_DIR)/account.c $(SRC_DIR)/transaction.c $(SRC_DIR)/utils.c

OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/menu.o $(BUILD_DIR)/account.o $(BUILD_DIR)/transaction.o $(BUILD_DIR)/utils.o

HEADERS = include/types.h include/menu.h include/account.h include/transaction.h include/utils.h

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

reset:
	rm -rf database
	mkdir -p database

.PHONY: all clean run reset
