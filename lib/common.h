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

// Communication parameters
#define FILENAME_SIZE 20 // for FIFO path
#define PAYLIAD_SIZE sizeof(payload)

// World parameters
#define MAX_WORLD_SIZE 128

typedef struct payload_t
{
    int content_size;
    // sem_t cs;           // sekcja krytyczna
    // sem_t client_query; // zapytanie od klienta
    // sem_t server_reply; // odpowiedź od serwera
} payload;

typedef struct point_t {
    int x;
    int y;
} point;

#endif // COMMON_H
