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

#define FILENAME_SIZE 20
#define FILEPACK_SIZE 256

// struct payload_t
// {
//     int content_size;
//     char message[FILENAME_SIZE];
//     uint8_t file_content[FILEPACK_SIZE];
//     sem_t cs;           // sekcja krytyczna
//     sem_t client_query; // zapytanie od klienta
//     sem_t server_reply; // odpowiedź od serwera
// };

#endif // COMMON_H
