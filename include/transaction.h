/* Functions for financial transaction operations are declared in this file
   These functions handle money movements between accounts
 */

#ifndef TRANSACTION_H
#define TRANSACTION_H

/* Deposit Function
   Purpose: Add money to a customer's account
   
   Process:
   1. Authenticate the user
   2. Obtain and verify the deposit
   3. Add amount to account balance
   4. Save updated account
   5. Log the transaction
 */
void deposit(void);

/*
   Withdraw Function
   Purpose: Take money out of a customer's account
   
   Process:
   1. Authenticate the user
   2. Obtain and confirm the withdrawal sum
   3. Verify that there are enough cashs
   4. Deduct the sum from the account balance
   5. Save updated account
   6. Log the transaction
 */
void withdraw(void);

/*  Remittance Function
   Purpose: Transfer money from one account to another
  
   1. Authenticate sender
   2. Confirm the existence of the receiving account
   3. Obtain and verify the transfer amount
   4. Calculate the transfer fee based on account types
   5. Deduct amount + fee from sender
   6. Add the amount to receiver (receiver is not charged)
   7. Save both accounts
   8. Log the transaction
   
   Transfer Fees:
   - Savings → Current: 2%
   - Current → Savings: 3%
   - Same type: 1%
 */
void remittance(void);

#endif 
