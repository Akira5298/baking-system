/*
 * menu.c - Menu functions implementation
 */

#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

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
 * Get menu choice from user (supports both numbers and keywords)
 */
int get_menu_choice(void) {
    char input[100];
    
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
