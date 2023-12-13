#include "util.h"

// Vraća TCP socket.
int create_tcp_socket(void)
{
    int new_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (new_socket_fd == -1)
    {
        perror("Neuspešno kreiranje TCP socketa.\n");
        exit(EXIT_FAILURE);
    }

    return new_socket_fd;
}

// Na osnovu ip adrese i porta kreira i vraća strukturu sockaddr_in.
struct sockaddr_in *create_adress(char *ip, int port)
{
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    if (strlen(ip) == 0)
        address->sin_addr.s_addr = INADDR_ANY;
    else
        inet_pton(AF_INET, ip, &address->sin_addr.s_addr);

    return address;
}

// Šalje podatke na server.
void send_to_server(int client_socket_fd, char *data)
{
    if (send(client_socket_fd, data, 1024, 0) == -1)
    {
        perror("Desila se greška pri slanju podataka na server.\n");
        exit(EXIT_FAILURE);
    }
}

// Prima podatke sa servera.
const char *receive_from_server(int client_socket_fd, bool print)
{
    char response[1024];

    ssize_t ammount_received = recv(client_socket_fd, response, 1024, 0);

    if (ammount_received <= 0)
    {
        perror("Desila se greška pri primanju podataka od servera.\n");
        exit(EXIT_FAILURE);
    }

    char *result = malloc(strlen(response));
    strncpy(result, response, strlen(response));

    if (print)
    {
        puts(result);
    }

    return result;
}

// Glavna logika klijentske aplikacije.
void start_communicating_with_server(int client_socket_fd)
{
    (void)receive_from_server(client_socket_fd, true);

    int option;
    do
    {
        (void)receive_from_server(client_socket_fd, true);

        option = choose_and_send_option(client_socket_fd);
        get_server_response(client_socket_fd, option);
    } while (option != 0);

    close(client_socket_fd);
}

int choose_and_send_option(int client_socket_fd)
{
    int option;
    char option_str[1024];

    fprintf(stdout, "Vaš izbor: ");
    scanf("%d", &option);
    fprintf(stdout, "\n");

    sprintf(option_str, "%d", option);
    option_str[strlen(option_str)] = '\0';

    send_to_server(client_socket_fd, option_str);

    return option;
}

void get_server_response(int client_socket_fd, int option)
{
    switch (option)
    {
    case 1:
        make_payment(client_socket_fd);
        break;
    case 2:
        display_total_sum(client_socket_fd);
        break;
    case 3:
        // sign_up(client_socket_fd);
        break;
    case 4:
        // sign_in(client_socket_fd);
        break;
    case 0:
        // Zatvori socket
        break;
    default:
        fprintf(stdout, "Neispravan izbor!\n\n");
        option = -1;
        break;
    }
}

void make_payment(int client_socket_fd)
{
    struct payment new_payment;
    get_payment_data(&new_payment);

    send(client_socket_fd, &new_payment, sizeof(struct payment), 0);

    //(void)receive_from_server(client_socket_fd, true);
}

void get_payment_data(struct payment *new_payment)
{
    char *name = NULL;
    char *surname = NULL;
    char *address = NULL;
    char *card_no = NULL;
    short cvv;
    float ammount;

    size_t len = 0;

    getchar();
    fprintf(stdout, "Ime: ");
    getline(&name, &len, stdin);

    fprintf(stdout, "Prezime: ");
    getline(&surname, &len, stdin);

    fprintf(stdout, "Adresa: ");
    getline(&address, &len, stdin);

    fprintf(stdout, "Broj platne kartice: ");
    getline(&card_no, &len, stdin);

    fprintf(stdout, "CVV broj: ");
    scanf("%hd", &cvv);

    fprintf(stdout, "Iznos: ");
    scanf("%f", &ammount);

    fprintf(stdout, "\n");

    new_payment->name = name;
    new_payment->surname = surname;
    new_payment->address = address;
    new_payment->card_no = card_no;
    new_payment->cvv = cvv;
    new_payment->ammount = ammount;
}

void display_total_sum(int client_socket_fd)
{
    float total_sum = atof(receive_from_server(client_socket_fd, false));
    fprintf(stdout, "Ukupan iznos je: %f\n\n", total_sum);
}