
#include "account.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

char* generate_account_number(void) {
    static char account_num[20];
    srand(time(NULL));
    
    int attempts = 0;
    do {
        int digits = 7 + (rand() % 3); // 7, 8, or 9 digits
        
        long long num = 0;
        for (int i = 0; i < digits; i++) {
            num = num * 10 + (rand() % 10);
        }
        
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

bool authenticate(const char *account_num, const char *pin) {
    Account acc;
    if (!load_account(account_num, &acc)) {
        return false;
    }
    
    return strcmp(acc.pin, pin) == 0;
}

void create_account(void) {
    Account acc;
    char input[100];
    
    printf("\n========================================\n");
    printf("       CREATE NEW BANK ACCOUNT\n");
    printf("========================================\n");
    
    if (!get_string_input(acc.name, MAX_NAME_LEN, "Enter full name: ")) {
        printf("Error: Failed to read name.\n");
        return;
    }
    
    if (!is_valid_name(acc.name)) {
        return;
    }
    
    if (!get_string_input(acc.id_number, MAX_ID_LEN, "Enter ID number: ")) {
        printf("Error: Failed to read ID number.\n");
        return;
    }
    
    if (!is_valid_id(acc.id_number)) {
        return;
    }
    
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
    
    do {
        if (!get_string_input(acc.pin, PIN_LEN + 1, "Enter 4-digit PIN: ")) {
            printf("Error: Failed to read PIN.\n");
            return;
        }
        
        if (!is_valid_pin(acc.pin)) {
            printf("Error: PIN must be exactly 4 digits.\n");
        }
    } while (!is_valid_pin(acc.pin));
    
    char *account_num = generate_account_number();
    if (account_num == NULL) {
        printf("Error: Failed to generate unique account number.\n");
        return;
    }
    strcpy(acc.account_number, account_num);
    
    acc.balance = 0.0;
    
    if (!save_account(&acc)) {
        printf("Error: Failed to save account.\n");
        return;
    }
    
    FILE *fp = fopen(INDEX_FILE, "a");
    if (fp != NULL) {
        fprintf(fp, "%s\n", acc.account_number);
        fclose(fp);
    }
    
    char log_msg[200];
    sprintf(log_msg, "Created account %s for %s", acc.account_number, acc.name);
    log_transaction(log_msg);
    
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

void delete_account(void) {
    char account_num[20];
    char id_last4[5];
    char pin[PIN_LEN + 1];
    
    printf("\n========================================\n");
    printf("         DELETE BANK ACCOUNT\n");
    printf("========================================\n");
    
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
    
    if (!get_string_input(account_num, 20, "\nEnter account number to delete: ")) {
        return;
    }
    
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
    
    int id_len = strlen(acc.id_number);
    if (id_len < 4 || strcmp(&acc.id_number[id_len - 4], id_last4) != 0) {
        printf("Error: ID verification failed.\n");
        return;
    }
    
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
