#include "../lib/client_func.h"
#include "../lib/common.h"
#include "../lib/connect.h"

// TODO: Clean this mess

int main(int argc, char* argv)
{

    // Client constants
    int wait_tenth_of_second = 10; // Time to wait for user input

    initscr();
    noecho();
    raw();
    keypad(stdscr, TRUE);

    refresh();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);

    ui interface;
    point temp_world = { CLIENT_MAP_SIZE + 2, CLIENT_MAP_SIZE + 2 };
    init_windows(&interface, temp_world);

    char map[MAX_WORLD_SIZE][MAX_WORLD_SIZE] = { 0 };

    // Ignore 'pipe closed' signal so program continues
    sigaction(SIGPIPE, &(struct sigaction) { SIG_IGN }, NULL);

    if (make_folder_if_not_created(FIFO_LOCATION_FOLDER)) {
        return 3;
    }

    // Start communication
    char response_fifo_path[MAXLEN];
    char request_fifo_path[MAXLEN];

    pthread_t keyboard_input;

    key_info key_info;

    key_info.key = 0;

    if (pthread_mutex_init(&key_info.mutex, NULL) != 0) {
        return 4;
    }

    pthread_create(&keyboard_input, NULL, keyboard_input_func, &key_info);

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
            printf(
                "[__CLIENT__]: Could not find server, retry %d of 5 in 3s...\n",
                tries++);
        } else {

            break;
        }
        sleep(sleep_time);
    }

    if (tries > 5) {
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

    printf("connected!\n");

    client_data data;
    data.pid = getpid();

    int frame_counter = 0;

    while (data.key != 'q' && data.key != 'Q') {
        data.key = ' ';

        pthread_mutex_lock(&key_info.mutex);
        data.key = key_info.key;
        key_info.key = 0;
        pthread_mutex_unlock(&key_info.mutex);

        // SEND

        int w = write(client_request, &data, sizeof(client_data));
        if (w == -1) {
            // FIFO Write error
            close(client_request);
            return 8;
        }

        // RECEIVE

        server_data response;

        int r = read(server_response, &response, sizeof(server_data));
        if (r == -1) {
            // FIFO Reading error
            close(client_request);
            return 9;
        }

        interface.world_size = response.world_size;
        update_windows(interface, response.map);
        refresh();
        usleep(1000 * 100 * wait_tenth_of_second);
    }

    close(client_request);
    unlink(response_fifo_path);

    pthread_join(keyboard_input, NULL);

    endwin();

    return 0;
}