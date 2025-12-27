/*
 * utils.c - Utility functions implementation
 */

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

/*
 * Clear input buffer
 */
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/*
 * Get string input from user with validation
 */
bool get_string_input(char *buffer, int max_len, const char *prompt) {
    printf("%s", prompt);
    if (fgets(buffer, max_len, stdin) == NULL) {
        return false;
    }
    
    // Remove newline
    buffer[strcspn(buffer, "\n")] = 0;
    
    // Check if empty
    if (strlen(buffer) == 0) {
        printf("Error: Input cannot be empty.\n");
        return false;
    }
    
    return true;
}

/*
 * Get double input from user with validation
 */
bool get_double_input(double *value, const char *prompt) {
    char input[100];
    printf("%s", prompt);
    
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return false;
    }
    
    char *endptr;
    errno = 0;
    *value = strtod(input, &endptr);
    
    // Check for conversion errors
    if (endptr == input || (*endptr != '\0' && *endptr != '\n')) {
        printf("Error: Invalid input. Please enter a number.\n");
        return false;
    }
    
    if (errno == ERANGE) {
        printf("Error: Number out of range.\n");
        return false;
    }
    
    return true;
}

/*
 * Get integer input from user with validation
 */
bool get_int_input(int *value, const char *prompt) {
    double temp;
    if (!get_double_input(&temp, prompt)) {
        return false;
    }
    
    if (temp != (int)temp) {
        printf("Error: Please enter a whole number.\n");
        return false;
    }
    
    *value = (int)temp;
    return true;
}

/*
 * Validate PIN format
 */
bool is_valid_pin(const char *pin) {
    if (strlen(pin) != PIN_LEN) {
        return false;
    }
    
    for (int i = 0; i < PIN_LEN; i++) {
        if (!isdigit(pin[i])) {
            return false;
        }
    }
    
    return true;
}

/*
 * Validate amount
 */
bool is_valid_amount(double amount, double max) {
    if (amount <= 0) {
        printf("Error: Amount must be greater than RM0.\n");
        return false;
    }
    
    if (amount > max) {
        printf("Error: Amount cannot exceed RM%.2f per operation.\n", max);
        return false;
    }
    
    return true;
}

/*
 * Create database directory if it doesn't exist
 */
void create_database_dir(void) {
    #ifdef _WIN32
        system("if not exist database mkdir database");
    #else
        system("mkdir -p database");
    #endif
}

/*
 * Log transaction to transaction.log file
 */
void log_transaction(const char *action) {
    FILE *fp = fopen(TRANSACTION_LOG, "a");
    if (fp == NULL) {
        fprintf(stderr, "Warning: Could not open transaction log\n");
        return;
    }
    
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec, action);
    
    fclose(fp);
}

/*
 * Convert account type to string
 */
const char* account_type_to_string(AccountType type) {
    return (type == SAVINGS) ? "Savings" : "Current";
}

/*
 * Convert string to account type
 */
AccountType string_to_account_type(const char *str) {
    char lower[50];
    int i;
    for (i = 0; str[i] && i < 49; i++) {
        lower[i] = tolower(str[i]);
    }
    lower[i] = '\0';
    
    if (strstr(lower, "savings") != NULL) {
        return SAVINGS;
    }
    return CURRENT;
}
