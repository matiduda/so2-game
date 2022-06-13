// #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_func.h"

int load_map(char *filepath, char dest[][MAX_WORLD_SIZE], point *size_res) {
    // Function loads game map from a text file.

    // The function detects the world size using
    // provided map and saves it under *size_res

    // If an error occures, the functinon returns
    // positive integer.

    // 0  - map loaded correctly
    // 1 - invalid argument passed
    // 2 - file i/o error
    // 3 - invalid character detected

    const char valid_symb[] = { ' ', '-', '#', '*', 'A', 'D', 'T', 't', 'c' };
    const int valid_s = sizeof(valid_symb);

    if(!filepath)
        return 1;

    FILE *f = fopen(filepath, "rt");
    if(!f)
        return 1;

    // Detect world size
    int size_x = MAX_WORLD_SIZE;
    int size_y = MAX_WORLD_SIZE;
    char buffer = 0;
    int read = 0;

    for(int i = 0; i < MAX_WORLD_SIZE; i++) {
        read = fread(&buffer, sizeof(char), 1, f);
        if(read != 1)
            return fclose(f), 2;
        
        if(buffer == '\n') { size_x = i; break; }
    }

    rewind(f);

    // Read map data

    for(int i = 0; i < MAX_WORLD_SIZE; i++) {
        for(int j = 0; j < size_x; j++) {

            read = fread(&buffer, sizeof(char), 1, f);
            if(read != 1)
                return fclose(f), 2;
            printf("symbol:%c\n", buffer);
            // Check if the character is valid
            int ok = 0;
            for(int v = 0; v < valid_s; v++) {
                if(buffer == valid_symb[v]) {
                    ok++;
                    break;
                }
            }
            if(!ok)
                return fclose(f), 3;
            
            dest[i][j] = buffer;
        }

        // Read newline character
        printf("reading newline...\n");
        read = fread(&buffer, sizeof(char), 1, f);

        if(feof(f)) {
            size_y = i;
            break;
        }

        if(read != 1)
            return fclose(f), 2;
        printf("read %c...\n", buffer);
        if(buffer != '\n') {
            return fclose(f), 3;
        }
    }

    size_res->x = ++size_x;
    size_res->y = size_y;

    return fclose(f), 0;
}


player init_player(int id) {

    player p = { 0 };

    sprintf(p.name, "Player%d", id);

    sem_init(&p.received_data, 0, 0);
    sem_init(&p.map_calculated, 0, 0);

    p.ID = ++id;

    return p;
}

void print_player_info(player p) {
    printf("Parameter: %s\nID: %d\nType: %d\nCurr X/Y: %d %d\nDeaths: %d\n\n", p.name, p.ID, p.type, p.pos.x, p.pos.y, p.deaths);
}