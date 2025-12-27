/* This file contains helper functions that are used throughout the banking system.
   
   Main categories:
   1. Input Functions - Get data from users safely
   2. Validation Functions - Check if data is valid
   3. File System Functions - Create directories
   4. Logging Functions - Record system activities
   5. Conversion Functions - Convert between data formats
 */

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>


/* Clear input buffer function
   Purpose: Remove any remaining characters from the input stream
   
   Why we need this:
   When you write "123abc" and scanf reads "123", "abc" stays in the buffer
    To avoid input errors, this function removes any remaining text
 */
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);  /* Read until newline or end */
}

/* Get string input function
   Purpose: Safely get text input from the user
   
   Parameters:
   - buffer: where the text is kept
   - max_len: maximum length allowed
   - prompt: a message to display to the user
   
   Returns: true if the input is correct, false if it is incorrect or empty
   
   Safety checks:
   1. Input cannot be empty
   2. Input cannot be too long
   3. Removes the newline character at the end
 */
bool get_string_input(char *buffer, int max_len, const char *prompt) {
    printf("%s", prompt);
    
    // Read the input from user
    if (fgets(buffer, max_len, stdin) == NULL) {
        return false;  
    }
    
    // Remove the newline character at the end, replacing with null terminator
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';  
    } else {
        clear_input_buffer();  
    }
    
    // Check if the input is empty
    if (strlen(buffer) == 0) {
        printf("Error: Input cannot be empty.\n");
        return false;
    }
    
    // Check if the input is too long 
    if (strlen(buffer) >= (size_t)(max_len - 1)) {
        printf("Error: Input is too long.\n");
        return false;
    }
    
    return true;
}

/* 
   Get double input function
   Purpose: Safely obtain from the user a decimal number (such as money amounts)
   
  Parameters:
   - value: A pointer to the storage location of the number
   - prompt: A message to display to the user
   
   Returns: true if the number is valid, false if it isnt
   
   Why we need this:
   When we expect "12.50," users may write "abc."
   Invalid numbers are identified and rejected by this function
 */
bool get_double_input(double *value, const char *prompt) {
    char input[100];
    printf("%s", prompt);
    
    // Read the input as a text first
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return false;
    }
    
    // Then try to convert text to number (double)
    char *endptr;
    errno = 0;  
    *value = strtod(input, &endptr);  
    
    //Check if the conversion worked 
    if (endptr == input || (*endptr != '\0' && *endptr != '\n')) {
        printf("Error: Invalid input. Please enter a number.\n");
        return false;
    }
    
    // Check if the number was too large 
    if (errno == ERANGE) {
        printf("Error: Number out of range.\n");
        return false;
    }
    
    return true;
}

/* Get integer input function
   Purpose: Securely obtain an integer (whole number) from the user
 
   Parameters:
   - value: A pointer to where the integer will be stored
   - prompt: A message to show to the user
   
   Returns: true if the intege is valid, false if it isnt
   
   How it works:
   1. Uses get_double_input to read a number
   2. Verifies if it's a whole number (no decimals)
   3. Converts to an integer if its valid
 */
bool get_int_input(int *value, const char *prompt) {
    double temp;
    if (!get_double_input(&temp, prompt)) {
        return false;
    }
    
    // Check if it's a whole number (exp. 12.0 is ok, but 12.5 is not)
    if (temp != (int)temp) {
        printf("Error: Please enter a whole number.\n");
        return false;
    }
    
    *value = (int)temp;
    return true;
}

/* Validate pin function
  Purpose: To find out if its a valid PIN format
  
  These are the rules for a valid PIN:
  1. It must be exactly four characters long
  2. All characters must be digits (0-9)
  
  Examples:
    "1234" → Valid
    "12" → Invalid (too short)
    "12345" → Invalid (too long)
    "12a4" → Invalid (contains letter)
 */
bool is_valid_pin(const char *pin) {
    // Verify the length 
    if (strlen(pin) != PIN_LEN) {
        return false;
    }
    
    // Check if each character is a digit
    for (int i = 0; i < PIN_LEN; i++) {
        if (!isdigit(pin[i])) {
            return false;
        }
    }
    
    return true;
}

/* Validate amount function
   Purpose: Check if a money amount is valid
   
   Parameters:
   - amount: The sum of money to verify
   - max: Maximum amount allowed for this operation
   
   Validation rules:
   1. Must be greater than RM0
   2. This operation's maximum cannot be exceeded
   3. It cannot be excessively large (up to 999 million)
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
    
    if (amount > 999999999.99) {
        printf("Error: Amount is too large.\n");
        return false;
    }
    
    // Check decimal places (money can only have 2 decimal places)
    double cents = amount * 100;
    if (cents != (long long)cents) {
        printf("Error: Amount cannot have more than 2 decimal places.\n");
        return false;
    }
    
    return true;
}

/* Validate name function
   Purpose: Check if a person's name is valid
   
   Validation rules:
   1. Must be at least 2 characters long
   2. Cannot exceed 50 characters
   3. It can only contain letters, spaces, and certain punctuation (. ' -)
   
   Examples:
     "John Doe" → Valid
     "Mary-Jane O'Connor" → Valid
     "A" → Invalid (too short)
     "John123" → Invalid (contains numbers)
 */
bool is_valid_name(const char *name) {
    if (strlen(name) < 2) {
        printf("Error: Name must be at least 2 characters long.\n");
        return false;
    }
    
    // Check each character
    for (size_t i = 0; i < strlen(name); i++) {
        if (!isalpha(name[i]) && name[i] != ' ' && name[i] != '.' && name[i] != '-') {
            printf("Error: Name can only contain letters, spaces, hyphens, and periods.\n");
            return false;
        }
    }
    
    return true;
}

/* Validate a function
   Purpose: To check if an ID number is valid
   
   Validation rules:
   1. It must be at least 3 characters long
   2. Can only contain letters, numbers, hyphens, and underscores
   
   Examples:
     "IC123456" → Valid
     "PASS-123" → Valid
     "A1" → Invalid (too short)
     "IC#123" → Invalid (cause it contains #)
 */
bool is_valid_id(const char *id) {
    if (strlen(id) < 3) {
        printf("Error: ID must be at least 3 characters long.\n");
        return false;
    }
    
    // Verify each character
    for (size_t i = 0; i < strlen(id); i++) {
        if (!isalnum(id[i]) && id[i] != '-' && id[i] != '_') {
            printf("Error: ID can only contain letters, numbers, hyphens, and underscores.\n");
            return false;
        }
    }
    
    return true;
}


/* Create database directory function
   Purpose: Create the database folder if it doesn't exist already
   
   How it works:
     The "mkdir" command is used on Windows
     The "mkdir -p" command is used on Unix/Mac (p flag creates parent directories)
   
   All account files are kept in this folder
 */
void create_database_dir(void) {
    #ifdef _WIN32
        system("if not exist database mkdir database");  
    #else
        system("mkdir -p database"); 
    #endif
}


/* 
   Log transaction function
   Purpose: Record a transaction event in the log file
   
   Parameters:
   - action: Description of what happened (exp. "Deposit: Account 12345")
   
   How it works:
   1. Use append mode to open the log file (adds to end)
   2. Obtain the current time and date
   3. Write timestamp and add the action to the log file
   4. Close the file

   An audit trail of all system operations is produced as a result
 */
void log_transaction(const char *action) {
    FILE *fp = fopen(TRANSACTION_LOG, "a");  
    if (fp == NULL) {
        fprintf(stderr, "Warning: Could not open transaction log\n");
        return;
    }
    
    // Get the current date and time 
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    // Write timestamp and action it to the log file 
    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec, action);
    
    fclose(fp);
}

/* Account type to string function
   Purpose: Convert AccountType enum to a text
   
   Parameters:
     type: The value of the AccountType enum (SAVINGS or CURRENT)
   
   Returns: A string that is readable by humans
   
   Example:
     SAVINGS → "Savings"
     CURRENT → "Current"
 */
const char* account_type_to_string(AccountType type) {
    return (type == SAVINGS) ? "Savings" : "Current";
}

/* String to account type function
   Purpose: Convert text input into an AccountType enum
   
   Parameters:
     str: The string that needs to be converted 
   (such as "savings", "CURRENT", "S", or "c")
 
   SAVINGS or CURRENT enum value is returned
   
   How it works:
   1. To make comparison easier, convert input to lowercase
   2. Verify whether "savings," "s," "sav", "save", or "saving" match
   3. If not, return CURRENT
   
   Examples:
     "savings" → SAVINGS
     "SAVINGS" → SAVINGS
     "S" → SAVINGS
     "current" → CURRENT
     "C" → CURRENT
 */
AccountType string_to_account_type(const char *str) {
    char lower[50];
    int i;
    // Convert to lowercase
    for (i = 0; str[i] && i < 49; i++) {
        lower[i] = tolower(str[i]);
    }
    lower[i] = '\0';
    
    /* Check if string contains "savings" (or variations like "s", "sav") 
     otherwise default to CURRENT */
    if (strstr(lower, "savings") != NULL) {
        return SAVINGS;
    }
    return CURRENT;  
}
