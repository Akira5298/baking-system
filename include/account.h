/* This file declares functions for account management operations
   Bank account creation, loading, saving, and management are all handled 
   by these functions
 */

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "types.h"


// Database Functions
// Load an account from the database file
bool load_account(const char *account_num, Account *acc);

// Save an account to the database file
bool save_account(const Account *acc);

// Check if an account exists in the system
bool account_exists(const char *account_num);

// Verify the validity of an account number and PIN combination
bool authenticate(const char *account_num, const char *pin);

// Account Management Functions
// Generate a new unique account number
char* generate_account_number(void);

// Count total number of accounts in system
int count_accounts(void);

// Create a new bank account 
void create_account(void);

// Delete an existing bank account 
void delete_account(void);

#endif
