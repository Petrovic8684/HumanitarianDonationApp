#include "io.h"

void write_total_sum_to_file(float total_sum)
{
    FILE *file = fopen("totalsum.txt", "w");
    fprintf(file, "%f", total_sum);
    fclose(file);
}

void read_total_sum_from_file(float *const total_sum)
{
    FILE *file = fopen("totalsum.txt", "r");
    fscanf(file, "%f", total_sum);
    fclose(file);
}

bool write_user_to_file(struct user *new_user)
{
    FILE *file = fopen("users.txt", "a+");

    char *line = NULL;
    size_t len = 0;
    int read;

    char *token;

    while ((read = getline(&line, &len, file)) != -1)
    {
        token = strtok(line, " ");
        if (strcmp(token, new_user->username) == 0)
            return false;
    }

    fprintf(file, "%s %s %s %s %s %s %s\n", new_user->username, new_user->password, new_user->name, new_user->surname, new_user->jmbg, new_user->card_no, new_user->email);
    fclose(file);

    if (line)
        free(line);

    return true;
}