/*
All financial transactions, including deposits, withdrawals, 
and transfers (remittances), are managed by this file. (It's like a bank cashier)

#include "transaction.h"
#include "account.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>


 /* Users can deposit money to their accounts using this function 
  
   Process:
     1. Obtain the account number and PIN
     2. Authenticate user
     3. Obtain and verify the deposit amount
     4. Add money to balance
     5. Save updated account
 */
void deposit(void) {
    char account_num[20];     
    char pin[PIN_LEN + 1];    
    double amount;              
    
    printf("\n========================================\n");
    printf("              DEPOSIT\n");
    printf("========================================\n");
    

    // STEP 1: Get Account Information

    if (!get_string_input(account_num, 20, "Enter account number: ")) {
        return;
    }
    
    if (!get_string_input(pin, PIN_LEN + 1, "Enter 4-digit PIN: ")) {
        return;
    }
    
    /* STEP 2: Verify User Identity
       Verify the account's existence and PIN.
     */
    if (!authenticate(account_num, pin)) {
        printf("Error: Authentication failed.\n");
        return;
    }
    
     // STEP 3: Load the Account Data

    Account acc;
    if (!load_account(account_num, &acc)) {
        printf("Error: Failed to load account.\n");
        return;
    }
    
    // Show the current balance
    printf("Current Balance: RM%.2f\n", acc.balance);
    

     /* STEP 4: Obtain and Validate Deposit Amount
       Find out the user's desired deposit amount
       Validation checks: correct decimal format, positive amount, and not too large
     */
    if (!get_double_input(&amount, "Enter deposit amount (RM): ")) {
        return;
    }
    
    if (!is_valid_amount(amount, MAX_DEPOSIT)) {
        return; 
    }
    
    /* STEP 5: Update the Account Balance
       Increase the account balance by the deposit amount
     */
    acc.balance += amount;
    
    // STEP 6: Save the Updated Account
    if (!save_account(&acc)) {
        printf("Error: Failed to save account.\n");
        return;
    }
    
    /* 
       STEP 7: Record the Transaction in Log
       Keep a record of this deposit for audit purposes
     */
    char log_msg[200];
    sprintf(log_msg, "Deposit: Account %s, Amount: RM%.2f", account_num, amount);
    log_transaction(log_msg);
    
     // STEP 8: Show the Success Message
    printf("\n========================================\n");
    printf("Deposit successful!\n");
    printf("Amount Deposited: RM%.2f\n", amount);
    printf("New Balance: RM%.2f\n", acc.balance);
    printf("========================================\n");
}

/* 
   WITHDRAW FUNCTION
   Purpose: Take money from a customer's account
   
   How it works:
   1. Request the account number and PIN 
   2. Confirm the identity of the user
   3. Enter the account information
   4. Verify that the account has sufficient money
   5. Subtract the amount of the withdrawal
   6. Save the updated account
   7. Record the transaction you've made
   
   Important: You can't take out more money than the account balance
 */
void withdraw(void) {
    char account_num[20];     
    char pin[PIN_LEN + 1];   
    double amount;          
    
    printf("\n========================================\n");
    printf("            WITHDRAWAL\n");
    printf("========================================\n");
    
     //STEP 1: Get the Account Information
    if (!get_string_input(account_num, 20, "Enter account number: ")) {
        return;
    }
    
    if (!get_string_input(pin, PIN_LEN + 1, "Enter 4-digit PIN: ")) {
        return;
    }

     // STEP 2: Verify User's Identity
    if (!authenticate(account_num, pin)) {
        printf("Error: Authentication failed.\n");
        return;
    }
    
     // STEP 3: Load the Account Data
    Account acc;
    if (!load_account(account_num, &acc)) {
        printf("Error: Failed to load account.\n");
        return;
    }
    
    // Show the balance available
    printf("Available Balance: RM%.2f\n", acc.balance);
    

     // STEP 4: Get the Desired Withdrawal Amount
    if (!get_double_input(&amount, "Enter withdrawal amount (RM): ")) {
        return;
    }
    
    /* STEP 5: Validate the Withdrawal Amount
       Check 1: Amount must be positive
       Check 2: It must not exceed the account balance 
       Check 3: It must be within the maximum withdrawal amount allowed
     */
    if (amount <= 0) {
        printf("Error: Amount must be greater than RM0.\n");
        return;
    }
    
    // Verify that the account has sufficient money 
    if (amount > acc.balance) {
        printf("Error: Insufficient funds. Please try again.\n");
        return;
    }
    

    /* STEP 6: Update Balance
       Take the withdrawal amount and deduct it from the account balance
     */
    acc.balance -= amount;
    
    /* 
       STEP 7: Save the Updated Account
     */
    if (!save_account(&acc)) {
        printf("Error: Failed to save account.\n");
        return;
    }
    
     // STEP 8: Record the Transaction in Log
    char log_msg[200];
    sprintf(log_msg, "Withdrawal: Account %s, Amount: RM%.2f", account_num, amount);
    log_transaction(log_msg);
    
     // STEP 9: Show a Success Message
    printf("\n========================================\n");
    printf("Withdrawal successful!\n");
    printf("Amount Withdrawn: RM%.2f\n", amount);
    printf("New Balance: RM%.2f\n", acc.balance);
    printf("========================================\n");
}

/* REMITTANCE FUNCTION
  The purpose is to transfer money from one account to another 
 
  How it works:
  1. Request the sender's account number and PIN
  2. Confirm the sender's identity
  3. Load the sender's account information
  4. Request the account number of the recipient
  5. Confirm the existence of the receiving account
  6. Load the account information of the recipient
  7. Obtain the transfer amount and validate
  8. Calculate transfer fee (different rates for different account types)
  9. Deduct the amount + fee from the sender 
  10. Give the recipient the money (the recipient does not pay the fee)
  11. Save both accounts
  12. Record the transaction
  
  Important: Transfer fees depend on account types:
    Savings to Current: 2% fee
    Current to Savings: 3% fee
    Same type (S->S or C->C): 1% fee
 */
void remittance(void) {
    char sender_num[20];        
    char sender_pin[PIN_LEN + 1]; 
    char receiver_num[20];   
    double amount;       
    
    printf("\n========================================\n");
    printf("            REMITTANCE\n");
    printf("========================================\n");

     // STEP 1: Obtain Sender's Information
    if (!get_string_input(sender_num, 20, "Enter sender account number: ")) {
        return;
    }
    
    if (!get_string_input(sender_pin, PIN_LEN + 1, "Enter sender 4-digit PIN: ")) {
        return;
    }
    

     // STEP 2: Verify Sender's Identity
    if (!authenticate(sender_num, sender_pin)) {
        printf("Error: Authentication failed.\n");
        return;
    }
    

     // STEP 3: Load Sender's Account Data
    Account sender;
    if (!load_account(sender_num, &sender)) {
        printf("Error: Failed to load sender account.\n");
        return;
    }
    
    // Show sender's balance that is available
    printf("Sender Balance: RM%.2f\n", sender.balance);
    
  
     // STEP 4: Obtain Receiver's Account Number
    if (!get_string_input(receiver_num, 20, "Enter receiver account number: ")) {
        return;
    }

    /* STEP 5: Validate Receiver Account
     * Check 1: You can't transfer to yourself
     * Check 2: There must be a receiver account.
     */
    if (strcmp(sender_num, receiver_num) == 0) {
        printf("Error: Cannot transfer to the same account.\n");
        return;
    }

    // STEP 6: Load Receiver's Account Data
    Account receiver;
    if (!load_account(receiver_num, &receiver)) {
        printf("Error: Receiver account not found.\n");
        return;
    }
    
    // Show the information of the receiver
    printf("Receiver: %s (%s)\n", receiver.name, account_type_to_string(receiver.type));
    
    // STEP 7: Get the Transfer Amount
    if (!get_double_input(&amount, "Enter transfer amount (RM): ")) {
        return;
    }
    
    /* STEP 8: Calculate Transfer Fee
       Fee rates depend on account types:
       - Savings → Current: 2% fee (cheaper rate)
       - Current → Savings: 3% fee (higher rate)
       - Same type: 1% fee (default rate)
       
       NOTE: The recipient does not pay the fee; only the sender does
     */
    double fee = 0.0;
    if (sender.type == SAVINGS && receiver.type == CURRENT) {
        fee = amount * 0.02; 
        printf("Remittance Fee (2%%): RM%.2f\n", fee);
    } else if (sender.type == CURRENT && receiver.type == SAVINGS) {
        fee = amount * 0.03; 
        printf("Remittance Fee (3%%): RM%.2f\n", fee);
    }
    
    // Calculate the total amount that sender will pay
    double total_deduction = amount + fee;
    printf("Total Deduction: RM%.2f\n", total_deduction);
    
    /* STEP 9: Validate Transfer Amount
       Check 1: The amount must be positive
       Check 2: The sender must have enough money to send (amount + fee)
     */
    if (amount <= 0) {
        printf("Error: Amount must be greater than RM0.\n");
        return;
    }
    
    if (total_deduction > sender.balance) {
        printf("Error: Insufficient funds. Please try again.\n");
        return;
    }
    
    /* STEP 10: Update the Balances of Both Accounts
       Sender: loses amount + fee
       Receiver: only gains the amount 
     */
    sender.balance -= total_deduction;
    receiver.balance += amount;
    
    /* STEP 11: Save Both Accounts
       The transaction is unsuccessful unless both accounts are saved correctly
     */
    if (!save_account(&sender) || !save_account(&receiver)) {
        printf("Error: Failed to save accounts.\n");
        return;
    }
    
  // STEP 12: Record the Transaction in Log
    char log_msg[300];
    sprintf(log_msg, "Remittance: From %s to %s, Amount: RM%.2f, Fee: RM%.2f",
            sender_num, receiver_num, amount, fee);
    log_transaction(log_msg);
    
     // STEP 13: Show a Success Message
    printf("\n========================================\n");
    printf("Remittance successful!\n");
    printf("Amount Transferred: RM%.2f\n", amount);
    printf("Remittance Fee: RM%.2f\n", fee);
    printf("Sender New Balance: RM%.2f\n", sender.balance);
    printf("Receiver New Balance: RM%.2f\n", receiver.balance);
    printf("========================================\n");
}
