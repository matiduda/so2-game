#ifndef COMMON_H
#define COMMON_H

// Header file for both server and client

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ncurses.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FIFO_LOCATION_FOLDER "./pipes"

#define CLIENTS 4

#define ROUND_TIME_IN_SEC10TH 10

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

#define ENEMY_MAX_ENEMIES 10
#define ENEMY_FOV 2
#define ENEMY_MAP_SIZE (2 * ENEMY_FOV + 1)

enum direction { STANDING,
    UP,
    DOWN,
    LEFT,
    RIGHT };

typedef struct point_t {
    int x;
    int y;
} point;

typedef struct key_thread_info {
    int key;
    pthread_mutex_t mutex;
} key_info;

typedef struct user_interface {
    point world_size;
    WINDOW* game_window;
    WINDOW* stat_window;
    WINDOW* legend;
} ui;

enum colors { DEFAULT = 1,
    WALL,
    PLAYER,
    ENEMY,
    COIN,
    CAMPSITE,
    BUSHES };

// ----------------

typedef struct payload_client_request {
    int key;
    int bot;
    pid_t pid;
} client_data;

typedef struct payload_server_response {
    char map[MAX_WORLD_SIZE][MAX_WORLD_SIZE];
    point world_size;
    point player_position;
    point campsite_xy;
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
    char* fifo_path;
    int fifo_fd;
};

struct enemy_view {
    char map[ENEMY_MAP_SIZE][ENEMY_MAP_SIZE];
};

struct enemy_request {
    struct enemy_view maps[ENEMY_MAX_ENEMIES];
    point positions[ENEMY_MAX_ENEMIES];
    int active_enemies;
};

struct enemy_response {
    pid_t client_pid;
    enum direction direction[ENEMY_MAX_ENEMIES];
    int active_enemies;
};

// ------------------

void init_colors();
void init_windows(ui* interface, point world_size, int stat_height, int stat_width);
void print_legend(WINDOW* w, int Y, int X);

int make_folder_if_not_created(char* path);
int create_fifo_path(char* dest, int id, char* type);

int kbhit(void);
void* keyboard_input_func(void* pkey);

#endif // COMMON_H
