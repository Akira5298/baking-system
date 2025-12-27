
#ifndef UTILS_H
#define UTILS_H

#include "types.h"

void clear_input_buffer(void);
bool get_string_input(char *buffer, int max_len, const char *prompt);
bool get_double_input(double *value, const char *prompt);
bool get_int_input(int *value, const char *prompt);

bool is_valid_pin(const char *pin);
bool is_valid_amount(double amount, double max);

void create_database_dir(void);
void log_transaction(const char *action);

const char* account_type_to_string(AccountType type);
AccountType string_to_account_type(const char *str);

#endif // UTILS_H
