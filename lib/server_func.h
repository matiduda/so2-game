#ifndef SERVER_H
#define SERVER_H

#include "common.h"

#define ROUND_TIME_IN_SECONDS 5

#define MAP_LOCATION "map.txt"

// Data specific to the server

typedef struct player_info {
    sem_t received_data;
    sem_t map_calculated;
    char map[MAX_WORLD_SIZE][MAX_WORLD_SIZE];
    point world_size;
    point pos;
    int is_connected;
    char name[8];
    int ID;
    int PID;
    int type;
    int deaths;
    int coins_carried;
    int coins_brought;
    int move;
} player;


int load_map(char *filepath, char dest[][MAX_WORLD_SIZE], point *size_res);

player init_player(int id);

void print_player_info(player pstruct);

void update_player(player *player, char map[][MAX_WORLD_SIZE], point world_size);

#endif // SERVER_H
