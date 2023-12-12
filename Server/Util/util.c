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

// Konektovani klijenti.
static struct accepted_socket accepted_sockets[MAX_CONNECTIONS];

// Broj konektovanih klijenata.
static int accepted_sockets_count = 0;

// Inicijalizuje hvatanje nadolazećih klijenata.
void start_accepting_incoming_connections(int server_socket_fd)
{
    while (true)
    {
        struct accepted_socket *client_socket = accept_incoming_connection(server_socket_fd);
        accepted_sockets[accepted_sockets_count++] = *client_socket;

        serve_client_on_separate_thread(client_socket);
    }
}

// Hvata nadolazećeg klijenta i vraća ga.
struct accepted_socket *accept_incoming_connection(int server_socket_fd)
{
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(struct sockaddr_in);

    int client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_address, &client_address_size);

    if (client_socket_fd == -1)
    {
        perror("Desila se greška dok je server čekao na novu konekciju.\n");
        exit(EXIT_FAILURE);
    }

    struct accepted_socket *accepted_socket = malloc(sizeof(struct accepted_socket));
    accepted_socket->address = client_address;
    accepted_socket->socket_fd = client_socket_fd;
    accepted_socket->accepted_successfully = client_socket_fd > 0;

    if (!accepted_socket->accepted_successfully)
        accepted_socket->error = client_socket_fd;

    return accepted_socket;
}

// Kreira posebnu nit za opsluživanje pojedinačnog klijenta.
void serve_client_on_separate_thread(struct accepted_socket *client_socket)
{
    pthread_t id;
    pthread_create(&id, NULL, (void *)serve_client, client_socket->socket_fd);
}

// Opslužuje pojedinačnog klijenta.
void serve_client(int client_socket_fd)
{
    send_greeting_and_options(client_socket_fd);
    get_chosen_option(client_socket_fd);
}

// Šalje poruku o uspešnom povezivanju i opcije.
void send_greeting_and_options(int client_socket_fd)
{
    char *buffer = "***** Uspešno povezivanje sa serverom! *****\nOpcije:\n1) Izvrši uplatu\n2) Prikaži ukupno prikupljena sredstva\n3) Registracija\n4) Prijava\n";
    if (send(client_socket_fd, buffer, strlen(buffer), 0) == -1)
    {
        perror("Desila se greška pri slanju opcija klijentu.\n");
        exit(EXIT_FAILURE);
    }
}

void get_chosen_option(int client_socket_fd)
{
    int option;

    if (recv(client_socket_fd, &option, sizeof(option), 0) == -1)
    {
        perror("Desila se greška pri primanju izabrane opcije.\n");
        exit(EXIT_FAILURE);
    }

    switch (option)
    {
    case 1:
        // make_payment(client_socket_fd);
        break;
    case 2:
        send_total_sum(client_socket_fd);
        break;
    case 3:
        // sign_up(client_socket_fd);
        break;
    case 4:
        // sign_in(client_socket_fd);
        break;
    default:
        perror("Neispravan izbor!");
        close(client_socket_fd);
        exit(EXIT_FAILURE);
    }
}

static float total_sum = 0.0f;

void send_total_sum(int client_socket_fd)
{
    if (send(client_socket_fd, &total_sum, sizeof(total_sum), 0) == -1)
    {
        perror("Desila se greška pri slanju ukupne sume klijentu.\n");
        exit(EXIT_FAILURE);
    }
}