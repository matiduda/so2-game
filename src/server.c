#include "../lib/common.h"
#include "../lib/connect.h"
#include "../lib/server_func.h"

int main(int argc, char** argv)
{
    srand(time(NULL));

    // Server constants
    char map[MAX_WORLD_SIZE][MAX_WORLD_SIZE] = { 0 };
    
    point world_size;
    int load = load_map(MAP_LOCATION, map, &world_size);

    if (load != 0) {
        switch (load) {
        case 1:
            printf("[__SERVER__]: map_load() error: Invalid argument passed\n");
            break;
        case 2:
            printf("[__SERVER__]: map_load() error: File i/o error\n");
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
        players[i] = init_player(i);
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

    // ------------------ GAME LOOP ------------------

    printw("Waiting for connection...\n");

    while (true) {

        pthread_mutex_lock(&key_info.mutex);
        closing_key = key_info.key;
        key_info.key = 0;
        pthread_mutex_unlock(&key_info.mutex);

        if (closing_key == 'q' || closing_key == 'Q') {

            for (int i = 0; i < CLIENTS; i++) {
                pthread_cancel(player_thr[i]);
            }

            break;
        }

        int has_connected_clients = 0;

        for (int i = 0; i < CLIENTS; i++) {

            if (players[i].is_connected) {
                has_connected_clients++;

                // Wait for data from connected client
                while(sem_wait(&players[i].received_data) != 0) {
                    if(errno != EINTR)
                        return perror("semaphore error, errno : "), printf("%d\n", errno), 10;
                }

                update_player(&players[i], map);
                copy_raw_map_data(map);

                print_info_server(interface.stat_window, &server_info);

                print_legend(interface.legend, 1, 1);
                update_windows_server(interface, map);

                server_info.round_number++;

                while(sem_post(&players[i].map_calculated) != 0) {
                    if(errno != EINTR)
                        return perror("semaphore error, errno : "), printf("%d\n", errno), 10;
                }
            }
        }

        sleep(1);
    }

    for (int i = 0; i < CLIENTS; i++) {
        pthread_join(player_thr[i], NULL);
        sem_destroy(&players[i].received_data);
    }

    pthread_cancel(keyboard_input);
    endwin();
    return 0;
}