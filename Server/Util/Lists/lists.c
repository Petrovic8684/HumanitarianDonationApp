#include "lists.h"

// Dodaje novi element u jednostruko spregnutu listu.
void add(struct list **head, void *value)
{
    struct list *new_node = (struct list *)malloc(sizeof(struct list));
    if (new_node == NULL)
    {
        fprintf(stderr, "Neuspela alokacija memorije za listu.\n");
        exit(EXIT_FAILURE);
    }

    new_node->data = value;
    new_node->next = NULL;

    if (*head == NULL)
    {
        *head = new_node;
    }
    else
    {
        struct list *current = *head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new_node;
    }
}

// Uklanja element iz jednostruko spregnute liste.
void rmv(struct list **head, void *value)
{
    if (*head == NULL)
    {
        return;
    }

    if ((*head)->data == value)
    {
        struct list *temp = *head;
        *head = (*head)->next;
        free(temp);
        return;
    }

    struct list *current = *head;
    struct list *prev = NULL;
    while (current != NULL && current->data != value)
    {
        prev = current;
        current = current->next;
    }

    if (current == NULL)
    {
        return;
    }

    prev->next = current->next;
    free(current);
}

// Briše sve elemente jednostruko spregnute liste.
void clear(struct list **head)
{
    struct list *current = *head;
    while (current != NULL)
    {
        struct list *temp = current;
        current = current->next;
        free(temp->data);
        free(temp);
    }

    *head = NULL;
}