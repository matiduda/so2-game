#ifndef CONNECT_H
#define CONNECT_H

#include "common.h"
#include "server_func.h"
#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void* player_connection(void* player_struct);
void player_connection_cleanup(void* data);
int reconnect_client(int* request_fd, int* response_fd, char* request_path, char* response_path);
int open_fifo(char* path, int read_or_write);
int make_fifo(char* path);

void clean_up_after_client(int id);

void create_response(player* player, server_data* response);

void* enemy_connection(void* enemy_struct);

#endif // CONNECT_H