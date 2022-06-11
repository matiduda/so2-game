#ifndef CONNECT_H
#define CONNECT_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include "common.h"

// Client - Server connection and data excange

void *player_connection(void *player_struct);

int open_fifo(char *path);
int get_client_info(int fifo_descriptor, client_info *info);
int reconnect_client(int fd, char* path, client_info *info);

void clean_up_after_client(int id);

#endif // CONNECT_H
