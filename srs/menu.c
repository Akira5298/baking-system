#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

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

int get_menu_choice(void) {
    char input[100];
    
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return -1;
    }
    
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    
    char lower[100];
    int i;
    for (i = 0; input[i] && i < 99; i++) {
        lower[i] = tolower(input[i]);
    }
    lower[i] = '\0';
    
    char *endptr;
    long num = strtol(input, &endptr, 10);
    if (*endptr == '\0' && num >= 1 && num <= 6) {
        return (int)num;
    }
    
    if (strstr(lower, "create") != NULL) return 1;
    if (strstr(lower, "delete") != NULL || strstr(lower, "remove") != NULL) return 2;
    if (strstr(lower, "deposit") != NULL) return 3;
    if (strstr(lower, "withdraw") != NULL || strstr(lower, "withdrawal") != NULL) return 4;
    if (strstr(lower, "remit") != NULL || strstr(lower, "transfer") != NULL) return 5;
    if (strstr(lower, "exit") != NULL || strstr(lower, "quit") != NULL) return 6;
    
    return -1;
}
