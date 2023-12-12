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

int create_tcp_socket(void);

struct sockaddr_in *create_adress(char *ip, int port);

void print_greeting_and_options(int client_socket_fd);

int choose_option(int client_socket_fd);

void get_server_response(int client_socket_fd, int option);

void make_payment(int client_socket_fd);

void show_total_sum(int client_socket_fd);

void sign_up(int client_socket_fd);

void sign_in(int client_socket_fd);

#endif