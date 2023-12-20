#include "io.h"

void write_total_sum_to_file(float total_sum)
{
    FILE *file = fopen("totalsum.txt", "w");
    fprintf(file, "%f", total_sum);
    fclose(file);
}

void read_total_sum_from_file(float *total_sum)
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
        {
            fclose(file);

            if (line)
                free(line);

            return false;
        }
    }

    fprintf(file, "%s %s %s %s %s %s %s\n", new_user->username, new_user->password, new_user->card_no, new_user->name, new_user->surname, new_user->jmbg, new_user->email);
    fclose(file);

    if (line)
        free(line);

    return true;
}

bool check_user_validity(struct user user, char *card_no)
{
    FILE *file = fopen("users.txt", "r");

    char *line = NULL;
    size_t len = 0;
    int read;

    char *token;

    while ((read = getline(&line, &len, file)) != -1)
    {
        token = strtok(line, " ");
        if (strcmp(token, user.username) == 0)
        {
            token = strtok(NULL, " ");
            if (strcmp(token, user.password) == 0)
            {
                token = strtok(NULL, " ");
                strcpy(card_no, token);

                fclose(file);

                if (line)
                    free(line);

                return true;
            }
        }
    }

    fclose(file);

    if (line)
        free(line);

    return false;
}

void read_payments_from_file(char *payments)
{
    FILE *file = fopen("payments.txt", "r");

    char *line = NULL;
    size_t len = 0;
    int read;

    for (int i = 0; i < 10; i++)
    {
        if ((read = getline(&line, &len, file)) == -1)
            return;

        strcat(payments, line);
    }
}

void write_payment_to_file(struct payment new_payment)
{
    FILE *file = fopen("payments.txt", "a+");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    fprintf(file, "%s %s %d-%02d-%02d %02d:%02d:%02d %f\n", new_payment.name, new_payment.surname, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, new_payment.ammount);
    fclose(file);
}

bool check_card_and_ccv_validity(char *card_no, char *cvv)
{
    FILE *file = fopen("validcards.txt", "r");

    char *line = NULL;
    size_t len = 0;
    int read;

    char *token;

    while ((read = getline(&line, &len, file)) == -1)
    {
        token = strtok(line, " ");
        if (strcmp(token, card_no) == 0)
        {
            token = strtok(NULL, " ");
            if (strcmp(token, cvv) == 0)
            {
                fclose(file);

                if (line)
                    free(line);

                return true;
            }
        }
    }

    fclose(file);

    if (line)
        free(line);

    return false;
}

bool check_card_validity(char *card_no)
{
    FILE *file = fopen("validcards.txt", "r");

    char *line = NULL;
    size_t len = 0;
    int read;

    char *token;

    while ((read = getline(&line, &len, file)) != -1)
    {
        token = strtok(line, " ");
        if (strcmp(token, card_no) == 0)
        {

            fclose(file);

            if (line)
                free(line);

            return true;
        }
    }

    fclose(file);

    if (line)
        free(line);

    return false;
}