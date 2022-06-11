#ifndef SERVER_H
#define SERVER_H

#include "common.h"

#define ROUND_TIME_IN_SECONDS 5

// Function headers specific to the server

int load_map(char *filepath, char dest[][MAX_WORLD_SIZE], point *size_res);

player init_player(int id);

void print_player_info(player pstruct);

#endif // SERVER_H
