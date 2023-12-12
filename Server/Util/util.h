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

#define MAX_CONNECTIONS 1000

struct accepted_socket
{
    int socket_fd;
    struct sockaddr_in address;
    int error;
    bool accepted_successfully;
};

int create_tcp_socket(void);

struct sockaddr_in *create_adress(char *ip, int port);

void start_accepting_incoming_connections(int server_socket_fd);

struct accepted_socket *accept_incoming_connection(int server_socket_fd);

void serve_client_on_separate_thread(struct accepted_socket *client_socket);

void serve_client(int client_socket_fd);

void send_greeting_and_options(int client_socket_fd);

void get_chosen_option(int client_socket_fd);

void send_total_sum(int client_socket_fd);

#endif