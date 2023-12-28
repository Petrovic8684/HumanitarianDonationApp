#include "util.h"

// Da li je korisnik prijavljen ili ne?
static bool is_logged_in = false;

// Broj platne kartice prijavljenog korisnika.
static char logged_card_no[1024];

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

// Prima tekst sa servera.
char *receive_text_from_server(int client_socket_fd, bool print)
{
    char *response = (char *)malloc(1024);
    response[0] = '\0';

    ssize_t ammount_received = recv(client_socket_fd, response, 1024, 0);

    if (ammount_received <= 0)
    {
        perror("Desila se greška pri primanju podataka od servera.\n");
        exit(EXIT_FAILURE);
    }

    if (print == true)
        fprintf(stdout, "%s", response);

    return response;
}

// Glavna logika klijentske aplikacije.
void start_communicating_with_server(int client_socket_fd)
{
    // Štampa poruku o uspešnom povezivanju.
    (void)receive_text_from_server(client_socket_fd, true);

    int option;
    do
    {
        // Štampa opcije.
        (void)receive_text_from_server(client_socket_fd, true);

        option = choose_and_send_option(client_socket_fd);
        get_server_response(client_socket_fd, option);
    } while (option != 0);
}

// Bira opcije sa tastature i šalje na server.
int choose_and_send_option(int client_socket_fd)
{
    int option = -1;
    char text[1024];

    while (true)
    {
        fprintf(stdout, "Vaš izbor: ");
        if (scanf("%d", &option) == 1)
        {

            if (is_logged_in == true && option == 4)
            {
                fprintf(stdout, "\n***** Već ste prijavljeni. *****\n\n");
            }
            else if (is_logged_in == false && option == 5)
            {
                fprintf(stdout, "\n***** Za tu opciju morate biti prijavljeni. *****\n\n");
            }
            else if (option >= 0 && option <= 5)
                break;
            else
                fprintf(stdout, "\n***** Neispravan unos opcije. *****\n\n");
        }
        else if (scanf("%s", text) != 0)
            fprintf(stdout, "\n***** Neispravan unos opcije. *****\n\n");
    }

    if (send(client_socket_fd, &option, sizeof(option), 0) == -1)
    {
        perror("Greška pri slanju opcije na server.\n");
        exit(EXIT_FAILURE);
    }

    return option;
}

// Prima odgovor servera na opciju.
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
}

// Kreiranje nove uplate.
void make_payment(int client_socket_fd)
{
    struct payment new_payment;
    get_payment_data(&new_payment);

    send_payment_data(client_socket_fd, new_payment);

    (void)receive_text_from_server(client_socket_fd, true);

    char *response = receive_text_from_server(client_socket_fd, false);
    write_payment_to_file(response);
}

// Učitavanje podataka o uplati sa tastature.
void get_payment_data(struct payment *new_payment)
{
    char *name = NULL;
    char *surname = NULL;
    char *address = NULL;
    char *card_no = NULL;
    char *cvv = NULL;
    float ammount;

    size_t len = 0;
    int char_count = 0;

    getchar();
    fprintf(stdout, "Ime: ");
    char_count = getline(&name, &len, stdin);

    name[char_count - 1] = '\0';

    fprintf(stdout, "Prezime: ");
    char_count = getline(&surname, &len, stdin);

    surname[char_count - 1] = '\0';

    fprintf(stdout, "Adresa: ");
    char_count = getline(&address, &len, stdin);

    address[char_count - 1] = '\0';

    if (is_logged_in == false)
    {
        while (true)
        {
            fprintf(stdout, "Broj platne kartice: ");
            char_count = getline(&card_no, &len, stdin);
            card_no[char_count - 1] = '\0';

            if (is_card_no_valid(card_no))
                break;
            else
                fprintf(stdout, "***** Broj plate kartice mora biti u formatu xxxx-xxxx-xxxx-xxxx. *****\n");
        }
    }
    else
    {
        card_no = logged_card_no;
    }

    while (true)
    {
        fprintf(stdout, "CVV broj: ");
        char_count = getline(&cvv, &len, stdin);
        cvv[char_count - 1] = '\0';

        if (is_cvv_valid(cvv))
            break;
        else
            fprintf(stdout, "***** CVV mora biti trocifren broj. *****\n");
    }

    char text[1024];

    while (true)
    {
        fprintf(stdout, "Iznos: ");
        if (scanf("%f", &ammount) == 1)
        {
            if (ammount >= 200.0f)
                break;
            else
                fprintf(stdout, "***** Iznos mora biti bar 200 rsd. *****\n");
        }
        else if (scanf("%s", text) != 0)
            fprintf(stdout, "***** Iznos mora biti bar 200 rsd. *****\n");
    }

    fprintf(stdout, "\n");

    new_payment->name = name;
    new_payment->surname = surname;
    new_payment->address = address;
    new_payment->card_no = card_no;
    new_payment->cvv = cvv;
    new_payment->ammount = ammount;
}

// Slanje podataka o uplati na server.
void send_payment_data(int client_socket_fd, struct payment new_payment)
{
    if (send(client_socket_fd, new_payment.name, 1024, 0) == -1)
    {
        perror("Greška u slanju imena.\n");
        exit(EXIT_FAILURE);
    }
    if (send(client_socket_fd, new_payment.surname, 1024, 0) == -1)
    {
        perror("Greška u slanju prezimena.\n");
        exit(EXIT_FAILURE);
    }
    if (send(client_socket_fd, new_payment.address, 1024, 0) == -1)
    {
        perror("Greška u slanju adrese.\n");
        exit(EXIT_FAILURE);
    }
    if (send(client_socket_fd, new_payment.card_no, 1024, 0) == -1)
    {
        perror("Greška u slanju broja kartice.\n");
        exit(EXIT_FAILURE);
    }
    if (send(client_socket_fd, new_payment.cvv, 1024, 0) == -1)
    {
        perror("Greška u slanju cvv broja.\n");
        exit(EXIT_FAILURE);
    }
    if (send(client_socket_fd, &new_payment.ammount, sizeof(new_payment.ammount), 0) == -1)
    {
        perror("Greška u slanju iznosa.\n");
        exit(EXIT_FAILURE);
    }
}

// Preko regularnih izraza proverava ispravnost formata unetog broja platne kartice.
bool is_card_no_valid(char *card_no)
{
    if (strlen(card_no) != 19)
    {
        return false;
    }

    regex_t regex;
    int result;

    result = regcomp(&regex, "[0-9][0-9][0-9][0-9]-[0-9][0-9][0-9][0-9]-[0-9][0-9][0-9][0-9]-[0-9][0-9][0-9][0-9]", REG_EXTENDED);
    if (result)
    {

        perror("Nemoguće izraditi regularni izraz.\n");
        exit(EXIT_FAILURE);
    }

    result = regexec(&regex, card_no, 0, NULL, 0);
    if (!result)
        return true;
    else if (result == REG_NOMATCH)
        return false;
    else
    {

        perror("Desila se greška pri upoređivanju sa regularnim izrazom.\n");
        exit(EXIT_FAILURE);
    }
}

// Proverava ispravnost cvv broja.
bool is_cvv_valid(char *const cvv)
{
    if (strlen(cvv) > 4)
        return false;

    for (int i = 0; i < 3; i++)
        if (cvv[i] < '0' || cvv[i] > '9')
            return false;

    return true;
}

// Prikazuje ukupno prikupljena sredstva.
void display_total_sum(int client_socket_fd)
{
    float total_sum;
    if (recv(client_socket_fd, &total_sum, sizeof(total_sum), 0) == -1)
    {

        perror("Desila se greška pri prijemu ukupnog iznosa sa servera.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "\nUkupan iznos je: %f\n\n", total_sum);
}

// Registrovanje novog korisnika.
void sign_up(int client_socket_fd)
{
    struct user new_user;
    get_user_data(&new_user);

    send_user_data(client_socket_fd, new_user);

    (void)receive_text_from_server(client_socket_fd, true);
    fprintf(stdout, "\n");
}

// Učitavanje podataka o novom korisniku sa tastature.
void get_user_data(struct user *new_user)
{
    char *username = NULL;
    char *password = NULL;
    char *name = NULL;
    char *surname = NULL;
    char *jmbg = NULL;
    char *card_no = NULL;
    char *email = NULL;

    size_t len = 0;
    int char_count = 0;

    getchar();

    fprintf(stdout, "Korisničko ime: ");
    char_count = getline(&username, &len, stdin);

    username[char_count - 1] = '\0';

    fprintf(stdout, "Lozinka: ");
    char_count = getline(&password, &len, stdin);

    password[char_count - 1] = '\0';

    fprintf(stdout, "Ime: ");
    char_count = getline(&name, &len, stdin);

    name[char_count - 1] = '\0';

    fprintf(stdout, "Prezime: ");
    char_count = getline(&surname, &len, stdin);

    surname[char_count - 1] = '\0';

    fprintf(stdout, "JMBG: ");
    char_count = getline(&jmbg, &len, stdin);

    jmbg[char_count - 1] = '\0';

    while (true)
    {
        fprintf(stdout, "Broj platne kartice: ");
        char_count = getline(&card_no, &len, stdin);

        card_no[char_count - 1] = '\0';

        if (is_card_no_valid(card_no))
            break;
        else
            fprintf(stdout, "***** Broj plate kartice mora biti u formatu xxxx-xxxx-xxxx-xxxx. *****\n");
    }

    fprintf(stdout, "Email: ");
    char_count = getline(&email, &len, stdin);

    email[char_count - 1] = '\0';

    fprintf(stdout, "\n");

    new_user->username = username;
    new_user->password = password;
    new_user->name = name;
    new_user->surname = surname;
    new_user->jmbg = jmbg;
    new_user->card_no = card_no;
    new_user->email = email;
}

// Slanje podataka o novom korisniku na server.
void send_user_data(int client_socket_fd, struct user new_user)
{
    if (send(client_socket_fd, new_user.username, 1024, 0) == -1)
    {
        perror("Greška u slanju korisničkog imena.\n");
        exit(EXIT_FAILURE);
    }
    if (send(client_socket_fd, new_user.password, 1024, 0) == -1)
    {
        perror("Greška u slanju lozinke.\n");
        exit(EXIT_FAILURE);
    }
    if (send(client_socket_fd, new_user.name, 1024, 0) == -1)
    {
        perror("Greška u slanju imena.\n");
        exit(EXIT_FAILURE);
    }
    if (send(client_socket_fd, new_user.surname, 1024, 0) == -1)
    {
        perror("Greška u slanju broja prezimena.\n");
        exit(EXIT_FAILURE);
    }
    if (send(client_socket_fd, new_user.jmbg, 1024, 0) == -1)
    {
        perror("Greška u slanju jmbg-a.\n");
        exit(EXIT_FAILURE);
    }
    if (send(client_socket_fd, new_user.card_no, 1024, 0) == -1)
    {
        perror("Greška u slanju broja kartice.\n");
        exit(EXIT_FAILURE);
    }
    if (send(client_socket_fd, new_user.email, 1024, 0) == -1)
    {
        perror("Greška u slanju email-a.\n");
        exit(EXIT_FAILURE);
    }
}

// Prijavljivanje korisnika.
void sign_in(int client_socket_fd)
{
    struct user user;

    char *username = NULL;
    char *password = NULL;

    user.name = user.card_no = user.email = user.jmbg = user.surname = "";

    size_t len = 0;
    int char_count = 0;

    getchar();
    fprintf(stdout, "Korisničko ime: ");
    char_count = getline(&username, &len, stdin);

    username[char_count - 1] = '\0';

    fprintf(stdout, "Lozinka: ");
    char_count = getline(&password, &len, stdin);

    password[char_count - 1] = '\0';

    user.username = username;
    user.password = password;

    bool can_log_in = false;

    send_user_data(client_socket_fd, user);

    if (recv(client_socket_fd, &can_log_in, sizeof(can_log_in), 0) == -1)
    {
        perror("Greška pri prijavljivanju.\n");
        exit(EXIT_FAILURE);
    }

    logged_card_no[0] = '\0';

    if (recv(client_socket_fd, logged_card_no, 1024, 0) == -1)
    {
        perror("Greška pri prijavljivanju.\n");
        exit(EXIT_FAILURE);
    }

    if (can_log_in == true)
    {
        is_logged_in = true;
        fprintf(stdout, "\n***** Uspešno ste se prijavili. *****\n\n");
    }
    else
    {
        is_logged_in = false;
        fprintf(stdout, "\n***** Neuspela prijava. *****\n\n");
    }
}

// Prikaz poslednjih 10 uplata. Samo za prijavljene korisnike.
void display_payment_history(int client_socket_fd)
{
    char buffer[16384];
    if (recv(client_socket_fd, buffer, 16384, 0) == -1)
    {
        perror("Greška pri primanju istorije placanja.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "%s\n\n", buffer);
}

void write_payment_to_file(char *response)
{
    FILE *file = fopen("payment.txt", "w");

    if (file == NULL)
    {
        perror("Nemoguće otvoriti fajl totalsum.txt");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s\n", response);

    fclose(file);
}