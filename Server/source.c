#include "Util/util.h"

int main(void)
{
    // Inicijalizacija serverskog socketa.
    int server_socket_fd = create_tcp_socket();
    struct sockaddr_in *server_address = create_adress("", 2000);

    if (bind(server_socket_fd, (struct sockaddr *)server_address, sizeof(*server_address)) != 0)
    {
        perror("Neuspešna inicijalizacija serverskog soketa.\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket_fd, MAX_CONNECTIONS) == -1)
    {
        perror("Serverski socket nije u stanju da prihvata konekcije.\n");
        exit(EXIT_FAILURE);
    }

    // Poziv glavne logike serverske aplikacije.
    start_accepting_incoming_connections(server_socket_fd);

    // Zatvaranje serverskog socketa.
    if (shutdown(server_socket_fd, SHUT_RDWR) == -1)
    {
        perror("Neuspešno zatvaranje serverskog socketa.\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}