/*
   This file handles everything involved in presenting the menu and 
   obtaining the user's choice. (Like a information desk at a bank)
 */

#include "menu.h"
#include <stdio.h>     
#include <stdlib.h>   
#include <string.h>  
#include <ctype.h>    
#include <time.h>    

//This function shows every banking operation that is available
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
  This displays the number of accounts, and the program start time
   
   Parameters:
     account_count - Number of accounts that are currently inside the system
 */
void display_session_info(int account_count) {
    // Get the current date and time in a readable format
    time_t now = time(NULL);        
    struct tm *t = localtime(&now);    
    
    // Display the session information 
    printf("\n========================================\n");
    printf("     BANKING SYSTEM - SESSION INFO\n");
    printf("========================================\n");
    // Show date in YYYY-MM-DD format and time in HH:MM:SS format
    printf("Date: %04d-%02d-%02d\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    printf("Time: %02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
    // displays how many accounts exist
    printf("Total Accounts: %d\n", account_count);
    printf("========================================\n");
}

/*
    This function reads what the user typed and figures out which menu option
  they want. 
  
  Examples:
      User enters "3" → returns 3 (deposit)
      User enters "deposit" → returns 3 (deposit)
      User enters "exit" → returns 6 (exit)
  
  Returns:
    1-6: these are valid menu option numbers
    -1: Invalid input (it doesn't match any option)
 */
int get_menu_choice(void) {
    char input[100];  
    

    /* STEP 1:
    Obtain a line of text from the user. Return an error if reading failed
    */
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return -1; 
    }
    
    /* STEP 2:
    Remove the newline character at the end (by pressing the Enter key) 
    */
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';  
    } else {
        //The input was too long - removes the remaining characters from buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    
    /* STEP 3: 
    To match keywords, convert to lowercase
      This enables us to match "Deposit," "DEPOSIT," and "deposit" in the same way
      by converting each character to lowercase
     */
    char lower[100];  
    int i;
    for (i = 0; input[i] && i < 99; i++) {
        lower[i] = tolower(input[i]);  
    }
    lower[i] = '\0';  
    
    /* STEP 4: 
       Try to convert the input (string) to a number (base 10)
     */
    char *endptr; 
    long num = strtol(input, &endptr, 10);  
    
    /* If conversion succeeded and number is valid (1-6), return it */
    if (*endptr == '\0' && num >= 1 && num <= 6) {
        return (int)num;
    }
    
    /* STEP 5: 
       Check to see if it matches any keywords if it wasn't a number.
       We use strstr() to determine whether the keyword appears anywhere in the input
     */
    if (strstr(lower, "create") != NULL) return 1;                   
    if (strstr(lower, "delete") != NULL || strstr(lower, "remove") != NULL) return 2;  
    if (strstr(lower, "deposit") != NULL) return 3;                  
    if (strstr(lower, "withdraw") != NULL || strstr(lower, "withdrawal") != NULL) return 4; 
    if (strstr(lower, "remit") != NULL || strstr(lower, "transfer") != NULL) return 5; 
    if (strstr(lower, "exit") != NULL || strstr(lower, "quit") != NULL) return 6;
    
    // If we arrive at this point, the input is invalid because it didn't match anything
    return -1;
}
