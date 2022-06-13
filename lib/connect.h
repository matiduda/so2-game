#ifndef CONNECT_H
#define CONNECT_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include "common.h"
#include "server_func.h"

#define CLIENT_DATA_FIFO_PATH "client_data"
#define SERVER_DATA_FIFO_PATH "server_data"

typedef struct payload_client_request
{
    char key;
    pid_t pid;

} client_data;

typedef struct payload_server_response
{
    int ok;
    pid_t pid;
} server_data;


void *player_connection(void *player_struct);
int reconnect_client(int* request_fd, int* response_fd, char* request_path, char* response_path);
int open_fifo(char *path, int read_or_write);
int make_fifo(char *path);


void clean_up_after_client(int id);

#endif // CONNECT_H
