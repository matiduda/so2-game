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

typedef struct point_t {
    int x;
    int y;
} point;

typedef struct key_thread_info {
    char key;
    pthread_mutex_t mutex;
} key_info;

int create_fifo_path(char *dest, int id, char *type);
int kbhit(void);
void* keyboard_input_func(void *pkey);

#endif // COMMON_H
