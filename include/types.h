/* 
   The fundamental data structures and constants 
   used by the financial system are defined in this file
   
   Contents:
   1. Constants - Maximum values and limits
   2. Enums - Types of accounts
   3. Structs - Account data structure
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>


//CONSTANTS - system limits and configuration

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

/* Account types enum
   Defines the two types of bank accounts:
   - SAVINGS: Savings account (typically lower fees, interest-bearing)
   - CURRENT: Current account (checking account, for daily transactions)
 */
typedef enum {
    SAVINGS, 
    CURRENT   
} AccountType;

/* 
   Account Structure
   All of a bank account's data is stored in this structure:
   - account_number: Unique identifier (7-9 digits)
   - name: Customer's full name
   - id_number: Government ID (passport, IC number, etc.)
   - type: SAVINGS or CURRENT
   - pin: 4-digit security code used for authentication
   - balance: Current amount of money in the account
 */
typedef struct {
    char account_number[20];   
    char name[MAX_NAME_LEN];      
    char id_number[MAX_ID_LEN];   
    AccountType type;            
    char pin[PIN_LEN + 1];       
    double balance;         
} Account;

#endif 
