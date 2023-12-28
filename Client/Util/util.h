#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>
#include <regex.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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

char *receive_text_from_server(int client_socket_fd, bool print);

void start_communicating_with_server(int client_socket_fd);

int choose_and_send_option(int client_socket_fd);

void get_server_response(int client_socket_fd, int option);

void make_payment(int client_socket_fd);

void get_payment_data(struct payment *new_payment);

void send_payment_data(int client_socket_fd, struct payment new_payment);

bool is_card_no_valid(char *card_no);

bool is_cvv_valid(char *const cvv);

void display_total_sum(int client_socket_fd);

void sign_up(int client_socket_fd);

void get_user_data(struct user *new_user);

void send_user_data(int client_socket_fd, struct user new_user);

void sign_in(int client_socket_fd);

void display_payment_history(int client_socket_fd);

void write_payment_to_file(char *response);

#endif