/*
   The system-wide utility and helper functions are declared in this file
   
   Categories:
   1. Input Functions - Safe user input handling
   2. Validation Functions - Validation of Data
   3. File System Functions - Management of directories
   4. Logging Functions - Transaction logging
   5. Conversion Functions - Conversions of Data Types
 */

#ifndef UTILS_H
#define UTILS_H

#include "types.h"

 // Input Handling Functions

// Remove any remaining characters from the input buffer.
void clear_input_buffer(void);

// Securely obtain the user's string input
bool get_string_input(char *buffer, int max_len, const char *prompt);

// Securely obtain the user's decimal number (double) input
bool get_double_input(double *value, const char *prompt);

// Securely obtain the user's whole number (integer) input
bool get_int_input(int *value, const char *prompt);

 // Validation Functions

// Verify the validity of the four-digit PIN
bool is_valid_pin(const char *pin);

// Verify the validity of the money amount (positive, within limits)
bool is_valid_amount(double amount, double max);

// Verify the validity of the name (letters, spaces, basic punctuation) 
bool is_valid_name(const char *name);

// Verify the validity of the ID (alphanumeric with hyphens/underscores) 
bool is_valid_id(const char *id);

 // File System Functions

// Create the database directory if it doesn't already exist
void create_database_dir(void);

 // Logging Functions

// In the log file, record details about a transaction or system event
void log_transaction(const char *action);

 // Conversion Functions

// Convert theAccountType enum to string ("Savings" or "Current")
const char* account_type_to_string(AccountType type);

// Convert string input to the corresponding AccountType enum (SAVINGS or CURRENT)
AccountType string_to_account_type(const char *str);

#endif 
