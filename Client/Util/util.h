#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct payment
{
    char *name;
    char *surname;
    char *address;
    char *card_no;
    short cvv;
    float ammount;
};

int create_tcp_socket(void);

struct sockaddr_in *create_adress(char *ip, int port);

void send_to_server(int client_socket_fd, char *data);

const char *receive_from_server(int client_socket_fd, bool print);

void start_communicating_with_server(int client_socket_fd);

int choose_and_send_option(int client_socket_fd);

void get_server_response(int client_socket_fd, int option);

void make_payment(int client_socket_fd);

void get_payment_data(struct payment *new_payment);

void display_total_sum(int client_socket_fd);

#endif