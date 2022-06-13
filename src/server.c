#include "../lib/common.h"
#include "../lib/connect.h"
#include "../lib/server_func.h"

int main(int argc, char **argv) {

    // Ignore 'pipe closed' signal so program continues
    sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);

    // Disable printf bufering
    setvbuf(stdout, NULL, _IONBF, 0);

    char map[MAX_WORLD_SIZE][MAX_WORLD_SIZE] = { 0 };
    player players[4] = { 0 };
    pthread_t player_thr[4];

    for(int i = 0; i < CLIENTS; i++) {
        players[i] = init_player(i);
        pthread_create(&player_thr[i], NULL, player_connection, &players[i]);
    }

    // ------------------ GAME LOOP ------------------ 

    int time = 0;

    while(true) {

        int has_connected_clients = 0;

        for(int i = 0; i < CLIENTS; i++) {

            if(players[i].is_connected) {
                has_connected_clients++;
                printf("[ %d ] WAITING FOR DATA...\n", time++);

                // Wait for data from connected client
                int sw = sem_wait(&players[i].received_data);
                if(sw == -1)
                    return perror("semaphore error"), -1;
                printf("[ %d ] RECEIVED DATA FROM %d clients!\n", time++, has_connected_clients);

                int value; 
                sem_getvalue(&players[i].received_data, &value); 
                printf("The value of the semaphors is %d\n", value);


                sleep(0.01);
                
                sw = sem_post(&players[i].map_calculated);
                if(sw == -1)
                    return perror("semaphore error"), -1;
            }
        }
    }

    for(int i = 0; i < CLIENTS; i++) {
        sem_destroy(&players[i].received_data);
        pthread_join(player_thr[i], NULL);
    }



    return 0;
}