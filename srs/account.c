/* This file manages all account-related functions, such as creating, deleting,
   account loading and saving, everything related to the customer data
 */

#include "account.h"
#include "utils.h"
#include <stdio.h>   
#include <stdlib.h>   
#include <string.h>  
#include <time.h>      

/* By reading the index file, which contains the list of all account numbers, 
   it determines the total number of bank accounts in the system
  
  Returns:
    The number of accounts (0 if there are no accounts or no file exists)
 */
int count_accounts(void) {
    // Attempt to access the index file 
    FILE *fp = fopen(INDEX_FILE, "r");
    if (fp == NULL) {
        return 0;  
    }
    
    // Count the file's lines; each line represents a single account
    int count = 0;
    char line[100];
    while (fgets(line, sizeof(line), fp) != NULL) {
        count++;  
    }
    
    fclose(fp);
    return count;
}

/*
  Verifies whether an account number is already exists
  By doing this, you can avoid duplicate account numbers
  
  Parameters:
    account_num - The account number to be verified
  
  Returns:
    true if the account exists, false else
 */
bool account_exists(const char *account_num) {
    // Open the index file 
    FILE *fp = fopen(INDEX_FILE, "r");
    if (fp == NULL) {
        return false;  
    }
    
    /* Read each line (each account number) to compare and find a duplicate 
       true if you find it*/
    char line[100];
    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = 0;  
        if (strcmp(line, account_num) == 0) { 
            fclose(fp);
            return true;
        }
    }
    
    fclose(fp);
    return false; 
}

/*
  Generates a seven-to nine-digit random account number
  Keeps trying until it locates an unused number
  
  How it works:
    1. Create a seven, eight, or nine-digit random number
    2. Verify whether it is currently in use
    3. Try again if in use
    4. Return it if it's unique
  
  Returns:
    A pointer to the string of the created account number
    NULL if it failed after 100 attempts (unlikely)
 */
char* generate_account_number(void) {
    static char account_num[20];  
    srand(time(NULL));  
    
    int attempts = 0;
    do {
        // Generate a random number with seven, eight, or nine digits 
        int digits = 7 + (rand() % 3);  
        
        // Build the number one digit at a time 
        long long num = 0;
        for (int i = 0; i < digits; i++) {
            num = num * 10 + (rand() % 10);  
        }
        
        // make sure it has a minimum of seven digits 
        if (num < 1000000) {
            num += 1000000;
        }
        
        // Convert the number to string 
        sprintf(account_num, "%lld", num);
        
        // This is necessary so that it doesn't loop forever
        attempts++;
        if (attempts > 100) {
            return NULL;
        }
    } while (account_exists(account_num)); 
    
    return account_num;  
}

/*
  Loads data from an account's file into an Account structure
  (Like taking a client's info out of the filing cabinet)
  
  Parameters:
    account_num - The account number that has to be loaded
    acc - A pointer to the account structure to fill with data
  
  Returns:
    true if the account loaded successfuly, false else
 */
bool load_account(const char *account_num, Account *acc) {
    // Build the filename: database/12345678.txt 
    char filename[100];
    sprintf(filename, "%s/%s.txt", DATABASE_DIR, account_num);
    
    // Try to open the account file 
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return false; 
    }
    
    // Read every field inside the file 
    char type_str[20];
    if (fscanf(fp, "Account Number: %s\n", acc->account_number) != 1 ||
        fscanf(fp, "Name: %[^\n]\n", acc->name) != 1 ||
        fscanf(fp, "ID Number: %s\n", acc->id_number) != 1 ||
        fscanf(fp, "Account Type: %s\n", type_str) != 1 ||
        fscanf(fp, "PIN: %s\n", acc->pin) != 1 ||
        fscanf(fp, "Balance: %lf\n", &acc->balance) != 1) {
        fclose(fp);
        return false;  
    }
    
    // Convert the account type string to an enum value
    acc->type = string_to_account_type(type_str);
    fclose(fp);
    return true;  
}

/*
  Saves an account's information to a file
  Creates or overwrites a file containing account information
  (to keeping track of a customer's updated data)
  
  Parameters:
    acc - A pointer to the account structure containing data to save
  
  Returns:
    true if the saving was successful, false else
 */
bool save_account(const Account *acc) {
    // Build the filename: database/12345678.txt 
    char filename[100];
    sprintf(filename, "%s/%s.txt", DATABASE_DIR, acc->account_number);
    
    // Open the file for writing (creates a new one or overwrites an existing one)
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        return false;  
    }
    
    // Write each field to the file in a specific format
    fprintf(fp, "Account Number: %s\n", acc->account_number);
    fprintf(fp, "Name: %s\n", acc->name);
    fprintf(fp, "ID Number: %s\n", acc->id_number);
    fprintf(fp, "Account Type: %s\n", account_type_to_string(acc->type));
    fprintf(fp, "PIN: %s\n", acc->pin);
    fprintf(fp, "Balance: %.2f\n", acc->balance);  
    
    fclose(fp);
    return true; 
}

/*
  Verifies whether the given PIN and the account's PIN match
  (Like verifying a password)
  
  Parameters:
    account_num - Account number for authentication
    pin - PIN to verify
  
  Returns:
    True if the PIN is accurate, false else
 */
bool authenticate(const char *account_num, const char *pin) {
    Account acc;
    
    // Load the account from file
    if (!load_account(account_num, &acc)) {
        return false;  
    }
    
    // Compare the provided PIN with the stored one
    return strcmp(acc.pin, pin) == 0;  
}

/*
  This feature guide the user open a new bank account
  It gathers all required data and verifies every input
  
  Steps:
    1. Obtain the customer's full name
    2. Obtain the customer's ID number
    3. Select the account type (Savings or Current)
    4. Set a four-digit PIN
    5. Generate unique account number
    6. Save to the database
 */
void create_account(void) {
    Account acc;      
    char input[100];  
    
    printf("\n========================================\n");
    printf("       CREATE NEW BANK ACCOUNT\n");
    printf("========================================\n");
    
    /*
       STEP 1: 
       Ask for the customer's full name and verify it
     */
    if (!get_string_input(acc.name, MAX_NAME_LEN, "Enter full name: ")) {
        printf("Error: Failed to read name.\n");
        return;
    }
    
    // Validate name (checks for special characters, length, etc.)
    if (!is_valid_name(acc.name)) {
        return;  
    }
    
    /* STEP 2:
       Ask for the customer's identification number and verify it
     */
    if (!get_string_input(acc.id_number, MAX_ID_LEN, "Enter ID number: ")) {
        printf("Error: Failed to read ID number.\n");
        return;
    }
    
    // Validate ID number 
    if (!is_valid_id(acc.id_number)) {
        return;  
    }
    
    /* STEP 3: 
       Allow the user to select between a Current and Savings account
     */
    printf("Select account type:\n");
    printf("1. Savings\n");
    printf("2. Current\n");
    printf("Enter choice: ");
    
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error: Failed to read account type.\n");
        return;
    }
    
    // Convert the input to a number and set the account type
    int type_choice = atoi(input);
    if (type_choice == 1) {
        acc.type = SAVINGS;  
    } else if (type_choice == 2) {
        acc.type = CURRENT; 
    } else {
        printf("Error: Invalid account type.\n");
        return;
    }
    
    /* STEP 4:
       Request the user to generate a four-digit PIN for security
       Continue requesting until they provide a working PIN
     */
    do {
        if (!get_string_input(acc.pin, PIN_LEN + 1, "Enter 4-digit PIN: ")) {
            printf("Error: Failed to read PIN.\n");
            return;
        }
        
        // Check if PIN is valid (should be exactly four digits) 
        if (!is_valid_pin(acc.pin)) {
            printf("Error: PIN must be exactly 4 digits.\n");
        }
    } while (!is_valid_pin(acc.pin));  
    
    /* STEP 5: 
       Create a unique random account number
     */
    char *account_num = generate_account_number();
    if (account_num == NULL) {
        printf("Error: Failed to generate unique account number.\n");
        return;
    }
    strcpy(acc.account_number, account_num);
    
    // Set starting balance to zero (new accounts start empty) 
    acc.balance = 0.0;
    
    /* STEP 6: 
       Save the account file and add to index
     */
    
    // Save the account data to a file 
    if (!save_account(&acc)) {
        printf("Error: Failed to save account.\n");
        return;
    }
    
    // Add the account number to the index file 
    FILE *fp = fopen(INDEX_FILE, "a");  /* 'a' means append mode */
    if (fp != NULL) {
        fprintf(fp, "%s\n", acc.account_number);
        fclose(fp);
    }
    
    // Log this action to keep the records
    char log_msg[200];
    sprintf(log_msg, "Created account %s for %s", acc.account_number, acc.name);
    log_transaction(log_msg);
    
    /* STEP 7: 
       Show the user's updated account information along with a successful message

     */
    printf("\n========================================\n");
    printf("Account created successfully!\n");
    printf("========================================\n");
    printf("IMPORTANT: Please save this information!\n");
    printf("========================================\n");
    printf("Account Number: %s\n", acc.account_number);
    printf("Name: %s\n", acc.name);
    printf("Account Type: %s\n", account_type_to_string(acc.type));
    printf("PIN: ****\n");  
    printf("Initial Balance: RM%.2f\n", acc.balance);
    printf("========================================\n");
    printf("NOTE: Keep your Account Number and PIN\n");
    printf("      safe for future transactions.\n");
    printf("========================================\n");
}

/*
  DELETE ACCOUNT - Remove an Account from the System
  Once the user's identification has been confirmed, this function deletes a bank account
  It requires three security checks:
    1. There must be an account number
    2. The ID's final four characters must match
    3. The PIN needs to be accurate
  
  (This requires appropriate proof)
 */
void delete_account(void) {
    char account_num[20];      
    char id_last4[5];          
    char pin[PIN_LEN + 1];    
    
    printf("\n========================================\n");
    printf("         DELETE BANK ACCOUNT\n");
    printf("========================================\n");
    
    /* STEP 1: 
       Provide a list of all account numbers so the user is aware of the accounts
     */
    FILE *fp = fopen(INDEX_FILE, "r");
    if (fp == NULL) {
        printf("No accounts found.\n");
        return;
    }
    
    printf("Existing account numbers:\n");
    char line[100];
    int count = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = 0;
        printf("  - %s\n", line);
        count++;
    }
    fclose(fp);
    
    // Check if there are any accounts to delete
    if (count == 0) {
        printf("No accounts found.\n");
        return;
    }
    
    /* STEP 2: 
       Get the account number to delete
     */
    if (!get_string_input(account_num, 20, "\nEnter account number to delete: ")) {
        return;
    }
    
    // Try to load the account 
    Account acc;
    if (!load_account(account_num, &acc)) {
        printf("Error: Account not found.\n");
        return;
    }
    
    /* STEP 3: 
       Ask for the last four characters of ID as first security check
     */
    if (!get_string_input(id_last4, 5, "Enter last 4 characters of ID number: ")) {
        return;
    }
    
    /* STEP 4: Verify with PIN
       Ask for PIN as an additional security measure
     */
    if (!get_string_input(pin, PIN_LEN + 1, "Enter 4-digit PIN: ")) {
        return;
    }
    
    /* STEP 5: 
       Compare the last four characters of stored ID with what user have entered
     */
    int id_len = strlen(acc.id_number);
    if (id_len < 4 || strcmp(&acc.id_number[id_len - 4], id_last4) != 0) {
        printf("Error: ID verification failed.\n");
        return;
    }
    
    /* STEP 6: 
       Verify if the PIN matches or not
     */
    if (!authenticate(account_num, pin)) {
        printf("Error: PIN verification failed.\n");
        return;
    }
    
    char filename[100];
    sprintf(filename, "%s/%s.txt", DATABASE_DIR, account_num);
    if (remove(filename) != 0) {
        printf("Error: Failed to delete account file.\n");
        return;
    }
    
    FILE *index_fp = fopen(INDEX_FILE, "r");
    FILE *temp_fp = fopen("database/temp_index.txt", "w");
    
    if (index_fp != NULL && temp_fp != NULL) {
        while (fgets(line, sizeof(line), index_fp) != NULL) {
            line[strcspn(line, "\n")] = 0;
            if (strcmp(line, account_num) != 0) {
                fprintf(temp_fp, "%s\n", line);
            }
        }
        fclose(index_fp);
        fclose(temp_fp);
        
        remove(INDEX_FILE);
        rename("database/temp_index.txt", INDEX_FILE);
    }
    
    char log_msg[200];
    sprintf(log_msg, "Deleted account %s", account_num);
    log_transaction(log_msg);
    
    printf("\n========================================\n");
    printf("Account %s deleted successfully!\n", account_num);
    printf("========================================\n");
}
