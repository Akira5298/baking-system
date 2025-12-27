/*
 * Banking System Application - Main Entry Point
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
#include <stdbool.h>
#include "types.h"
#include "menu.h"
#include "account.h"
#include "transaction.h"
#include "utils.h"

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
