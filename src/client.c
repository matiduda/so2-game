#include "../lib/common.h"
#include "../lib/client_func.h"

int main(int argc, char *argv) {

    initscr();
    // cbreak();
    // noecho();
    // nodelay(stdscr, TRUE);
    // scrollok(stdscr, TRUE);

    // Open fifo for receiving data
    
    char data_receive_path[MAXLEN];

    int CLIENT_ID = -1;

    for(int i = 1; i < CLIENTS + 1; i++) {
        create_fifo_path(data_receive_path, i, FIFO_CLIENT_OUT);

        if(mkfifo(data_receive_path, 0777) < 0)
            if(errno != EEXIST)
                return perror("Could not create FIFO"), 1;
            else
                continue;

        CLIENT_ID = i;
        printf("created OUTPUT at `%s`, ID: %d\n", data_receive_path, i);
        break;
    }

    if(CLIENT_ID < 0)
        return perror("There isn't a free player slot inside the server\n"), 2;

    char q = 0;

    // Connect to FIFO for sending data to the server

    char data_send_path[MAXLEN];

    int fd = -1; // FIFO descriptor

    create_fifo_path(data_send_path, CLIENT_ID, FIFO_CLIENT_INP);

    printw("Opening FIFO on %s\n", data_send_path);

    // Try connecting to a FIFO

    fd = open(data_send_path, O_WRONLY);
    if (fd < 0)
        printw("FIFO error at ``\n", data_send_path);
    
    // ------------------ GAME LOOP ------------------
    
    client_info info;
    info.pid = getpid();

    int w = write(fd, &info, sizeof(info));
    if(w == -1) {
        // FIFO Write error
        close(fd);
        return 1;
    }

    client_data data;

    while (q != 'q') {
        if (kbhit()) {
            q = getch();
            printw("Key pressed! It was: %d\n", q);
            // SEND

            data.key = q;

            w = write(fd, &data, sizeof(data));
            if(w == -1) {
                // FIFO Write error
                close(fd);
                return 1;
            }
            refresh();
        }
    }

    endwin();


    close(fd);
    unlink(data_receive_path);

    return 0;
}