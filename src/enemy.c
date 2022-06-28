#include "../lib/common.h"

int main(int argc, char* argv)
{
    srand(time(NULL));
    // Ignore 'pipe closed' signal so program continues
    signal(SIGPIPE, SIG_IGN);

    initscr();
    noecho();
    raw();
    keypad(stdscr, TRUE);
    curs_set(0);

    refresh();
    start_color();
    init_colors();

    WINDOW* beast_view[ENEMY_MAX_ENEMIES];

    if (make_folder_if_not_created(FIFO_LOCATION_FOLDER)) {
        return 3;
    }

    // Start communication
    char response_fifo_path[MAXLEN];
    char request_fifo_path[MAXLEN];

    // ---------------------- Server data (client read) ----------------------

    int server_response;

    create_fifo_path(response_fifo_path, 0, SERVER_DATA_FIFO_PATH);

    if (mkfifo(response_fifo_path, 0777) < 0)
        if (errno != EEXIST) {
            printf("Could not open FIFO for communication\n");
            return 5;
        }

    // Connect to FIFO for sending data to the server

    int client_request = -1; // FIFO descriptor

    create_fifo_path(request_fifo_path, 0, CLIENT_DATA_FIFO_PATH);

    // ---------------------- Client data (client wirte) ----------------------

    int tries = 1;
    int sleep_time = 3;

    while (tries <= 5) {
        // Try connecting to a FIFO

        client_request = open(request_fifo_path, O_WRONLY);
        if (client_request < 0) {
            printw(
                "[__BEAST__]: Could not find server, retry %d of 5 in 3s...\n",
                tries++);
            refresh();
        } else {
            break;
        }

        sleep(sleep_time);
    }

    if (tries > 5) {
        unlink(response_fifo_path);
        endwin();
        printf("[__BEAST__]: Could not connect to server, aborted after 5 "
               "tries\n");
        return 9;
    }

    server_response = open(response_fifo_path, O_RDONLY);
    if (server_response < 0) {
        printf("FIFO error at `%s`\n", response_fifo_path);
        return 7;
    }

    // Initialize windows
    const int IN_ROW = 5;

    const int X_SPACING = 5;
    const int Y_SPACING = 4;

    for (int i = 0; i < ENEMY_MAX_ENEMIES; i++) {
        beast_view[i] = newwin(ENEMY_MAP_SIZE + 3, ENEMY_MAP_SIZE + 2, i / IN_ROW * (ENEMY_MAP_SIZE + Y_SPACING), i % IN_ROW * (ENEMY_MAP_SIZE + X_SPACING));
        box(beast_view[i], 0, 0);
        wrefresh(beast_view[i]);
    }

    clear();
    refresh();

    printw("Connected. Waiting for enemy spawn request!");

    // ------------------- GAME LOOP -------------------

    pthread_t keyboard_input;

    key_info key_info;

    key_info.key = 0;

    if (pthread_mutex_init(&key_info.mutex, NULL) != 0) {
        return 4;
    }

    pthread_create(&keyboard_input, NULL, keyboard_input_func, &key_info);

    struct enemy_request response = { 0 };
    struct enemy_response request = { 0 };

    request.client_pid = getpid();

    int key = 0;

    point enemy_found[ENEMY_MAX_ENEMIES] = {0};

    while (1) {

        pthread_mutex_lock(&key_info.mutex);
        key = key_info.key;
        key_info.key = 0;
        pthread_mutex_unlock(&key_info.mutex);

        if (key == 'q' || key == 'Q')
            break;

        // RECEIVE
        int ret = 0;

        while (ret = read(server_response, &response, sizeof(struct enemy_request)) == -1 && errno == EINTR)
            continue;
        if (ret == -1)
            return close(client_request), perror("READ ERROR\n"), 9;

        const int map_half = ENEMY_MAP_SIZE / 2;

        for (int i = 0; i < response.active_enemies; i++) {

            int valid_move = 0;
            while (!valid_move) {

                point dpos = { 0 };
                request.direction[i] = rand() % 5;
                switch (request.direction[i]) {
                case UP:
                    dpos.y--;
                    break;
                case DOWN:
                    dpos.y++;
                    break;
                case LEFT:
                    dpos.x--;
                    break;
                case RIGHT:
                    dpos.x++;
                    break;
                }
                if (response.maps[i].map[map_half + dpos.y][map_half + dpos.x] == MAP_EMPTY)
                    break;
            }

            enemy_found[i].y = -1;
            enemy_found[i].x = -1;

            // Check vertical and horizontal lines
            for (int j = 0; j < ENEMY_MAP_SIZE; j++) {
                if (response.maps[i].map[map_half][j] == '1' || response.maps[i].map[map_half][j] == '2' || response.maps[i].map[map_half][j] == '3' || response.maps[i].map[map_half][j] == '4') {
                    enemy_found[i].y = j;
                    enemy_found[i].x = map_half;
                    break;
                }
                if (response.maps[i].map[j][map_half] == '1' || response.maps[i].map[j][map_half] == '2' || response.maps[i].map[j][map_half] == '3' || response.maps[i].map[j][map_half] == '4') {
                    enemy_found[i].y = map_half;
                    enemy_found[i].x = j;
                    break;
                }
            }

            if (enemy_found[i].x != -1) {
                if (enemy_found[i].y < map_half && response.maps[i].map[map_half][map_half - 1] != MAP_WALL) {
                    request.direction[i] = LEFT;
                    continue;
                }
                if (enemy_found[i].y > map_half && response.maps[i].map[map_half][map_half + 1] != MAP_WALL) {
                    request.direction[i] = RIGHT;
                    continue;
                }
                if (enemy_found[i].x < map_half && response.maps[i].map[map_half - 1][map_half] != MAP_WALL) {
                    request.direction[i] = UP;
                    continue;
                }
                if (enemy_found[i].x > map_half && response.maps[i].map[map_half + 1][map_half] != MAP_WALL) {
                    request.direction[i] = DOWN;
                    continue;
                }
            }
        }

        // DRAW

        werase(stdscr);
        refresh();

        for (int w = 0; w < response.active_enemies; w++) {
            response.maps[w].map[ENEMY_MAP_SIZE / 2][ENEMY_MAP_SIZE / 2] = MAP_BEAST;

            for (int i = 0; i < ENEMY_MAP_SIZE; i++) {
                for (int j = 0; j < ENEMY_MAP_SIZE; j++) {

                    wattron(beast_view[w], COLOR_PAIR(DEFAULT));

                    char c = response.maps[w].map[i][j];

                    if (c == '1' || c == '2' || c == '3' || c == '4') {
                    } else {
                        switch (c) {
                        case MAP_WALL:
                            wattron(beast_view[w], COLOR_PAIR(WALL));
                            c = MAP_EMPTY;
                            break;
                        case MAP_BEAST:
                            wattron(beast_view[w], COLOR_PAIR(ENEMY));
                            break;
                        case MAP_CAMPSITE:
                            wattron(beast_view[w], COLOR_PAIR(CAMPSITE));
                            break;
                        case MAP_BUSHES:
                            wattron(beast_view[w], COLOR_PAIR(BUSHES));
                            break;
                        case MAP_COIN_1:
                            wattron(beast_view[w], COLOR_PAIR(COIN));
                            break;
                        case MAP_COIN_10:
                            wattron(beast_view[w], COLOR_PAIR(COIN));
                            break;
                        case MAP_COIN_50:
                            wattron(beast_view[w], COLOR_PAIR(COIN));
                            break;
                        case MAP_COIN_DROPPED:
                            wattron(beast_view[w], COLOR_PAIR(COIN));
                            break;
                        }
                    }

                    if(i == enemy_found[w].x && j == enemy_found[w].y)
                        wattron(beast_view[w], COLOR_PAIR(CAMPSITE));

                    mvwprintw(beast_view[w], i + 2, j + 1, "%c", c);
                    wattron(beast_view[w], COLOR_PAIR(DEFAULT));
                }
            }

            mvprintw(w / IN_ROW * (ENEMY_MAP_SIZE + Y_SPACING) + 1, w % IN_ROW * (ENEMY_MAP_SIZE + X_SPACING), "%02d / %02d", response.positions[w].x, response.positions[w].y);

            box(beast_view[w], 0, 0);
            wrefresh(beast_view[w]);
        }

        refresh();
    
        // SEND

        while (ret = write(client_request, &request, sizeof(struct enemy_response)) == -1 && errno == EINTR)
            continue;
        if (ret == -1)
            return close(client_request), perror("WRITE ERROR\n"), 9;

    }

    close(client_request);
    unlink(response_fifo_path);
    endwin();
    return 0;
}