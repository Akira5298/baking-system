/*
 * Banking System Application
 * COMP1312 Programming I Coursework
 * 
 * This program implements a complete banking system with the following features:
 * - Create new bank accounts
 * - Delete existing accounts
 * - Deposit money
 * - Withdraw money
 * - Transfer money between accounts (remittance)
 * - Transaction logging
 * 
 * All account data is stored in the 'database' directory
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <errno.h>

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

// Function prototypes
void display_menu(void);
void display_session_info(int account_count);
void log_transaction(const char *action);
int get_menu_choice(void);
void clear_input_buffer(void);
bool get_string_input(char *buffer, int max_len, const char *prompt);
bool get_double_input(double *value, const char *prompt);
bool get_int_input(int *value, const char *prompt);

// Account management functions
void create_account(void);
void delete_account(void);
void deposit(void);
void withdraw(void);
void remittance(void);

// Helper functions
bool load_account(const char *account_num, Account *acc);
bool save_account(const Account *acc);
bool account_exists(const char *account_num);
char* generate_account_number(void);
bool authenticate(const char *account_num, const char *pin);
int count_accounts(void);
void create_database_dir(void);
bool is_valid_pin(const char *pin);
bool is_valid_amount(double amount, double max);
const char* account_type_to_string(AccountType type);
AccountType string_to_account_type(const char *str);

/*
 * Main function - Entry point of the program
 */
int main(void) {
    int choice;
    bool running = true;
    
    // Create database directory if it doesn't exist
    create_database_dir();
    
    // Display session information
    int account_count = count_accounts();
    display_session_info(account_count);
    
    // Main program loop
    while (running) {
        display_menu();
        choice = get_menu_choice();
        
        switch (choice) {
            case 1:
                create_account();
                break;
            case 2:
                delete_account();
                break;
            case 3:
                deposit();
                break;
            case 4:
                withdraw();
                break;
            case 5:
                remittance();
                break;
            case 6:
                printf("\nThank you for using our Banking System!\n");
                log_transaction("User exited the system");
                running = false;
                break;
            default:
                printf("\nInvalid option. Please select a valid menu option.\n");
        }
    }
    
    return 0;
}

/*
 * Display the main menu
 */
void display_menu(void) {
    printf("\n");
    printf("========================================\n");
    printf("         BANKING SYSTEM MENU\n");
    printf("========================================\n");
    printf("1. Create New Bank Account\n");
    printf("2. Delete Bank Account\n");
    printf("3. Deposit\n");
    printf("4. Withdrawal\n");
    printf("5. Remittance\n");
    printf("6. Exit\n");
    printf("========================================\n");
    printf("Enter your choice (number or keyword): ");
}

/*
 * Display session information (date/time and account count)
 */
void display_session_info(int account_count) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    printf("\n========================================\n");
    printf("     BANKING SYSTEM - SESSION INFO\n");
    printf("========================================\n");
    printf("Date: %04d-%02d-%02d\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    printf("Time: %02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
    printf("Total Accounts: %d\n", account_count);
    printf("========================================\n");
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
 * Get menu choice from user (supports both numbers and keywords)
 */
int get_menu_choice(void) {
    char input[100];
    int choice;
    
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return -1;
    }
    
    // Remove newline
    input[strcspn(input, "\n")] = 0;
    
    // Convert to lowercase for keyword matching
    char lower[100];
    int i;
    for (i = 0; input[i] && i < 99; i++) {
        lower[i] = tolower(input[i]);
    }
    lower[i] = '\0';
    
    // Try to parse as number first
    char *endptr;
    long num = strtol(input, &endptr, 10);
    if (*endptr == '\0' && num >= 1 && num <= 6) {
        return (int)num;
    }
    
    // Check for keywords
    if (strstr(lower, "create") != NULL) return 1;
    if (strstr(lower, "delete") != NULL || strstr(lower, "remove") != NULL) return 2;
    if (strstr(lower, "deposit") != NULL) return 3;
    if (strstr(lower, "withdraw") != NULL || strstr(lower, "withdrawal") != NULL) return 4;
    if (strstr(lower, "remit") != NULL || strstr(lower, "transfer") != NULL) return 5;
    if (strstr(lower, "exit") != NULL || strstr(lower, "quit") != NULL) return 6;
    
    return -1; // Invalid choice
}

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
 * Count total number of accounts
 */
int count_accounts(void) {
    FILE *fp = fopen(INDEX_FILE, "r");
    if (fp == NULL) {
        return 0;
    }
    
    int count = 0;
    char line[100];
    while (fgets(line, sizeof(line), fp) != NULL) {
        count++;
    }
    
    fclose(fp);
    return count;
}

/*
 * Check if account exists
 */
bool account_exists(const char *account_num) {
    FILE *fp = fopen(INDEX_FILE, "r");
    if (fp == NULL) {
        return false;
    }
    
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
 * Generate unique account number
 */
char* generate_account_number(void) {
    static char account_num[20];
    srand(time(NULL));
    
    int attempts = 0;
    do {
        // Generate random number between 7-9 digits
        int digits = 7 + (rand() % 3); // 7, 8, or 9 digits
        
        long long num = 0;
        for (int i = 0; i < digits; i++) {
            num = num * 10 + (rand() % 10);
        }
        
        // Ensure minimum 7 digits
        if (num < 1000000) {
            num += 1000000;
        }
        
        sprintf(account_num, "%lld", num);
        
        attempts++;
        if (attempts > 100) {
            return NULL; // Failed to generate unique number
        }
    } while (account_exists(account_num));
    
    return account_num;
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

/*
 * Load account data from file
 */
bool load_account(const char *account_num, Account *acc) {
    char filename[100];
    sprintf(filename, "%s/%s.txt", DATABASE_DIR, account_num);
    
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return false;
    }
    
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
    
    acc->type = string_to_account_type(type_str);
    fclose(fp);
    return true;
}

/*
 * Save account data to file
 */
bool save_account(const Account *acc) {
    char filename[100];
    sprintf(filename, "%s/%s.txt", DATABASE_DIR, acc->account_number);
    
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        return false;
    }
    
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
 * Authenticate user with account number and PIN
 */
bool authenticate(const char *account_num, const char *pin) {
    Account acc;
    if (!load_account(account_num, &acc)) {
        return false;
    }
    
    return strcmp(acc.pin, pin) == 0;
}

/*
 * Create a new bank account
 */
void create_account(void) {
    Account acc;
    char input[100];
    
    printf("\n========================================\n");
    printf("       CREATE NEW BANK ACCOUNT\n");
    printf("========================================\n");
    
    // Get name
    if (!get_string_input(acc.name, MAX_NAME_LEN, "Enter full name: ")) {
        printf("Error: Failed to read name.\n");
        return;
    }
    
    // Get ID number
    if (!get_string_input(acc.id_number, MAX_ID_LEN, "Enter ID number: ")) {
        printf("Error: Failed to read ID number.\n");
        return;
    }
    
    // Get account type
    printf("Select account type:\n");
    printf("1. Savings\n");
    printf("2. Current\n");
    printf("Enter choice: ");
    
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error: Failed to read account type.\n");
        return;
    }
    
    int type_choice = atoi(input);
    if (type_choice == 1) {
        acc.type = SAVINGS;
    } else if (type_choice == 2) {
        acc.type = CURRENT;
    } else {
        printf("Error: Invalid account type.\n");
        return;
    }
    
    // Get PIN
    do {
        if (!get_string_input(acc.pin, PIN_LEN + 1, "Enter 4-digit PIN: ")) {
            printf("Error: Failed to read PIN.\n");
            return;
        }
        
        if (!is_valid_pin(acc.pin)) {
            printf("Error: PIN must be exactly 4 digits.\n");
        }
    } while (!is_valid_pin(acc.pin));
    
    // Generate account number
    char *account_num = generate_account_number();
    if (account_num == NULL) {
        printf("Error: Failed to generate unique account number.\n");
        return;
    }
    strcpy(acc.account_number, account_num);
    
    // Set initial balance
    acc.balance = 0.0;
    
    // Save account to file
    if (!save_account(&acc)) {
        printf("Error: Failed to save account.\n");
        return;
    }
    
    // Add to index file
    FILE *fp = fopen(INDEX_FILE, "a");
    if (fp != NULL) {
        fprintf(fp, "%s\n", acc.account_number);
        fclose(fp);
    }
    
    // Log transaction
    char log_msg[200];
    sprintf(log_msg, "Created account %s for %s", acc.account_number, acc.name);
    log_transaction(log_msg);
    
    printf("\n========================================\n");
    printf("Account created successfully!\n");
    printf("Account Number: %s\n", acc.account_number);
    printf("Name: %s\n", acc.name);
    printf("Account Type: %s\n", account_type_to_string(acc.type));
    printf("Initial Balance: RM%.2f\n", acc.balance);
    printf("========================================\n");
}

/*
 * Delete an existing bank account
 */
void delete_account(void) {
    char account_num[20];
    char id_last4[5];
    char pin[PIN_LEN + 1];
    
    printf("\n========================================\n");
    printf("         DELETE BANK ACCOUNT\n");
    printf("========================================\n");
    
    // Show existing accounts
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
    
    if (count == 0) {
        printf("No accounts found.\n");
        return;
    }
    
    // Get account details for verification
    if (!get_string_input(account_num, 20, "\nEnter account number to delete: ")) {
        return;
    }
    
    // Load account to verify
    Account acc;
    if (!load_account(account_num, &acc)) {
        printf("Error: Account not found.\n");
        return;
    }
    
    if (!get_string_input(id_last4, 5, "Enter last 4 characters of ID number: ")) {
        return;
    }
    
    if (!get_string_input(pin, PIN_LEN + 1, "Enter 4-digit PIN: ")) {
        return;
    }
    
    // Verify credentials
    int id_len = strlen(acc.id_number);
    if (id_len < 4 || strcmp(&acc.id_number[id_len - 4], id_last4) != 0) {
        printf("Error: ID verification failed.\n");
        return;
    }
    
    if (!authenticate(account_num, pin)) {
        printf("Error: PIN verification failed.\n");
        return;
    }
    
    // Delete account file
    char filename[100];
    sprintf(filename, "%s/%s.txt", DATABASE_DIR, account_num);
    if (remove(filename) != 0) {
        printf("Error: Failed to delete account file.\n");
        return;
    }
    
    // Remove from index file
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
    
    // Log transaction
    char log_msg[200];
    sprintf(log_msg, "Deleted account %s", account_num);
    log_transaction(log_msg);
    
    printf("\n========================================\n");
    printf("Account %s deleted successfully!\n", account_num);
    printf("========================================\n");
}

/*
 * Deposit money into an account
 */
void deposit(void) {
    char account_num[20];
    char pin[PIN_LEN + 1];
    double amount;
    
    printf("\n========================================\n");
    printf("              DEPOSIT\n");
    printf("========================================\n");
    
    // Get account number
    if (!get_string_input(account_num, 20, "Enter account number: ")) {
        return;
    }
    
    // Get PIN
    if (!get_string_input(pin, PIN_LEN + 1, "Enter 4-digit PIN: ")) {
        return;
    }
    
    // Authenticate
    if (!authenticate(account_num, pin)) {
        printf("Error: Authentication failed.\n");
        return;
    }
    
    // Load account
    Account acc;
    if (!load_account(account_num, &acc)) {
        printf("Error: Failed to load account.\n");
        return;
    }
    
    printf("Current Balance: RM%.2f\n", acc.balance);
    
    // Get deposit amount
    if (!get_double_input(&amount, "Enter deposit amount (RM): ")) {
        return;
    }
    
    // Validate amount
    if (!is_valid_amount(amount, MAX_DEPOSIT)) {
        return;
    }
    
    // Update balance
    acc.balance += amount;
    
    // Save account
    if (!save_account(&acc)) {
        printf("Error: Failed to save account.\n");
        return;
    }
    
    // Log transaction
    char log_msg[200];
    sprintf(log_msg, "Deposit: Account %s, Amount: RM%.2f", account_num, amount);
    log_transaction(log_msg);
    
    printf("\n========================================\n");
    printf("Deposit successful!\n");
    printf("Amount Deposited: RM%.2f\n", amount);
    printf("New Balance: RM%.2f\n", acc.balance);
    printf("========================================\n");
}

/*
 * Withdraw money from an account
 */
void withdraw(void) {
    char account_num[20];
    char pin[PIN_LEN + 1];
    double amount;
    
    printf("\n========================================\n");
    printf("            WITHDRAWAL\n");
    printf("========================================\n");
    
    // Get account number
    if (!get_string_input(account_num, 20, "Enter account number: ")) {
        return;
    }
    
    // Get PIN
    if (!get_string_input(pin, PIN_LEN + 1, "Enter 4-digit PIN: ")) {
        return;
    }
    
    // Authenticate
    if (!authenticate(account_num, pin)) {
        printf("Error: Authentication failed.\n");
        return;
    }
    
    // Load account
    Account acc;
    if (!load_account(account_num, &acc)) {
        printf("Error: Failed to load account.\n");
        return;
    }
    
    printf("Available Balance: RM%.2f\n", acc.balance);
    
    // Get withdrawal amount
    if (!get_double_input(&amount, "Enter withdrawal amount (RM): ")) {
        return;
    }
    
    // Validate amount
    if (amount <= 0) {
        printf("Error: Amount must be greater than RM0.\n");
        return;
    }
    
    if (amount > acc.balance) {
        printf("Error: Insufficient funds. Please try again.\n");
        return;
    }
    
    // Update balance
    acc.balance -= amount;
    
    // Save account
    if (!save_account(&acc)) {
        printf("Error: Failed to save account.\n");
        return;
    }
    
    // Log transaction
    char log_msg[200];
    sprintf(log_msg, "Withdrawal: Account %s, Amount: RM%.2f", account_num, amount);
    log_transaction(log_msg);
    
    printf("\n========================================\n");
    printf("Withdrawal successful!\n");
    printf("Amount Withdrawn: RM%.2f\n", amount);
    printf("New Balance: RM%.2f\n", acc.balance);
    printf("========================================\n");
}

/*
 * Transfer money between accounts (remittance)
 */
void remittance(void) {
    char sender_num[20];
    char sender_pin[PIN_LEN + 1];
    char receiver_num[20];
    double amount;
    
    printf("\n========================================\n");
    printf("            REMITTANCE\n");
    printf("========================================\n");
    
    // Get sender account number
    if (!get_string_input(sender_num, 20, "Enter sender account number: ")) {
        return;
    }
    
    // Get sender PIN
    if (!get_string_input(sender_pin, PIN_LEN + 1, "Enter sender 4-digit PIN: ")) {
        return;
    }
    
    // Authenticate sender
    if (!authenticate(sender_num, sender_pin)) {
        printf("Error: Authentication failed.\n");
        return;
    }
    
    // Load sender account
    Account sender;
    if (!load_account(sender_num, &sender)) {
        printf("Error: Failed to load sender account.\n");
        return;
    }
    
    printf("Sender Balance: RM%.2f\n", sender.balance);
    
    // Get receiver account number
    if (!get_string_input(receiver_num, 20, "Enter receiver account number: ")) {
        return;
    }
    
    // Validate accounts are distinct
    if (strcmp(sender_num, receiver_num) == 0) {
        printf("Error: Cannot transfer to the same account.\n");
        return;
    }
    
    // Load receiver account
    Account receiver;
    if (!load_account(receiver_num, &receiver)) {
        printf("Error: Receiver account not found.\n");
        return;
    }
    
    printf("Receiver: %s (%s)\n", receiver.name, account_type_to_string(receiver.type));
    
    // Get transfer amount
    if (!get_double_input(&amount, "Enter transfer amount (RM): ")) {
        return;
    }
    
    // Calculate remittance fee
    double fee = 0.0;
    if (sender.type == SAVINGS && receiver.type == CURRENT) {
        fee = amount * 0.02; // 2% fee
        printf("Remittance Fee (2%%): RM%.2f\n", fee);
    } else if (sender.type == CURRENT && receiver.type == SAVINGS) {
        fee = amount * 0.03; // 3% fee
        printf("Remittance Fee (3%%): RM%.2f\n", fee);
    }
    
    double total_deduction = amount + fee;
    printf("Total Deduction: RM%.2f\n", total_deduction);
    
    // Validate amount
    if (amount <= 0) {
        printf("Error: Amount must be greater than RM0.\n");
        return;
    }
    
    if (total_deduction > sender.balance) {
        printf("Error: Insufficient funds. Please try again.\n");
        return;
    }
    
    // Perform transfer
    sender.balance -= total_deduction;
    receiver.balance += amount;
    
    // Save both accounts
    if (!save_account(&sender) || !save_account(&receiver)) {
        printf("Error: Failed to save accounts.\n");
        return;
    }
    
    // Log transaction
    char log_msg[300];
    sprintf(log_msg, "Remittance: From %s to %s, Amount: RM%.2f, Fee: RM%.2f",
            sender_num, receiver_num, amount, fee);
    log_transaction(log_msg);
    
    printf("\n========================================\n");
    printf("Remittance successful!\n");
    printf("Amount Transferred: RM%.2f\n", amount);
    printf("Remittance Fee: RM%.2f\n", fee);
    printf("Sender New Balance: RM%.2f\n", sender.balance);
    printf("Receiver New Balance: RM%.2f\n", receiver.balance);
    printf("========================================\n");
}
