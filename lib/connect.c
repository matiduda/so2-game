#include "connect.h"

// TODO: Clean this mess

void *player_connection(void *player_struct) {
    
    player* p = (player *)player_struct;

    char request_fifo_path[MAXLEN];
    char response_fifo_path[MAXLEN];

    if(create_fifo_path(request_fifo_path, p->ID, CLIENT_DATA_FIFO_PATH) != 0)
        return printf("[%s]: incorrect FIFO path length", p->name), NULL;

    if(create_fifo_path(response_fifo_path, p->ID, SERVER_DATA_FIFO_PATH) != 0)
        return printf("[%s]: incorrect FIFO send path length", p->name), NULL;

    // ---------------------- Client data (server read) ----------------------

    if(mkfifo(request_fifo_path, 0777) < 0)
        if(errno != EEXIST)
            return printf("[%s]: ", p->name), perror("could not create FIFO"), NULL;
        else
            printf("[%s]: FIFO already exists\n", p->name);
    
    printf("[%s]: Waiting for connection\n",  p->name);

    // Waif for client to connect

    int client_request = open(request_fifo_path, O_RDONLY);
    if (client_request < 0)
        return printf("[%s]: ", p->name), perror("Could not open client FIFO (read)"), NULL;

    // ---------------------- Server data (server write) ----------------------

    int server_response = open(response_fifo_path, O_WRONLY);
    if (server_response < 0)
        return printf("[%s]: ", p->name), perror("Could not open server FIFO (write)"), NULL;

    client_data data;

    while(1) {

        printf("[%s]: Listening for packets\n",  p->name);

        int r = read(client_request, &data, sizeof(data));
        if(r == -1) {
            // FIFO Reading error
            printf("[%s]: Reading error!\n",  p->name);
            close(client_request);
            return NULL;
        }

        printf("[%s]: Received data: %c\n",  p->name, data.key);

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
            default:
                printf("[%s]: client move - unknown: %c\n",  p->name, data.key);
                break;
        }

        // Check if the program was not force closed

        if(getpgid(data.pid) < 0) {
            printf("[%s]: [PID] client process terminated\n",  p->name);

            printf("[%s]: [PID] cleaning up client FIFOs\n",  p->name);

            clean_up_after_client(p->ID);

            int rec = reconnect_client(&client_request, &server_response, request_fifo_path, response_fifo_path);
            if(rec) {
                printf("reconnect error\n");
                return NULL;
            }
            

            continue;
        }

        server_data response;

        if(data.key == 'o')
            response.ok = 0;
        else
            response.ok = 1;
        
        int w = write(server_response, &response, sizeof(server_data));
        if(w == -1) {
            // FIFO Write error
            printf("[%s]: Could not write response\n",  p->name);
            // Close all communication
            return NULL;
        }

        
        if(data.key == 'q' || data.key == 'Q') {
            printf("[%s]: [Q] client requested quit, reconnecting...\n",  p->name);

            int rec = reconnect_client(&client_request, &server_response, request_fifo_path, response_fifo_path);
            if(rec) {
                printf("reconnect error\n");
                return NULL;
            }

            printf("[%s]: [Q] reconnected\n",  p->name);

            continue;
        }

    }

    // Delete FIFO
    close(client_request);
    unlink(request_fifo_path);

    return NULL;
}

int reconnect_client(int* request_fd, int* response_fd, char* request_path, char* response_path) {

    if(!request_fd || !response_fd || !request_path || !response_path)
        return -1;

    if(*request_fd >= 0) {
        close(*request_fd);
        unlink(request_path);
    }

    if(make_fifo(request_path) < 0)
        return printf("[%s : %d]: Reconnect error (could not make request FIFO)\n", __FILE__, __LINE__), -1;

    int new_request = open_fifo(request_path, 0);

    if(new_request < 0) {
        unlink(request_path);
        printf("[%s : %d]: Reconnect error (could not open request FIFO)\n", __FILE__, __LINE__);
        return -2;
    }


    int new_response = open(response_path, O_WRONLY);
    if (new_response < 0) {
        unlink(request_path);
        close(new_request);
        printf("[%s : %d]: Reconnect error (could not open response FIFO)\n", __FILE__, __LINE__);
        return -3;
    }

    *request_fd = new_request;
    *response_fd = new_response;

    return 0;
}

int make_fifo(char *path) {

    // Returns FIFO file descriptor

    if(mkfifo(path, 0777) < 0)
        if(errno != EEXIST)
            return -1; // could not create FIFO

        // else - fifo already exists, not a problem
}

int open_fifo(char *path, int read_or_write) {

    // read_or_write:
    // READ     - 0
    // WRITE    - 1

    if(read_or_write != 0 && read_or_write != 1)
        return -2;

    int fd = -1;

    if(read_or_write)
        fd = open(path, O_WRONLY);
    else
        fd = open(path, O_RDONLY);

    return fd;
}

void clean_up_after_client(int id) {
    char client_out_fifo[MAXLEN];
    create_fifo_path(client_out_fifo, id, SERVER_DATA_FIFO_PATH);
    unlink(client_out_fifo);
}

