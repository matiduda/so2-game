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
    char name[8];
    point world_size;
    point spawn_location;
    point pos;
    int is_connected;
    int round_number;
    int ID;
    int PID;
    int type;
    int deaths;
    int coins_carried;
    int coins_brought;
    int move;
} player;

typedef struct game_info_server {
    player *players;
    point campsite_xy;
    int server_PID;
    int player_count;
    int round_number;
} info_server;

void update_windows_server(ui interface, char dest[][MAX_WORLD_SIZE]);
void print_info_server(WINDOW *w, info_server *info);

int load_map(char *filepath, char dest[][MAX_WORLD_SIZE], point *size_res);

player init_player(int id);
void randomize_player_spawn(player *p);
void update_player(player *player, char map[][MAX_WORLD_SIZE]);

#endif // SERVER_H
