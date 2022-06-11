#ifndef COMMON_H
#define COMMON_H

// Header file for both server and client

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <ncurses.h>
#include <pthread.h>

#define FIFO_LOCATION_FOLDER "./pipes"

#define CLIENTS 4

#define FIFO_CLIENT_INP "client_inp"
#define FIFO_CLIENT_OUT "client_out"

#define MAXLEN 256 // file path max length

// World parameters
#define MAX_WORLD_SIZE 128

typedef struct point_t {
    int x;
    int y;
} point;

typedef struct player_info {
    char name[8];
    point pos;
    int ID;
    int PID;
    int type;
    int deaths;
    int coins_carried;
    int coins_brought;
} player;

typedef struct payload_client_data
{
    char key;
} client_data;

typedef struct payload_client_info
{
    pid_t pid;
} client_info;

int create_fifo_path(char *dest, int id, char *type);

#endif // COMMON_H
