#include "../lib/common.h"
#include "../lib/connect.h"
#include "../lib/server_func.h"

int main(int argc, char** argv)
{
#if LOGGING
    char logmsg[1024];
    FILE* logfile = configure_logging(SERVER_LOG_FILE);
    if (!logfile) {
        return printw("[__SERVER__]: Could not open log file\n"), 1;
    }
    if (log_this(logfile, "[__SERVER__]: Logging mode enabled\n"))
        return printw("[__SERVER__]: Logging error\n"), 2;

    int frame_counter = 0;
#endif

    // Server constants
    int wait_tenth_of_second = 10; // Time to wait for user input
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

#if LOGGING
    if (log_this(logfile, "[__SERVER__]: Loaded map from file\n"))
        return printw("[__SERVER__]: Logging error\n"), 2;
#endif

    initscr();
    start_color();
    noecho();
    raw();
    keypad(stdscr, TRUE);
    
    refresh();

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);


    ui interface;
    init_windows(&interface, world_size);

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
#if LOGGING
        if (log_this(logfile, "[__SERVER__]: Mutex initialization error\n"))
            return printw("[__SERVER__]: Logging error\n"), 1;
#endif
        return 4;
    }

    char closing_key = 0;

    // ------------------ GAME LOOP ------------------

    while (true) {

        int has_connected_clients = 0;

        for (int i = 0; i < CLIENTS; i++) {

            if (players[i].is_connected) {
                has_connected_clients++;

#if LOGGING
                sprintf(logmsg, "[__SERVER__]: ------------------------- frame %3d -------------------------\n", frame_counter++);
                if (log_this(logfile, logmsg))
                    return printw("[__SERVER__]: Logging error\n"), 1;
                if (log_this(logfile, "[__SERVER__]: Waiting for data...\n"))
                    return printw("[__SERVER__]: Logging error\n"), 1;
#endif

                // Wait for data from connected client
                if (sem_wait(&players[i].received_data) == -1) {
#if LOGGING
                    if (log_this(logfile, "[__SERVER__]: Semaphore error\n"))
                        return printw("[__SERVER__]: Logging error\n"), 1;
#endif
                    return perror("semaphore error"), -1;
                }

#if LOGGING
                sprintf(logmsg, "[__SERVER__]: RECEIVED DATA FROM %d clients!\n",
                    has_connected_clients);
                if (log_this(logfile, logmsg))
                    return printw("[__SERVER__]: Logging error\n"), 1;
#endif

                if (sem_post(&players[i].received_data) == -1) {
#if LOGGING
                    if (log_this(logfile, "[__SERVER__]: Semaphore error\n"))
                        return printw("[__SERVER__]: Logging error\n"), 1;
#endif
                    return perror("semaphore error"), -1;
                }
            }
            // break;
        }

        pthread_mutex_lock(&key_info.mutex);
        closing_key = key_info.key;
        key_info.key = 0;
        pthread_mutex_unlock(&key_info.mutex);

        if (closing_key == 't' || closing_key == 'T') {

            for (int i = 0; i < CLIENTS; i++) {
                pthread_cancel(player_thr[i]);
            }

            // TODO del
            endwin();
            return 0;

            break;
        }

        update_windows(interface, map);
        refresh();

        sleep(1);
    }

    for (int i = 0; i < CLIENTS; i++) {
        pthread_join(player_thr[i], NULL);
        sem_destroy(&players[i].received_data);
    }


    pthread_cancel(keyboard_input);
    endwin();

#if LOGGING

    fclose(logfile);
#endif

    return 0;
}