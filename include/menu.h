/* Functions related to the menu system and user interface are declared in this file
   These functions handle displaying menus and getting user choices
 */

#ifndef MENU_H
#define MENU_H

/* Display menu function
   Purpose: Show the main menu with all available options
   
   Displays:
   - Banking System title
   - List of operations (deposit, withdraw, transfer, etc.)
   - Exit option
 */
void display_menu(void);

/* Display session info function
   Purpose: Show information about the current session

   Parameters:
   - account_count: Number of accounts in the system at the moment
   
   Displays:
   - Current date and time
   - Total number of accounts
 */
void display_session_info(int account_count);

/* 
   Get the menu choice function
   Purpose: Obtain the user's menu selection
   
   Returns: An integer that represents the selected menu item
   
   Features:
   - Accepts numeric input (1-7)
   - Accepts keyword input ("deposit", "withdraw", etc.)
   - Verifies input and replies if it is valid
 */
int get_menu_choice(void);

#endif // MENU_H
