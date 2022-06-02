#include "../lib/common.h"
#include "../lib/connect.h"
#include "../lib/server_func.h"

char map[MAX_WORLD_SIZE][MAX_WORLD_SIZE] = { 0 };

int main(int argc, char **argv) {
    // Disable printf bufering
    setvbuf(stdout, NULL, _IONBF, 0);

    int IS_VERBOSE = 0;
    int IS_FORCE_CLEAR = 0;
    checkArguments(argc, argv, &IS_VERBOSE, &IS_FORCE_CLEAR);

    if(IS_FORCE_CLEAR) {
        printf("Successfully cleared all FIFOs\n"); // TODO: Make this verbose
        clearFIFO();
    }
    
    // Create communication channel for client
    int fd = 0;
    char path[LEN];

    nextFilepath(path, LEN, FIFO_LOCATION_FOLDER, FIFO_NAME);

    if(mkfifo(path, 0777) < 0)
        if(errno != EEXIST)
            return perror("Could not create FIFO"), errno;
        else
            printf("FIFO already exists\n"); // TODO: Make this verbose

    printf("Waiting for connection...\n"); // TODO: Make this verbose

    fd = open(path, O_RDONLY);
    if (fd < 0)
        return perror("Could not open FIFO"), errno;
    
    // ------------------ GAME LOOP ------------------ 

    char rec = 0;
    while(rec != 'q') {
        int r = read(fd, &rec, sizeof(char));
        if(r == -1) {
            // FIFO Reading error
            close(fd);
            return 3;
        }

        // Wait for the client request for moves

        // Calculate round outcome

        // Send the map information to all clients

        switch(rec) {
            case 65:
                printf("UP\n");
                break;
            case 66:
                printf("DOWN\n");
                break;
            case 67:
                printf("RIGHT\n");
                break;
            case 68:
                printf("LEFT\n");
                break;
        }
    }

    // Delete FIFO
    unlink(path); 

    return 0;
}