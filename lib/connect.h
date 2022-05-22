#ifndef CONNECT_H
#define CONNECT_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>


#define FIFO_LOCATION_FOLDER "./pipes"
#define FIFO_NAME "connect_client"

#define LEN 256 // file path max length

// Client - Server connection

int clearFIFO();
int nextFilepath(char *dest, size_t size, const char *s1, const char *s2);

#endif // CONNECT_H
