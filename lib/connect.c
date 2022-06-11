#include "connect.h"

void *player_connection(void *player_struct) {
    
    player* p = (player *)player_struct;

    // Search for client output FIFO

    // Create communication channel for client
    int fd = -1;
    char path[MAXLEN];

    int playerID = p->ID;

    if(create_fifo_path(path, p->ID, FIFO_CLIENT_INP) != 0)
        return printf("[%s]: incorrect FIFO path length", p->name), NULL;

    if(mkfifo(path, 0777) < 0)
        if(errno != EEXIST)
            return printf("[%s]: ", p->name), perror("could not create FIFO"), NULL;
        else
            printf("[%s]: FIFO already exists\n", p->name);

    printf("[%s]: waiting for connection...\n",  p->name);

    fd = open(path, O_RDONLY);
    if (fd < 0)
        return printf("[%s]: ", p->name), perror("Could not open FIFO"), NULL;

    // Listen for connection packet
    int r;
    client_info info;

    r = read(fd, &info, sizeof(info));
    if(r == -1) {
        // FIFO Reading error
        close(fd);
        return NULL;
    }

    printf("client pid: %d\n", info.pid);

    // Listen for data packets
    client_data data;

    while(1) {

        // Check if the program was not force closed

        if(getpgid(info.pid) < 0) {
            printf("[%s]: [PID] client process terminated\n",  p->name);

            printf("[%s]: [PID] cleaning up client FIFOs\n",  p->name);

            clean_up_after_client(p->ID);

            reconnect_client(fd, path, &info);

            printf("[%s]: [PID] reconnected\n",  p->name);
        }

        r = read(fd, &data, sizeof(data));
        if(r == -1) {
            // FIFO Reading error
            close(fd);
            return NULL;
        }

        if(data.key == 'q' || data.key == 'Q') {
            printf("[%s]: [Q] client requested quit\n",  p->name);

            reconnect_client(fd, path, &info);

            printf("[%s]: [PID] reconnected\n",  p->name);

            continue;
        }

        switch(data.key) {
            case 65:
                printf("[%s]: client move - UP\n",  p->name);
                break;
            case 66:
                printf("[%s]: client move - DOWN\n",  p->name);
                break;
            case 67:
                printf("[%s]: client move - RIGHT\n",  p->name);
                break;
            case 68:
                printf("[%s]: client move - LEFT\n",  p->name);
                break;
        }

        // Calculate round outcome

        // Send the map information to all clients
    }

    // Delete FIFO
    close(fd);
    unlink(path);

    return NULL;
}

int reconnect_client(int fd, char* path, client_info *info) {
    close(fd);
    unlink(path);
    open_fifo(path);
    get_client_info(fd, info);
}

int open_fifo(char *path) {

    // Returns FIFO file descriptor

    if(mkfifo(path, 0777) < 0)
        if(errno != EEXIST)
            return -1; // could not create FIFO

        // else - fifo already exists, not a problem


    int fifo_descriptor = -1;

    fifo_descriptor = open(path, O_RDONLY);
    if (fifo_descriptor < 0)
        return printf("[%s : %d]: FIFO opening error\n", __FILE__, __LINE__), 1;


    return fifo_descriptor;
}

int get_client_info(int fifo_descriptor, client_info *info) {

    // Listen for opening packet containing vital client information

    if(read(fifo_descriptor, info, sizeof(client_info)) == -1) {
        // FIFO Reading error
        printf("[%s : %d]: FIFO reading error\n", __FILE__, __LINE__);
        return 1;
    }

    return 0;
}

void clean_up_after_client(int id) {
    char client_out_fifo[MAXLEN];
    create_fifo_path(client_out_fifo, id, FIFO_CLIENT_OUT);
    unlink(client_out_fifo);
}