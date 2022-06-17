#ifndef SERVER_H
#define SERVER_H

#include "common.h"

#define ROUND_TIME_IN_SECONDS 5

#define MAP_LOCATION "map.txt"

// Data specific to the server

enum player_action { NONE,
    IN_WALL,
    IN_BUSHES,
    GETS_TREASURE,
    IS_DEAD,
    IN_CAMPSITE };

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
    int bush_timer;
    int ID;
    int PID;
    int type;
    int deaths;
    int coins_found;
    int coins_brought;
    int move;
} player;

typedef struct game_info_server {
    player* players;
    point campsite_xy;
    int server_PID;
    int player_count;
    int round_number;
} info_server;

void update_windows_server(ui interface, char dest[][MAX_WORLD_SIZE]);
void print_info_server(WINDOW* w, info_server* info);

int load_map(char* filepath, char dest[][MAX_WORLD_SIZE], point* size_res);
void copy_raw_map_data(char map[][MAX_WORLD_SIZE]);

player init_player(int id);
void randomize_player_spawn(player* p);
void update_player(player* player, char map[][MAX_WORLD_SIZE]);
enum player_action get_action(char new_location);

#endif // SERVER_H
