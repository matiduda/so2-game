#ifndef SERVER_H
#define SERVER_H

#include "common.h"

#define ROUND_TIME_IN_SECONDS 5

#define MAP_LOCATION "map.txt"

// Data specific to the server

typedef struct player_info {
    sem_t received_data;
    sem_t map_calculated;
    int is_connected;
    char name[8];
    point pos;
    int ID;
    int PID;
    int type;
    int deaths;
    int coins_carried;
    int coins_brought;
} player;


int load_map(char *filepath, char dest[][MAX_WORLD_SIZE], point *size_res);

player init_player(int id);

void print_player_info(player pstruct);

#endif // SERVER_H
