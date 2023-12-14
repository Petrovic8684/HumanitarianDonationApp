#ifndef _IO_H_
#define _IO_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../util.h"

void write_total_sum_to_file(float total_sum);

void read_total_sum_from_file(float *const total_sum);

bool write_user_to_file(struct user *new_user);

#endif