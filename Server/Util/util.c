#include "util.h"
#include "Lists/lists.h"
#include "IO/io.h"

// Konektovani klijenti.
static struct list *list = NULL;
static int list_length = 0;

// Ukupno prikupljena suma.
static float total_sum = 0.0f;

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

// Šalje tekst klijentu.
void send_text_to_client(int server_socket_fd, char *text)
{
    if (send(server_socket_fd, text, 1024, 0) == -1)
    {
        perror("Desila se greška pri slanju podataka klijentu.\n");
        exit(EXIT_FAILURE);
    }
}

// Inicijalizuje hvatanje nadolazećih klijenata.
void start_accepting_incoming_connections(int server_socket_fd)
{
    read_total_sum_from_file(&total_sum);

    while (true)
    {
        struct accepted_socket *client_socket = accept_incoming_connection(server_socket_fd);
        // accepted_sockets[accepted_sockets_count++] = *client_socket;
        add(&list, &client_socket);
        fprintf(stdout, "Trenutan broj aktivnih korisnika: %d\n", ++list_length);

        pthread_t id;
        pthread_create(&id, NULL, (void *)serve_client, client_socket);
    }

    // clear(&list);
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

// Opslužuje pojedinačnog klijenta.
void serve_client(struct accepted_socket *client_socket)
{
    send_text_to_client(client_socket->socket_fd, "***** Uspešno povezivanje sa serverom! *****\n\0");

    int option = 0;
    do
    {
        send_text_to_client(client_socket->socket_fd, "Opcije:\nO) Izlaz\n1) Izvrši uplatu\n2) Prikaži ukupno prikupljena sredstva\n3) Registracija\n4) Prijava\n5) Poslednjih 10 uplata\n\n\0");
        option = get_chosen_option(client_socket->socket_fd);
    } while (option != 0);

    // Zatvaranje klijentskog socketa.
    if (shutdown(client_socket->socket_fd, SHUT_RDWR) == -1)
    {
        perror("Neuspešno zatvaranje klijentskog socketa.\n");
        exit(EXIT_FAILURE);
    }

    rmv(&list, &client_socket);
    fprintf(stdout, "Trenutan broj aktivnih korisnika: %d\n", --list_length);
}

// Grana logiku na osnovu opcije koju je poslao klijent.
int get_chosen_option(int client_socket_fd)
{
    int option;
    if (recv(client_socket_fd, &option, sizeof(option), 0) == -1)
    {
        perror("Greška pri primanju opcije od klijenta!\n");
        exit(EXIT_FAILURE);
    }

    switch (option)
    {
    case 1:
        make_payment(client_socket_fd);
        break;
    case 2:
        send_total_sum(client_socket_fd);
        break;
    case 3:
        sign_up(client_socket_fd);
        break;
    case 4:
        sign_in(client_socket_fd);
        break;
    case 5:
        display_payment_history(client_socket_fd);
        break;
    default:
        break;
    }

    return option;
}

void make_payment(int client_socket_fd)
{
    struct payment new_payment;
    new_payment.ammount = 0.0f;

    receive_payment_data(client_socket_fd, &new_payment);

    if (check_card_validity(new_payment.card_no) == false)
    {
        send_text_to_client(client_socket_fd, "***** Broj platne kartice ne postoji u bazi. *****\n\n");
        return;
    }

    total_sum += new_payment.ammount;
    write_total_sum_to_file(total_sum);
    write_payment_to_file(new_payment);

    send_text_to_client(client_socket_fd, "***** Hvala na uplati. *****\n\n");
    fprintf(stdout, "Upravo je uplaćeno %.3f dinara.\n", new_payment.ammount);
}

void receive_payment_data(int client_socket_fd, struct payment *new_payment)
{
    char name[1024];
    char surname[1024];
    char address[1024];
    char card_no[1024];
    char cvv[1024];
    float ammount;

    if (recv(client_socket_fd, name, 1024, 0) == -1)
    {
        perror("Greška u primanju imena.\n");
        exit(EXIT_FAILURE);
    }
    if (recv(client_socket_fd, surname, 1024, 0) == -1)
    {
        perror("Greška u primanju prezimena.\n");
        exit(EXIT_FAILURE);
    }
    if (recv(client_socket_fd, address, 1024, 0) == -1)
    {
        perror("Greška u primanju adrese.\n");
        exit(EXIT_FAILURE);
    }
    if (recv(client_socket_fd, card_no, 1024, 0) == -1)
    {
        perror("Greška u primanju broja kartice.\n");
        exit(EXIT_FAILURE);
    }
    if (recv(client_socket_fd, cvv, 1024, 0) == -1)
    {
        perror("Greška u primanju cvv broja.\n");
        exit(EXIT_FAILURE);
    }
    if (recv(client_socket_fd, &ammount, sizeof(ammount), 0) == -1)
    {
        perror("Greška u primanju iznosa.\n");
        exit(EXIT_FAILURE);
    }

    new_payment->name = name;
    new_payment->surname = surname;
    new_payment->address = address;
    new_payment->card_no = card_no;
    new_payment->cvv = cvv;
    new_payment->ammount = ammount;
}

void send_total_sum(int client_socket_fd)
{
    if (send(client_socket_fd, &total_sum, sizeof(total_sum), 0) == -1)
    {
        perror("Desila se greška pri slanju ukupnog iznosa klijentu.\n");
        exit(EXIT_FAILURE);
    }
}

void sign_up(int client_socket_fd)
{
    struct user new_user;

    receive_user_data(client_socket_fd, &new_user);

    if (check_card_validity(new_user.card_no) == false)
    {
        send_text_to_client(client_socket_fd, "***** Broj platne kartice ne postoji u bazi. *****\n");
        return;
    }

    if (write_user_to_file(&new_user) == false)
    {
        send_text_to_client(client_socket_fd, "***** Već postoji korisnik sa takvim korisničkim imenom. *****\n");
        return;
    }

    send_text_to_client(client_socket_fd, "***** Uspešno ste se registrovali. *****\n");
}

void receive_user_data(int client_socket_fd, struct user *new_user)
{
    char username[1024];
    char password[1024];
    char name[1024];
    char surname[1024];
    char jmbg[1024];
    char card_no[1024];
    char email[1024];

    if (recv(client_socket_fd, username, 1024, 0) == -1)
    {
        perror("Greška u slanju korisničkog imena.\n");
        exit(EXIT_FAILURE);
    }
    if (recv(client_socket_fd, password, 1024, 0) == -1)
    {
        perror("Greška u slanju lozinke.\n");
        exit(EXIT_FAILURE);
    }
    if (recv(client_socket_fd, name, 1024, 0) == -1)
    {
        perror("Greška u slanju imena.\n");
        exit(EXIT_FAILURE);
    }
    if (recv(client_socket_fd, surname, 1024, 0) == -1)
    {
        perror("Greška u slanju broja prezimena.\n");
        exit(EXIT_FAILURE);
    }
    if (recv(client_socket_fd, jmbg, 1024, 0) == -1)
    {
        perror("Greška u slanju jmbg-a.\n");
        exit(EXIT_FAILURE);
    }
    if (recv(client_socket_fd, card_no, 1024, 0) == -1)
    {
        perror("Greška u slanju broja kartice.\n");
        exit(EXIT_FAILURE);
    }
    if (recv(client_socket_fd, email, 1024, 0) == -1)
        perror("Greška u slanju email-a.\n");
    exit(EXIT_FAILURE);

    new_user->username = username;
    new_user->password = password;
    new_user->name = name;
    new_user->surname = surname;
    new_user->jmbg = jmbg;
    new_user->card_no = card_no;
    new_user->email = email;
}

void sign_in(int client_socket_fd)
{
    struct user user;
    char card_no[1024];
    card_no[0] = '\0';
    bool can_log_in = false;

    receive_user_data(client_socket_fd, &user);

    if (check_user_validity(user, card_no) == true)
        can_log_in = true;
    else
        can_log_in = false;

    if (send(client_socket_fd, &can_log_in, sizeof(can_log_in), 0) == -1)
    {
        perror("Greška pri prijavljivanju.\n");
        exit(EXIT_FAILURE);
    }

    if (send(client_socket_fd, card_no, 1024, 0) == -1)
    {
        perror("Greška pri prijavljivanju.\n");
        exit(EXIT_FAILURE);
    }
}

void display_payment_history(int client_socket_fd)
{
    char payments[16384];
    payments[0] = '\0';

    read_payments_from_file(payments);

    if (send(client_socket_fd, payments, 16384, 0) == -1)
    {
        perror("Greška pri slanju istorije placanja.\n");
        exit(EXIT_FAILURE);
    }
}