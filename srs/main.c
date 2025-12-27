/*
  This is the entry point of our banking application
  (It is where the program starts running)
 */

#include <stdio.h>      
#include <stdbool.h>    
#include "types.h"     
#include "menu.h"       
#include "account.h"    
#include "transaction.h"
#include "utils.h"     


int main(void) {
    // Variables we'll need throughout the program 
    int choice;          
    bool running = true;  
    
     // Set up everything we need, before the bank opens
    
    // If the database folder doesn't already exist, create it
    create_database_dir();
    
    // Count the number of accounts and display session info
    int account_count = count_accounts();
    display_session_info(account_count);

    /* This loop keeps it running, until the user decides to stop using the program
       It's like the bank that is open and serving customers all day
     */
    while (running) {
        // Show the menu 
        display_menu();
        
        // Obtain the user's choice (either input a number from 1-6 or keyword)
        choice = get_menu_choice();
        
        // We call the appropriate function based on the user's selection
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
    
    // If program ends successfully
    return 0;
}
