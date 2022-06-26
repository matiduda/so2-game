#include "../lib/common.h"
#include "../lib/connect.h"
#include "../lib/server_func.h"

int main(int argc, char** argv)
{
    srand(time(NULL));

    // Server constants
    char map[MAX_WORLD_SIZE][MAX_WORLD_SIZE] = { 0 };

    point world_size;
    point campsite_xy = { 0 };
    int load = load_map(MAP_LOCATION, map, &world_size, &campsite_xy);

    if (load != 0) {
        switch (load) {
        case 1:
            printf("[__SERVER__]: map_load() error: Invalid argument passed\n");
            break;
        case 2:
            printf("[__SERVER__]: map_load() error: File i/o error\n");
            break;
        case 3:
            printf("[__SERVER__]: map_load() error: Could not locate campsite `%c`\n", MAP_CAMPSITE);
            break;
        default:
            printf("[__SERVER__]: map_load() error: Invalid character detected: `%c`\n", load);
            break;
        }
        return load;
    }

    // Initialize GUI
    initscr();
    noecho();
    raw();
    keypad(stdscr, TRUE);
    curs_set(0);

    start_color();
    init_colors();

    ui interface;
    init_windows(&interface, world_size, 16, 50);

    // Ignore 'pipe closed' signal so program continues
    sigaction(SIGPIPE, &(struct sigaction) { SIG_IGN }, NULL);

    // Disable printw bufering
    setvbuf(stdout, NULL, _IONBF, 0);

    if (make_folder_if_not_created(FIFO_LOCATION_FOLDER)) {
        perror("[make_folder_if_not_created()]: Could not create folder\n");
        return 1;
    }

    player players[4] = { 0 };
    pthread_t player_thr[4];

    for (int i = 0; i < CLIENTS; i++) {
        players[i] = init_player(i, campsite_xy);
        pthread_create(&player_thr[i], NULL, player_connection, &players[i]);
    }

    pthread_t keyboard_input;
    key_info key_info;
    key_info.key = 0;
    pthread_create(&keyboard_input, NULL, keyboard_input_func, &key_info);

    if (pthread_mutex_init(&key_info.mutex, NULL) != 0) {
        return 4;
    }

    char closing_key = 0;

    info_server server_info;
    server_info.server_PID = getpid();
    server_info.player_count = 4;
    server_info.players = players;
    server_info.round_number = 0;
    server_info.campsite_xy.x = campsite_xy.x;
    server_info.campsite_xy.y = campsite_xy.y;

    // ------------------ GAME LOOP ------------------

    while (true) {

        pthread_mutex_lock(&key_info.mutex);
        closing_key = key_info.key;
        key_info.key = 0;
        pthread_mutex_unlock(&key_info.mutex);


        if (closing_key == 'q' || closing_key == 'Q') {
            break;
        }

        if (closing_key == 'c' || closing_key == 't' || closing_key == 'T') {
            coin_spawn(closing_key);
        }

        if (closing_key == 'b' || closing_key == 'B') {
            // Spawn new enemy
        }

        copy_raw_map_data(map);
        
        for (int i = 0; i < CLIENTS; i++) { // Calculate player positions
 
            if (players[i].is_connected) {

                // Wait for data from connected client
                while (sem_wait(&players[i].received_data) != 0) {
                    if (errno != EINTR)
                        return perror("semaphore error, errno : "), printf("%d\n", errno), 10;
                }
                
                calculate_player(&players[i], map);
                draw_player(&players[i], map);

            }
        }

        calculate_treasures(players);

        for (int i = 0; i < CLIENTS; i++) { // Update player views

            if (players[i].is_connected) {

                update_player(&players[i], map, server_info.round_number);

                while (sem_post(&players[i].map_calculated) != 0) {
                    if (errno != EINTR)
                        return perror("semaphore error, errno : "), printf("%d\n", errno), 10;
                }
            }
        }

        server_info.round_number++;

        print_info_server(interface.stat_window, &server_info);
        print_legend(interface.legend, 0, 1);
        update_windows_server(interface, map);

        sleep(1);
    }

    for (int i = 0; i < CLIENTS; i++) {
        pthread_cancel(player_thr[i]);
        pthread_join(player_thr[i], NULL);
        sem_destroy(&players[i].received_data);
    }

    pthread_cancel(keyboard_input);
    endwin();
    return 0;
}