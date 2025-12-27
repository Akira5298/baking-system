#include <stdio.h>
#include <stdbool.h>
#include "types.h"
#include "menu.h"
#include "account.h"
#include "transaction.h"
#include "utils.h"

int main(void) {
    int choice;
    bool running = true;
    
    create_database_dir();
    
    int account_count = count_accounts();
    display_session_info(account_count);
    
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
