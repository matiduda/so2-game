#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

int load_map(char *filename, char dest[100][100], int size_x, int size_y) {
    FILE *f = fopen(filename, "rt");
    if(f == NULL) {
        printf("\n\nCOULDNT OPEN\n\n");
        return 1;
    }

    int read = 0;

    for(int j = 0; j < size_y; ++j) {
        read = fread(dest[j], sizeof(char), size_x, f);
        if(read != size_x) {
            fclose(f);
            return 0;

        }
        dest[j][size_x + 1] = '\0';
    }

    fclose(f);
    return 0;
}   

int main(void) {

    char map[100][100] = {0};
    int rval = load_map("res/map_layout.txt", map, 52, 25);

    if(rval != 0) {
        printf("ERROR: %d\n", rval);
        return rval;
    }



    // ncurses.h print hello world
    initscr();

    // Refreshes the screen to match what's in memory



    while(1) {
        for(int i = 0; i < 25; ++i)
            printw("%s", map[i]);


        refresh();
        // Waits for user input, return int value of the key
        int character = getch();

        printw("You pressed: %c\n", character);

        if(character == 'q' || character == 'Q')
            break;
    }


    endwin();

    return 0;
}