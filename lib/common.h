#ifndef COMMON_H
#define COMMON_H

// Header file for both server and client

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <ncurses.h>
#include <pthread.h>
#include <semaphore.h>

#define FIFO_LOCATION_FOLDER "./pipes"

#define CLIENTS 4

#define MAXLEN 256 // file path max length

// World parameters
#define MAX_WORLD_SIZE 128

#define CLIENT_FOV 2
#define CLIENT_MAP_SIZE (2 * CLIENT_FOV + 1)

#define CLIENT_LOG_FILE "client.log"
#define SERVER_LOG_FILE "server.log"

#define MAP_EMPTY ' '
#define MAP_WALL '-'
#define MAP_BUSHES '#'
#define MAP_BEAST '*'
#define MAP_CAMPSITE 'A'
#define MAP_COIN_DROPPED 'D' 
#define MAP_COIN_50 'T'
#define MAP_COIN_10 't'
#define MAP_COIN_1 'c'

#define CLIENT_DATA_FIFO_PATH "client_data"
#define SERVER_DATA_FIFO_PATH "server_data"

typedef struct point_t
{
    int x;
    int y;
} point;

typedef struct key_thread_info
{
    int key;
    pthread_mutex_t mutex;
} key_info;

typedef struct user_interface
{
    point world_size;
    WINDOW *game_window;
    WINDOW *stat_window;
    WINDOW *legend;
} ui;

enum colors { DEFAULT = 1, WALL, PLAYER, ENEMY, COIN, CAMPSITE, BUSHES };


// ----------------

typedef struct payload_client_request {
    int key;
    pid_t pid;
} client_data;

typedef struct payload_server_response {
    char map[MAX_WORLD_SIZE][MAX_WORLD_SIZE];
    point world_size;
    point player_position;
    pid_t pid;
    int ok;
    int number;
    int round_number;
    int type;
    int deaths;
    int coins_carried;
    int coins_brought;
} server_data;

struct thread_data_t {
    char *fifo_path;
    int fifo_fd;
};

// ------------------

void init_colors();
void init_windows(ui *interface, point world_size, int stat_height, int stat_width);
void print_legend(WINDOW *w, int Y, int X);

int make_folder_if_not_created(char *path);
int create_fifo_path(char *dest, int id, char *type);

int kbhit(void);
void *keyboard_input_func(void *pkey);

#endif // COMMON_H
