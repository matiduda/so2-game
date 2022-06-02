#ifndef SERVER_H
#define SERVER_H

#include "common.h"

#define ROUND_TIME_IN_SECONDS 5

// Function headers specific to the server

void checkArguments(int argc, char **argv, int* v, int* f);

int load_map(char *filepath, char dest[][MAX_WORLD_SIZE], point *size_res);

#endif // SERVER_H
