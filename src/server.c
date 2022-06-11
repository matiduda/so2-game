#include "../lib/common.h"
#include "../lib/connect.h"
#include "../lib/server_func.h"

int main(int argc, char **argv) {

    // Disable printf bufering
    setvbuf(stdout, NULL, _IONBF, 0);

    char map[MAX_WORLD_SIZE][MAX_WORLD_SIZE] = { 0 };
    player players[4] = { 0 };
    pthread_t player_thr[4];

    for(int i = 0; i < CLIENTS; i++) {
        players[i] = init_player(i);

        pthread_create(&player_thr[i], NULL, player_connection, &players[i]);
    }

    for(int i = 0; i < CLIENTS; i++) {
        
        pthread_join(player_thr[i], NULL);
    }


    // ------------------ GAME LOOP ------------------ 

    return 0;
}