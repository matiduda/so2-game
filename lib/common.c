#include "common.h"

void init_windows(ui *interface, point world_size) {

    WINDOW *game_window = newwin(world_size.y, world_size.x, 0, 0);
    WINDOW *stat_window = newwin(15, 25, 0, world_size.x);
    WINDOW *legend = newwin(5, 54, 0 + 15, world_size.x);

    // scrollok(game_window, false);
    // scrollok(stat_window, false);
    // scrollok(legend, false);

    box(game_window, 0, 0);
    box(stat_window, 0, 0);
    box(legend, 0, 0);

    interface->world_size = world_size;
    interface->game_window = game_window;
    interface->stat_window = stat_window;
    interface->legend = legend;
}

void update_windows(ui interface, char dest[][MAX_WORLD_SIZE]) {

    WINDOW *game_window = interface.game_window;
    WINDOW *stat_window = interface.stat_window;
    WINDOW *legend = interface.legend;


    werase(stat_window);
    werase(legend);
    werase(game_window);


    int world_height = interface.world_size.y;
    int world_width = interface.world_size.x;
    wresize(game_window, world_height + 2 ,world_width + 2);

    for(int i = 0; i < world_height; i++) {
        // attron(COLOR_PAIR(1));

        for(int j = 0; j < world_width; j++) {

            char c = dest[i][j];

            wattron(game_window, COLOR_PAIR(1));

            if(c == '-') {
                c = ' ';
                wattron(game_window, COLOR_PAIR(2));
            }

            mvwprintw(game_window, i + 1, j + 1, "%c", c);
        }

        mvwprintw(game_window, world_width, i, "\n");

    }


    static int frame_counter = 0;

    mvwprintw(stat_window, 1, 1, "frame: %d\n", frame_counter++);
    mvwprintw(legend, 1, 1, "legend");

    box(game_window, 0, 0);
    box(stat_window, 0, 0);
    box(legend, 0, 0);

    wrefresh(stat_window);
    wrefresh(legend);
    wrefresh(game_window);


}

// -----------------------------------------------------------

int make_folder_if_not_created(char *path) {
    if(!path)
        return 1;

    struct stat st = {0};

    if (stat(FIFO_LOCATION_FOLDER, &st) == -1)
        if(mkdir(FIFO_LOCATION_FOLDER, 0700))
            return 2;
    
    return 0;
}

int create_fifo_path(char *dest, int id, char *type) {
    // Check the length of the strings
    if(strlen(FIFO_LOCATION_FOLDER) + strlen(type) + 2 >= MAXLEN)
        return 1;

    sprintf(dest, "%s/%s_%d", FIFO_LOCATION_FOLDER, type, id);

    return 0;
}


int kbhit(void)
{
    int ch = getch();

    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
}

void* keyboard_input_func(void *pkey) {

    key_info* info = (key_info *)pkey;

    // Listen for keyboard input

    char key = 0;

    while(info->key != 'q' && info->key != 'Q') {
        if (kbhit()) {
            key = getch();
            pthread_mutex_lock(&info->mutex);
            info->key = key;
            pthread_mutex_unlock(&info->mutex);
        }
    }

    return NULL;
}

// ---------------- Logging ----------------

FILE* configure_logging(char *path) {
    if(!path)
        return NULL;
        
    return fopen(path, "w");
}

int log_this(FILE *f, char *msg) {
    if(!f || !msg)
        return 1;

    if(fprintf(f, "%s", msg) < 0)
        return 2;

    return 0;
}
