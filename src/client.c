#include "../lib/client_func.h"
#include "../lib/common.h"
#include "../lib/connect.h"

// TODO: Clean this mess

int main(int argc, char* argv)
{
#if LOGGING
    char logmsg[1024];
    FILE* logfile = configure_logging(CLIENT_LOG_FILE);
    if (!logfile) {
        return printf("[__CLIENT__]: Could not open log file\n"), 1;
    }
    if (log_this(logfile, "[__CLIENT__]: Logging mode enabled\n"))
        return printf("[__CLIENT__]: Logging error\n"), 2;
#endif

    // Client constants
    int wait_tenth_of_second = 10; // Time to wait for user input

    initscr();
    start_color();
    noecho();
    raw();
    keypad(stdscr, TRUE);

    ui interface;
    point temp_world = { 20, 10 };
    init_windows(&interface, temp_world);

    char map[MAX_WORLD_SIZE][MAX_WORLD_SIZE] = { 0 };


    // Ignore 'pipe closed' signal so program continues
    sigaction(SIGPIPE, &(struct sigaction) { SIG_IGN }, NULL);

    if (make_folder_if_not_created(FIFO_LOCATION_FOLDER)) {
#if LOGGING
        if (log_this(logfile, "[__CLIENT__]: Could not create FIFO folder\n"))
            return printf("[__CLIENT__]: Logging error\n"), 1;
#endif
        return 3;
    }

    // Start communication
    char response_fifo_path[MAXLEN];
    char request_fifo_path[MAXLEN];

    pthread_t keyboard_input;

    key_info key_info;

    key_info.key = 0;

    if (pthread_mutex_init(&key_info.mutex, NULL) != 0) {
#if LOGGING
        if (log_this(logfile, "[__CLIENT__]: Mutex initialization error\n"))
            return printf("[__CLIENT__]: Logging error\n"), 1;
#endif
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
#if LOGGING
                if (log_this(logfile, "[__CLIENT__]: Could not create FIFO\n"))
                    return printf("[__CLIENT__]: Logging error\n"), 1;
#endif

                return 5;
            } else
                continue;

        CLIENT_ID = i;

#if LOGGING
        sprintf(logmsg,
            "[__CLIENT__]: created response_fifo_path at `%s`, ID: %d\n",
            response_fifo_path, i);
        if (log_this(logfile, logmsg))
            return printf("[__CLIENT__]: Logging error\n"), 1;
#endif

        break;
    }

    if (CLIENT_ID < 0) {
#if LOGGING
        if (log_this(logfile, "[__CLIENT__]: There isn't a free player slot "
                              "inside the server\n"))
            return printf("[__CLIENT__]: Logging error\n"), 1;
#endif

        printw("There isn't a free player slot inside the server\n");
        refresh();
        return 6;
    }

    // Connect to FIFO for sending data to the server

    int client_request = -1; // FIFO descriptor

    create_fifo_path(request_fifo_path, CLIENT_ID, CLIENT_DATA_FIFO_PATH);

#if LOGGING
    sprintf(logmsg, "[__CLIENT__]: Opening FIFO on %s\n", request_fifo_path);
    if (log_this(logfile, logmsg))
        return printf("[__CLIENT__]: Logging error\n"), 1;
#endif

    // ---------------------- Client data (client wirte) ----------------------

    int tries = 1;
    int sleep_time = 3;

    while (tries <= 5) {
        // Try connecting to a FIFO

        client_request = open(request_fifo_path, O_WRONLY);
        if (client_request < 0) {
#if LOGGING
            sprintf(
                logmsg,
                "[__CLIENT__]: Could not find server, retry %d of 5 in 3s...\n",
                tries);
            if (log_this(logfile, logmsg))
                return printf("[__CLIENT__]: Logging error\n"), 1;
#endif

            printw(
                "[__CLIENT__]: Could not find server, retry %d of 5 in 3s...\n",
                tries++);
            refresh();
        } else {
#if LOGGING
            if (log_this(logfile,
                    "[__CLIENT__]: Successfully connected to server!\n"))
                return printf("[__CLIENT__]: Logging error\n"), 1;
#endif

            break;
        }
        sleep(sleep_time);
    }

    if (tries > 5) {
#if LOGGING
        sprintf(logmsg,
            "[__CLIENT__]: Could not find server, retry %d of 5 in 3s...\n",
            tries++);
        if (log_this(logfile, logmsg))
            return printf("[__CLIENT__]: Logging error\n"), 1;
#endif

        endwin();
        printf("[__CLIENT__]: Could not connect to server, aborted after 5 "
               "tries\n");
        return 9;
    }

    server_response = open(response_fifo_path, O_RDONLY);
    if (server_response < 0) {
#if LOGGING
        sprintf(logmsg, "FIFO error at `%s`\n", response_fifo_path);
        if (log_this(logfile, logmsg))
            return printf("[__CLIENT__]: Logging error\n"), 1;
#endif

        printw("FIFO error at `%s`\n", response_fifo_path);
        refresh();
        return 7;
    }

    // ------------------- GAME LOOP -------------------

    erase();
    refresh();

    client_data data;
    data.pid = getpid();

    int frame_counter = 0;

    while (data.key != 'q' && data.key != 'Q') {
#if LOGGING
        sprintf(logmsg, "[__CLIENT__]: ------------------------- frame %3d -------------------------\n", frame_counter);
        if (log_this(logfile, logmsg))
            return printf("[__CLIENT__]: Logging error\n"), 1;
#endif

        data.key = ' ';

        pthread_mutex_lock(&key_info.mutex);
        data.key = key_info.key;
        key_info.key = 0;
        pthread_mutex_unlock(&key_info.mutex);

#if LOGGING
        sprintf(logmsg, "[__CLIENT__]: pressed - `%c`\n", data.key);
        if (log_this(logfile, logmsg))
            return printf("[__CLIENT__]: Logging error\n"), 1;
#endif

        // SEND

        int w = write(client_request, &data, sizeof(data));
        if (w == -1) {
#if LOGGING
            if (log_this(logfile, "[__CLIENT__]: FIFO Write error"))
                return printf("[__CLIENT__]: Logging error\n"), 1;
#endif

            // FIFO Write error
            close(client_request);
            return 8;
        }

        // RECEIVE

        server_data response;

        int r = read(server_response, &response, sizeof(server_data));
        if (r == -1) {
#if LOGGING
            if (log_this(logfile, "[__CLIENT__]: FIFO Reading error"))
                return printf("[__CLIENT__]: Logging error\n"), 1;
#endif

            // FIFO Reading error
            close(client_request);
            return 9;
        }

#if LOGGING
        sprintf(logmsg, "[__CLIENT__]: RESPONSE: %d\n", response.ok);
        if (log_this(logfile, logmsg))
            return printf("[__CLIENT__]: Logging error\n"), 1;
#endif

        update_windows(interface, map);
        
        usleep(1000 * 100 * wait_tenth_of_second);
    }

    close(client_request);
    unlink(response_fifo_path);

    pthread_join(keyboard_input, NULL);

    endwin();

#if LOGGING
    fclose(logfile);
#endif

    return 0;
}