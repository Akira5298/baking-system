/*
 * transaction.c - Transaction operations implementation
 */

#include "transaction.h"
#include "account.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

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
