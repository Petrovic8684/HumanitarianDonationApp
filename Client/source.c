#include "Util/util.h"

int main(void)
{
    // Povezivanje klijenta na server.
    int client_socket_fd = create_tcp_socket();
    struct sockaddr_in *server_address = create_adress("192.168.0.16", 2010);

    if (connect(client_socket_fd, (struct sockaddr *)server_address, sizeof(*server_address)) != 0)
    {
        perror("Neuspešno povezivanje sa serverom.\n");
        exit(EXIT_FAILURE);
    }

    // Poziv glavne logike klijentske aplikacije.
    start_communicating_with_server(client_socket_fd);

    // Zatvaranje klijentskog socketa.
    if (shutdown(client_socket_fd, SHUT_RDWR) == -1)
    {
        perror("Neuspešno zatvaranje klijentskog socketa.\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}