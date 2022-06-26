#ifndef SERVER_H
#define SERVER_H

#include "common.h"

#define ROUND_TIME_IN_SECONDS 5
#define MAX_DROPPED_T 10
#define MAP_LOCATION "map.txt"
#define PLAYER_COUNT 4

// Data specific to the server

enum player_action { NONE,
    IN_WALL,
    IN_BUSHES,
    GETS_TREASURE,
    HITS_ENEMY,
    IN_CAMPSITE };

typedef struct player_info {
    sem_t received_data;
    sem_t map_calculated;
    char map[MAX_WORLD_SIZE][MAX_WORLD_SIZE];
    char name[8];
    point world_size;
    point spawn_location;
    point pos;
    point campsite_xy;
    int is_connected;
    int round_number;
    int bush_timer;
    int ID;
    pid_t PID;
    pid_t server_PID;
    int type;
    int deaths;
    int coins_found;
    int coins_brought;
    int move;
} player;

typedef struct game_info_server {
    player* players;
    point campsite_xy;
    pid_t server_PID;
    int player_count;
    int round_number;
} info_server;

typedef struct dropped_treasure {
    point position;
    int value;
} treasure;

void update_windows_server(ui interface, char dest[][MAX_WORLD_SIZE]);
void print_info_server(WINDOW* w, info_server* info);

int load_map(char* filepath, char dest[][MAX_WORLD_SIZE], point* size_res, point* campsite);
void copy_raw_map_data(char map[][MAX_WORLD_SIZE]);
void draw_player(player* player, char map[][MAX_WORLD_SIZE]);
void coin_spawn(char type);

void calculate_treasures(player *players);
void save_treasure(point p, int value);
int get_treasure(point p);

player init_player(int id, point campsite);
void randomize_player_spawn(player* p);
void calculate_player(player* player, char map[][MAX_WORLD_SIZE]);
void update_player(player* player, char map[][MAX_WORLD_SIZE], int round_number);
enum player_action get_action(char new_location);

#endif // SERVER_H
