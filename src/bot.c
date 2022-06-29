#include "../lib/client_func.h"
#include "../lib/common.h"

enum keys { K_DOWN = 258,
    K_UP,
    K_LEFT,
    K_RIGHT };

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

    start_color();
    init_colors();

    ui interface;

    char map[MAX_WORLD_SIZE][MAX_WORLD_SIZE] = { 0 };

    if (make_folder_if_not_created(FIFO_LOCATION_FOLDER)) {
        return 3;
    }

    // Start communication
    char response_fifo_path[MAXLEN];
    char request_fifo_path[MAXLEN];

    // ---------------------- Server data (client read) ----------------------

    int server_response;
    int CLIENT_ID = -1;

    for (int i = 1; i < CLIENTS + 1; i++) {
        create_fifo_path(response_fifo_path, i, SERVER_DATA_FIFO_PATH);

        if (mkfifo(response_fifo_path, 0777) < 0)
            if (errno != EEXIST) {
                return 5;
            } else
                continue;

        CLIENT_ID = i;
        break;
    }

    if (CLIENT_ID < 0) {
        endwin();
        printf("There isn't a free player slot inside the server\n");
        return 6;
    }

    // Connect to FIFO for sending data to the server

    int client_request = -1; // FIFO descriptor

    create_fifo_path(request_fifo_path, CLIENT_ID, CLIENT_DATA_FIFO_PATH);

    // ---------------------- Client data (client wirte) ----------------------

    int tries = 1;
    int sleep_time = 3;

    while (tries <= 5) {
        // Try connecting to a FIFO

        client_request = open(request_fifo_path, O_WRONLY);
        if (client_request < 0) {
            printw(
                "[__CLIENT__]: Could not find server, retry %d of 5 in 3s...\n",
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
        printf("[__CLIENT__]: Could not connect to server, aborted after 5 "
               "tries\n");
        return 9;
    }

    server_response = open(response_fifo_path, O_RDONLY);
    if (server_response < 0) {
        printf("FIFO error at `%s`\n", response_fifo_path);
        return 7;
    }

    // ------------------- GAME LOOP -------------------

    pthread_t keyboard_input;

    key_info key_info;

    key_info.key = 0;

    if (pthread_mutex_init(&key_info.mutex, NULL) != 0) {
        return 4;
    }

    pthread_create(&keyboard_input, NULL, keyboard_input_func, &key_info);

    client_data data;
    server_data response;

    data.pid = getpid();
    data.bot = 1;
    int frame_counter = 0;

    info_client client_info;

    int key = rand() % 5;

    switch (key) {
    case 1:
        key = K_UP;
        break;
    case 2:
        key = K_DOWN;
        break;
    case 3:
        key = K_LEFT;
        break;
    case 4:
        key = K_RIGHT;
        break;
    }

    while (1) {
        
        int close_key = 0;

        pthread_mutex_lock(&key_info.mutex);
        close_key = key_info.key;
        key_info.key = 0;
        pthread_mutex_unlock(&key_info.mutex);

        if(close_key == 'q' || close_key == 'Q')
            break;

        // SEND

        data.key = key;

        int ret = 0;

        while (ret = write(client_request, &data, sizeof(client_data)) == -1 && errno == EINTR)
            continue;
        if (ret == -1)
            return close(client_request), perror("WRITE ERROR\n"), 9;

        // RECEIVE

        while (ret = read(server_response, &response, sizeof(server_data)) == -1 && errno == EINTR)
            continue;
        if (ret == -1)
            return close(client_request), perror("READ ERROR\n"), 9;

        if (frame_counter == 0)
            init_windows(&interface, response.world_size, 16, 50);

        interface.world_size = response.world_size;
        client_info.response = &response;

        print_info_client(interface.stat_window, &client_info);
        print_legend(interface.legend, 0, 1);
        update_windows_client(interface, response.map, response.player_position);

        wrefresh(interface.stat_window);
        wrefresh(interface.legend);
        wrefresh(interface.game_window);
        refresh();
        usleep(1000 * 100 * ROUND_TIME_IN_SEC10TH);;

        key = rand() % 5;

        switch (key) {
        case 1:
            key = K_UP;
            break;
        case 2:
            key = K_DOWN;
            break;
        case 3:
            key = K_LEFT;
            break;
        case 4:
            key = K_RIGHT;
            break;
        }

        // Check for enemy

        // Check vertical and horizontal lines

        int y_offset = response.player_position.y - CLIENT_FOV;
        int x_offset = response.player_position.x - CLIENT_FOV;

        const int map_half = CLIENT_MAP_SIZE / 2;

        point enemy_found = { -1, -1 };

        for (int j = 0; j < CLIENT_MAP_SIZE; j++) {
            if (response.map[y_offset + map_half][x_offset + j] == MAP_BEAST) {
                enemy_found.y = j;
                enemy_found.x = map_half;
                break;
            }
            if (response.map[y_offset + j][x_offset + map_half] == MAP_BEAST) {
                enemy_found.y = map_half;
                enemy_found.x = j;
                break;
            }
        }

        if (enemy_found.x != -1) {
            if (enemy_found.y < map_half && response.map[y_offset + map_half][x_offset + map_half + 1] != MAP_WALL) {
                key = K_RIGHT;
            }
            if (enemy_found.y > map_half && response.map[y_offset + map_half][x_offset + map_half - 1] != MAP_WALL) {
                key = K_LEFT;
            }
            if (enemy_found.x < map_half && response.map[y_offset + map_half + 1][x_offset + map_half] != MAP_WALL) {
                key = K_DOWN;
            }
            if (enemy_found.x > map_half && response.map[y_offset + map_half - 1][x_offset + map_half] != MAP_WALL) {
                key = K_UP;
            }
        }

        frame_counter++;
    }

    close(client_request);
    unlink(response_fifo_path);
    endwin();
    return 0;
}