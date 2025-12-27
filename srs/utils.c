
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

bool get_string_input(char *buffer, int max_len, const char *prompt) {
    printf("%s", prompt);
    if (fgets(buffer, max_len, stdin) == NULL) {
        return false;
    }
    
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        clear_input_buffer();
    }
    
    if (strlen(buffer) == 0) {
        printf("Error: Input cannot be empty.\n");
        return false;
    }
    
    if (strlen(buffer) >= (size_t)(max_len - 1)) {
        printf("Error: Input is too long.\n");
        return false;
    }
    
    return true;
}

bool get_double_input(double *value, const char *prompt) {
    char input[100];
    printf("%s", prompt);
    
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return false;
    }
    
    char *endptr;
    errno = 0;
    *value = strtod(input, &endptr);
    
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

bool is_valid_amount(double amount, double max) {
    if (amount <= 0) {
        printf("Error: Amount must be greater than RM0.\n");
        return false;
    }
    
    if (amount > max) {
        printf("Error: Amount cannot exceed RM%.2f per operation.\n", max);
        return false;
    }
    
    if (amount > 999999999.99) {
        printf("Error: Amount is too large.\n");
        return false;
    }
    
    double cents = amount * 100;
    if (cents != (long long)cents) {
        printf("Error: Amount cannot have more than 2 decimal places.\n");
        return false;
    }
    
    return true;
}

bool is_valid_name(const char *name) {
    if (strlen(name) < 2) {
        printf("Error: Name must be at least 2 characters long.\n");
        return false;
    }
    
    for (size_t i = 0; i < strlen(name); i++) {
        if (!isalpha(name[i]) && name[i] != ' ' && name[i] != '.' && name[i] != '-') {
            printf("Error: Name can only contain letters, spaces, hyphens, and periods.\n");
            return false;
        }
    }
    
    return true;
}

bool is_valid_id(const char *id) {
    if (strlen(id) < 3) {
        printf("Error: ID must be at least 3 characters long.\n");
        return false;
    }
    
    for (size_t i = 0; i < strlen(id); i++) {
        if (!isalnum(id[i]) && id[i] != '-' && id[i] != '_') {
            printf("Error: ID can only contain letters, numbers, hyphens, and underscores.\n");
            return false;
        }
    }
    
    return true;
}

void create_database_dir(void) {
    #ifdef _WIN32
        system("if not exist database mkdir database");
    #else
        system("mkdir -p database");
    #endif
}

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

const char* account_type_to_string(AccountType type) {
    return (type == SAVINGS) ? "Savings" : "Current";
}

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
