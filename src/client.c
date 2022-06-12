#include "../lib/common.h"
#include "../lib/connect.h"
#include "../lib/client_func.h"

// TODO: Clean this mess

int main(int argc, char *argv) {

    sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);

    initscr();
    noecho();

    char response_fifo_path[MAXLEN];
    char request_fifo_path[MAXLEN];

    pthread_t keyboard_input;

    key_info key_info;

    key_info.key = 0;

    if (pthread_mutex_init(&key_info.mutex, NULL) != 0)
        return printw("Mutex initialization error\n"), 1;

    pthread_create(&keyboard_input, NULL, keyboard_input_func, &key_info);

    // ---------------------- Server data (client read) ----------------------

    int server_response;
    int CLIENT_ID = -1;

    for(int i = 1; i < CLIENTS + 1; i++) {
        create_fifo_path(response_fifo_path, i, SERVER_DATA_FIFO_PATH);

        if(mkfifo(response_fifo_path, 0777) < 0)
            if(errno != EEXIST)
                return perror("Could not create FIFO"), 1;
            else
                continue;

        CLIENT_ID = i;
        printw("created response_fifo_path at `%s`, ID: %d\n", response_fifo_path, i);
        break;
    }

    if(CLIENT_ID < 0)
        return perror("There isn't a free player slot inside the server\n"), 2;

    // Connect to FIFO for sending data to the server

    int client_request = -1; // FIFO descriptor

    create_fifo_path(request_fifo_path, CLIENT_ID, CLIENT_DATA_FIFO_PATH);

    printw("Opening FIFO on %s\n", request_fifo_path);

    // ---------------------- Client data (client wirte) ----------------------

    int tries = 1;
    int sleep_time = 3;

    while(tries < 5) {

        // Try connecting to a FIFO
        
        client_request = open(request_fifo_path, O_WRONLY);
        if (client_request < 0)
            printw("[CLIENT]: Could not find server, retry %d of 5 in 3s...\n", tries++);
        else {
            printw("[CLIENT]: Successfully connected to server!\n");
            break;
        }
        sleep(sleep_time);
    }

    server_response = open(response_fifo_path, O_RDONLY);
    if (server_response < 0)
        printw("FIFO error at `%s`\n", response_fifo_path);

    client_data data;
    data.pid = getpid();

    while (data.key != 'q' && data.key != 'Q') {
        
        data.key = '0';
        
        pthread_mutex_lock(&key_info.mutex);
        data.key = key_info.key;
        key_info.key = 0;
        pthread_mutex_unlock(&key_info.mutex);

        printw("LAST PRESSED KEY: %c\n", data.key);

        // SEND

        int w = write(client_request, &data, sizeof(data));
        if(w == -1) {
            // FIFO Write error
            close(client_request);
            return 1;
        }
        refresh();

        // RECEIVE

        printw("Waiting for response...\n");

        server_data response;        

        int r = read(server_response, &response, sizeof(server_data));
        if(r == -1) {
            // FIFO Reading error
            close(client_request);
            return 2;
        }

        printw("RESPONSE: %d\n", response.ok);

        refresh();
        sleep(3);
    }

    endwin();


    close(client_request);
    unlink(response_fifo_path);

    pthread_join(keyboard_input, NULL);

    return 0;
}