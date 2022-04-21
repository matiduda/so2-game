#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <semaphore.h> // sem*
#include <fcntl.h>     // stałe O_*
#include <unistd.h>    // close(), ftruncate()
#include <sys/types.h>
#include <sys/mman.h> // mmap, munmap, shm_open, shm_unlink
#include <string.h>
#include <stdint.h>

#define FILENAME_SIZE 20
#define FILEPACK_SIZE 256

#define SHM_NAME "shared_mem"

struct payload_t
{
    int content_size;
    char message[FILENAME_SIZE];
    uint8_t file_content[FILEPACK_SIZE];
    sem_t cs;           // sekcja krytyczna
    sem_t client_query; // zapytanie od klienta
    sem_t server_reply; // odpowiedź od serwera
};

#endif // COMMON_H
