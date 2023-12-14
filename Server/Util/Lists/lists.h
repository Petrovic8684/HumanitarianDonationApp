#ifndef _LISTS_H_
#define _LISTS_H_

#include <stdio.h>
#include <stdlib.h>

struct list
{
    void *data;
    struct list *next;
};

void add(struct list **head, void *value);

void rmv(struct list **head, void *value);

void clear(struct list **head);

#endif