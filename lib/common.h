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

#define LOGGING false

#define CLIENT_LOG_FILE "client.log"
#define SERVER_LOG_FILE "server.log"

typedef struct point_t
{
    int x;
    int y;
} point;

typedef struct key_thread_info
{
    char key;
    pthread_mutex_t mutex;
} key_info;

typedef struct user_interface
{
    point world_size;
    WINDOW *game_window;
    WINDOW *stat_window;
    WINDOW *legend;
} ui;

void init_windows(ui *interface, point world_size);
void update_windows(ui interface, char dest[][MAX_WORLD_SIZE]);

int make_folder_if_not_created(char *path);
int create_fifo_path(char *dest, int id, char *type);

int kbhit(void);
void *keyboard_input_func(void *pkey);

FILE* configure_logging(char *path);
int log_this(FILE *f, char *msg);

#endif // COMMON_H
