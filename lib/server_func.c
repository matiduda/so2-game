#include "server_func.h"

char MAP_ORIGINAL[MAX_WORLD_SIZE][MAX_WORLD_SIZE] = { 0 };
point MAP_SIZE = { 0, 0 };

void update_windows_server(ui interface, char dest[][MAX_WORLD_SIZE])
{

    WINDOW* game_window = interface.game_window;
    WINDOW* stat_window = interface.stat_window;
    WINDOW* legend = interface.legend;

    int world_height = interface.world_size.y;
    int world_width = interface.world_size.x;

    for (int i = 0; i < world_height; i++) {

        for (int j = 0; j < world_width; j++) {
            char c = dest[i][j];

            if (c == '1' || c == '2' || c == '3' || c == '4') {
                wattron(game_window, COLOR_PAIR(PLAYER));
            } else {
                switch (c) {
                case MAP_WALL:
                    wattron(game_window, COLOR_PAIR(WALL));
                    c = MAP_EMPTY;
                    break;
                case MAP_BEAST:
                    wattron(game_window, COLOR_PAIR(ENEMY));
                    break;
                case MAP_CAMPSITE:
                    wattron(game_window, COLOR_PAIR(CAMPSITE));
                    break;
                case MAP_BUSHES:
                    wattron(game_window, COLOR_PAIR(BUSHES));
                    break;
                case MAP_COIN_1:
                    wattron(game_window, COLOR_PAIR(COIN));
                    break;
                case MAP_COIN_10:
                    wattron(game_window, COLOR_PAIR(COIN));
                    break;
                case MAP_COIN_50:
                    wattron(game_window, COLOR_PAIR(COIN));
                    break;
                case MAP_COIN_DROPPED:
                    wattron(game_window, COLOR_PAIR(COIN));
                    break;
                default:
                    wattron(game_window, COLOR_PAIR(DEFAULT));
                    break;
                }
            }

            mvwprintw(game_window, i + 1, j + 1, "%c", c);
        }
    }

    wattron(game_window, COLOR_PAIR(DEFAULT));

    box(game_window, 0, 0);

    wrefresh(stat_window);
    wrefresh(legend);
    wrefresh(game_window);
}

void print_info_server(WINDOW* w, info_server* info)
{
    if (!w || !info)
        return;

    werase(w);

    const int Y = 1;
    const int X = 1;

    mvwprintw(w, Y + 0, X + 0, "Server's PID: %ld", (long)info->server_PID);
    mvwprintw(w, Y + 2, X + 0, "Campsite X/Y: %02d/%02d", info->campsite_xy.x, info->campsite_xy.y);
    mvwprintw(w, Y + 3, X + 0, "Round number: %d", info->round_number);
    mvwprintw(w, Y + 5, X + 0, "Parameter:");
    mvwprintw(w, Y + 6, X + 0, "PID");
    mvwprintw(w, Y + 7, X + 0, "Type");
    mvwprintw(w, Y + 8, X + 0, "Curr X/Y");
    mvwprintw(w, Y + 9, X + 0, "Deaths");
    mvwprintw(w, Y + 11, X + 0, "Coins");
    mvwprintw(w, Y + 12, X + 0, "carried");
    mvwprintw(w, Y + 13, X + 0, "brought");

    for (int i = 0; i < info->player_count; i++) {
        int xoff = 13 + i * 9;

        if (info->players[i].is_connected) {
            mvwprintw(w, Y + 5, X + xoff, "%s", info->players[i].name);
            mvwprintw(w, Y + 6, X + xoff, "%ld", (long)info->players[i].PID);
            if (info->players[i].type == 0)
                mvwprintw(w, Y + 7, X + xoff, "HUMAN");
            else if (info->players[i].type == 1)
                mvwprintw(w, Y + 7, X + xoff, "CPU");
            mvwprintw(w, Y + 8, X + xoff, "%02d/%02d", info->players[i].pos.x, info->players[i].pos.y);
            mvwprintw(w, Y + 9, X + xoff, "%d", info->players[i].deaths);

            mvwprintw(w, Y + 12, X + xoff, "%d", info->players[i].coins_found);
            mvwprintw(w, Y + 13, X + xoff, "%d", info->players[i].coins_brought);
        } else {
            mvwprintw(w, Y + 5, X + xoff, "%s", info->players[i].name);
            mvwprintw(w, Y + 6, X + xoff, "-");
            mvwprintw(w, Y + 7, X + xoff, "-");
            mvwprintw(w, Y + 8, X + xoff, "--/--");
            mvwprintw(w, Y + 9, X + xoff, "-");

            mvwprintw(w, Y + 12, X + xoff, "-");
            mvwprintw(w, Y + 13, X + xoff, "-");
        }
    }
}

// ------------------------------------------------------

int load_map(char* filepath, char dest[][MAX_WORLD_SIZE], point* size_res, point* campsite)
{
    // Function loads game map from a text file.

    // The function detects the world size using
    // provided map and saves it under *size_res

    // If an error occures, the functinon returns
    // positive integer.

    // 0  - map loaded correctly
    // 1 - invalid argument passed
    // 2 - file i/o error
    // 3 - could not find campsite in map
    // > 3 - invalid character detected

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
    int found_campsite = 0;

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
                    if (!found_campsite && buffer == MAP_CAMPSITE) {
                        campsite->x = j;
                        campsite->y = i;
                        found_campsite++;
                    }
                    break;
                }
            }
            if (!ok)
                return fclose(f), buffer;

            dest[i][j] = buffer;
            MAP_ORIGINAL[i][j] = buffer;
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

    fclose(f);

    size_res->x = size_x;
    size_res->y = size_y + 1;

    MAP_SIZE.x = size_x;
    MAP_SIZE.y = size_y + 1;

    if (!found_campsite)
        return 3;

    return 0;
}

player init_player(int id, point campsite)
{
    player p = { 0 };

    sprintf(p.name, "Player%d", id);

    sem_init(&p.received_data, 0, 0);
    sem_init(&p.map_calculated, 0, 0);

    p.ID = ++id;

    p.world_size.y = MAP_SIZE.y;
    p.world_size.x = MAP_SIZE.x;

    p.campsite_xy = campsite;
    p.server_PID = getpid();

    randomize_player_spawn(&p);

    return p;
}

void randomize_player_spawn(player* p)
{
    if (!p)
        return;

    point spawn;

    while (true) {
        spawn.x = rand() % (MAP_SIZE.x + 1);
        spawn.y = rand() % (MAP_SIZE.y + 1);

        if (MAP_ORIGINAL[spawn.y][spawn.x] == MAP_EMPTY)
            break;
    }

    p->spawn_location = spawn;
    p->pos = spawn;
}

void calculate_player(player* player, char map[][MAX_WORLD_SIZE])
{
    if (!player)
        return;

    point d_pos = { 0, 0 };

    switch (player->move) {
    case 259: // UP:
        d_pos.y--;
        break;
    case 258: // DOWN
        d_pos.y++;
        break;
    case 260: // LEFT
        d_pos.x--;
        break;
    case 261: // RIGHT
        d_pos.x++;
        break;
    }

    enum player_action action = get_action(map[player->pos.y + d_pos.y][player->pos.x + d_pos.x]);

    switch (action) {
    case IN_WALL: {
        d_pos.x = 0;
        d_pos.y = 0;
        break;
    }
    case IN_BUSHES: {
        if (player->bush_timer == 0) {
            player->bush_timer = 1;
            d_pos.x = 0;
            d_pos.y = 0;
            break;
        }
        player->bush_timer = 0;
        break;
    }
    case GETS_TREASURE: {
        switch (map[player->pos.y + d_pos.y][player->pos.x + d_pos.x]) {
        case MAP_COIN_1:
            player->coins_found += 1;
            break;
        case MAP_COIN_10:
            player->coins_found += 10;
            break;
        case MAP_COIN_50:
            player->coins_found += 50;
            break;
        case MAP_COIN_DROPPED:
            // TODO: Lookup for custom treasure
            player->coins_found = 99;
            break;
        }

        map[player->pos.y + d_pos.y][player->pos.x + d_pos.x] = MAP_EMPTY;
        MAP_ORIGINAL[player->pos.y + d_pos.y][player->pos.x + d_pos.x] = MAP_EMPTY;
        break;
    }
    case IS_DEAD: {
        player->deaths++;
        d_pos.x = 0;
        d_pos.y = 0;

        player->pos.x = player->spawn_location.x;
        player->pos.y = player->spawn_location.y;
        break;
    }
    case IN_CAMPSITE: {
        player->coins_brought += player->coins_found;
        player->coins_found = 0;
        break;
    }
    }

    player->pos.x += d_pos.x;
    player->pos.y += d_pos.y;
}

void update_player(player* player, char map[][MAX_WORLD_SIZE], int round_number)
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
   
    if (!player)
        return;

    player->round_number = round_number;

    // Draw player on map
    map[player->pos.y][player->pos.x] = player->ID + '0';

    const int PLAYER_MAP_SIZE = CLIENT_MAP_SIZE;
    int x = player->pos.x - CLIENT_FOV;
    int y = player->pos.y - CLIENT_FOV;

    for (int i = 0; i < PLAYER_MAP_SIZE; i++) {
        for (int j = 0; j < PLAYER_MAP_SIZE; j++) {

            // Check for out of bounds
            if (i + y < 0 || i + y > MAP_SIZE.y || j + x < 0 || j + x > MAP_SIZE.x)
                player->map[i][j] = '-';
            else
                player->map[i][j] = map[i + y][j + x];
        }
    }
}

void copy_raw_map_data(char map[][MAX_WORLD_SIZE])
{
    for (int i = 0; i < MAP_SIZE.y; i++)
        for (int j = 0; j < MAP_SIZE.x; j++)
            map[i][j] = MAP_ORIGINAL[i][j];
}
void draw_player(player* player, char map[][MAX_WORLD_SIZE])
{
    map[player->pos.y][player->pos.x] = player->ID + '0';
}

enum player_action get_action(char new_location)
{
    if (new_location == MAP_EMPTY)
        return NONE;

    if (new_location == MAP_WALL)
        return IN_WALL;

    if (new_location == MAP_COIN_1 || new_location == MAP_COIN_10 || new_location == MAP_COIN_50 || new_location == MAP_COIN_DROPPED)
        return GETS_TREASURE;

    if (new_location == MAP_BEAST || new_location == '1' || new_location == '2' || new_location == '3' || new_location == '4')
        return IS_DEAD;

    if (new_location == MAP_BUSHES)
        return IN_BUSHES;

    if (new_location == MAP_CAMPSITE)
        return IN_CAMPSITE;

    return NONE;
}

// ---
