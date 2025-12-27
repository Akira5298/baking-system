
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "types.h"

bool load_account(const char *account_num, Account *acc);
bool save_account(const Account *acc);
bool account_exists(const char *account_num);
bool authenticate(const char *account_num, const char *pin);

char* generate_account_number(void);
int count_accounts(void);
void create_account(void);
void delete_account(void);

#endif // ACCOUNT_H
