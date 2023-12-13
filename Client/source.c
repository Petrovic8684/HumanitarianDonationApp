#include "Util/util.h"

int main(void)
{
    // Povezivanje klijenta na server.
    int client_socket_fd = create_tcp_socket();
    struct sockaddr_in *server_address = create_adress("127.0.0.1", 2010);

    if (connect(client_socket_fd, (struct sockaddr *)server_address, sizeof(*server_address)) != 0)
    {
        perror("Neuspešno povezivanje sa serverom.\n");
        exit(EXIT_FAILURE);
    }

    // Poziv glavne logike klijentske aplikacije.
    start_communicating_with_server(client_socket_fd);

    return EXIT_SUCCESS;
}