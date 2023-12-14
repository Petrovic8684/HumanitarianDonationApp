#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_CONNECTIONS 10000

struct accepted_socket
{
    int socket_fd;
    struct sockaddr_in address;
    int error;
    bool accepted_successfully;
};

struct payment
{
    char *name;
    char *surname;
    char *address;
    char *card_no;
    char *cvv;
    float ammount;
};

struct user
{
    char *username;
    char *password;
    char *name;
    char *surname;
    char *jmbg;
    char *card_no;
    char *email;
};

int create_tcp_socket(void);

struct sockaddr_in *create_adress(char *ip, int port);

void send_text_to_client(int server_socket_fd, char *text);

void start_accepting_incoming_connections(int server_socket_fd);

struct accepted_socket *accept_incoming_connection(int server_socket_fd);

void serve_client(struct accepted_socket *client_socket);

int get_chosen_option(int client_socket_fd);

void make_payment(int client_socket_fd);

void receive_payment_data(int client_socket_fd, struct payment *new_payment);

void send_total_sum(int client_socket_fd);

void sign_up(int client_socket_fd);

void receive_user_data(int client_socket_fd, struct user *new_user);

#endif