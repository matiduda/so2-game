#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"

// Data specific to the client

typedef struct game_info_client {
    int client_PID;
    point campsite_xy;
    server_data *response;
} info_client;

void update_windows_client(ui interface, char dest[][MAX_WORLD_SIZE], point map_center);
void print_info_client(WINDOW *w, info_client *info);

#endif // CLIENT_H
