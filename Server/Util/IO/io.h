#ifndef _IO_H_
#define _IO_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "../util.h"

void write_total_sum_to_file(float total_sum);

void read_total_sum_from_file(float *total_sum);

bool write_user_to_file(struct user *new_user);

bool check_user_validity(struct user user, char *card_no);

void read_payments_from_file(char *payments);

void write_payment_to_file(struct payment new_payment);

bool check_card_and_ccv_validity(char *card_no, char *cvv);

bool check_card_validity(char *card_no);

#endif