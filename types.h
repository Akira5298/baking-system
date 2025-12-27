/*
 * types.h - Data types and constants for Banking System
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

// Constants
#define MAX_NAME_LEN 100
#define MAX_ID_LEN 20
#define PIN_LEN 4
#define MAX_ACCOUNTS 1000
#define DATABASE_DIR "database"
#define INDEX_FILE "database/accounts_index.txt"
#define TRANSACTION_LOG "database/transaction.log"
#define MIN_ACCOUNT_NUM 1000000
#define MAX_ACCOUNT_NUM 999999999
#define MAX_DEPOSIT 50000.0
#define MIN_AMOUNT 0.01

// Account types
typedef enum {
    SAVINGS,
    CURRENT
} AccountType;

// Account structure
typedef struct {
    char account_number[20];
    char name[MAX_NAME_LEN];
    char id_number[MAX_ID_LEN];
    AccountType type;
    char pin[PIN_LEN + 1];
    double balance;
} Account;

#endif // TYPES_H
