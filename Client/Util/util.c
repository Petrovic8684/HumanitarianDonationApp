#include "util.h"
#include <stdint.h>

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

// Glavna logika klijentske aplikacije.
void start_communicating_with_server(int client_socket_fd)
{
    print_greeting_and_options(client_socket_fd);
    int option = choose_option(client_socket_fd);
    get_server_response(client_socket_fd, option);
}

// Prikazuje poruku o uspešnom povezivanju i opcije.
void print_greeting_and_options(int client_socket_fd)
{
    char buffer[1024];

    ssize_t ammount_received = recv(client_socket_fd, buffer, 1024, 0);
    if (ammount_received > 0)
    {
        buffer[ammount_received] = 0;
        fprintf(stdout, "%s\n", buffer);
    }
}

int choose_option(int client_socket_fd)
{
    int option;

    fprintf(stdout, "Vaš izbor: ");
    scanf("%d", &option);
    fprintf(stdout, "\n");

    if (option < 1 || option > 4)
    {
        perror("Neispravan izbor!");
        exit(EXIT_FAILURE);
    }

    if (send(client_socket_fd, &option, sizeof(option), 0) == -1)
    {
        perror("Desila se greška pri slanju izabrane opcije serveru.\n");
        exit(EXIT_FAILURE);
    }

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
        show_total_sum(client_socket_fd);
        break;
    case 3:
        sign_up(client_socket_fd);
        break;
    case 4:
        sign_in(client_socket_fd);
        break;
    default:
        perror("Neispravan izbor!");
        close(client_socket_fd);
        exit(EXIT_FAILURE);
    }
}

void make_payment(int client_socket_fd)
{
}

void show_total_sum(int client_socket_fd)
{
    float total_sum;

    ssize_t ammount_received = recv(client_socket_fd, &total_sum, sizeof(total_sum), 0);
    if (ammount_received > 0)
    {
        fprintf(stdout, "Ukupan iznos je: %f\n", total_sum);
    }
}

void sign_up(int client_socket_fd)
{
}

void sign_in(int client_socket_fd)
{
}
