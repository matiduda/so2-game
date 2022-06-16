// #include <stdio.h>
#include "server_func.h"
#include <stdlib.h>
#include <string.h>

int load_map(char* filepath, char dest[][MAX_WORLD_SIZE], point* size_res)
{

    // Function loads game map from a text file.

    // The function detects the world size using
    // provided map and saves it under *size_res

    // If an error occures, the functinon returns
    // positive integer.

    // 0  - map loaded correctly
    // 1 - invalid argument passed
    // 2 - file i/o error
    // > 2 - invalid character detected

    const char valid_symb[] = {
        MAP_EMPTY,
        MAP_WALL,
        MAP_BUSHES,
        MAP_BEAST,
        MAP_CAMPSITE,
        MAP_COIN_DROPPED,
        MAP_COIN_50,
        MAP_COIN_10,
        MAP_COIN_1,
    };

    const int valid_s = sizeof(valid_symb);

    if (!filepath)
        return 1;

    FILE* f = fopen(filepath, "rt");
    if (!f)
        return 2;

    // Detect world size
    int size_x = MAX_WORLD_SIZE;
    int size_y = MAX_WORLD_SIZE;
    char buffer = 0;
    int read = 0;

    for (int i = 0; i < MAX_WORLD_SIZE; i++) {
        read = fread(&buffer, sizeof(char), 1, f);
        if (read != 1)
            return fclose(f), 2;

        if (buffer == '\n') {
            size_x = i;
            break;
        }
    }

    rewind(f);

    // Read map data

    for (int i = 0; i < MAX_WORLD_SIZE; i++) {
        for (int j = 0; j < size_x; j++) {

            read = fread(&buffer, sizeof(char), 1, f);
            if (read != 1)
                return fclose(f), 2;
            // Check if the character is valid
            int ok = 0;
            for (int v = 0; v < valid_s; v++) {
                if (buffer == valid_symb[v]) {
                    ok++;
                    break;
                }
            }
            if (!ok)
                return fclose(f), buffer;

            dest[i][j] = buffer;
        }

        // Read newline character

        read = fread(&buffer, sizeof(char), 1, f);

        if (feof(f)) {
            size_y = i;
            break;
        }

        if (read != 1)
            return fclose(f), 2;
        if (buffer != '\n') {
            return fclose(f), buffer;
        }
    }

    size_res->x = size_x;
    size_res->y = size_y + 1;

    return fclose(f), 0;
}

player init_player(int id)
{

    player p = { 0 };

    sprintf(p.name, "Player%d", id);

    sem_init(&p.received_data, 0, 0);
    sem_init(&p.map_calculated, 0, 0);

    p.ID = ++id;

    p.pos.x = 1;
    p.pos.y = 1;

    p.world_size.y = CLIENT_MAP_SIZE;
    p.world_size.x = CLIENT_MAP_SIZE;

    return p;
}

void update_player(player* player, char map[][MAX_WORLD_SIZE], point world_size)
{
    /*
    Player map

    ///////
    /  █* /
    /█ ███/
    /█ 3 █/
    /███ █/
    /█T█  /
    ///////
    */

    if (!player || !map)
        return;

    switch (player->move) {
    case 259: // UP:
        player->pos.y--;
        break;
    case 258: // DOWN
        player->pos.y++;
        break;
    case 260: // LEFT
        player->pos.x--;
        break;
    case 261: // RIGHT
        player->pos.x++;
        break;
    default:
        break;
    }

    const int PLAYER_MAP_SIZE = CLIENT_MAP_SIZE;
    int x = player->pos.x - CLIENT_FOV;
    int y = player->pos.y - CLIENT_FOV;

    for (int i = 0; i < PLAYER_MAP_SIZE; i++) {
        for (int j = 0; j < PLAYER_MAP_SIZE; j++) {

            // Check for out of bounds
            if (i + y < 0 || i + y > world_size.y || j + x < 0 || j + x > world_size.x)
                player->map[i][j] = '-';
            else
                player->map[i][j] = map[i + y][j + x];
        }
    }
}