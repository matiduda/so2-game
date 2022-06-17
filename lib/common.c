#include "common.h"

void init_colors() {
    init_pair(DEFAULT, COLOR_WHITE, COLOR_BLACK);
    init_pair(WALL, COLOR_BLACK, COLOR_WHITE);

    init_pair(PLAYER, COLOR_WHITE, COLOR_CYAN);
    init_pair(ENEMY, COLOR_RED, COLOR_BLACK);
    init_pair(COIN, COLOR_BLACK, COLOR_YELLOW);
    init_pair(CAMPSITE, COLOR_YELLOW, COLOR_RED);
    init_pair(BUSHES, COLOR_GREEN, COLOR_BLACK);
}

void init_windows(ui *interface, point world_size, int stat_height, int stat_width) {

    WINDOW *game_window = newwin(world_size.y + 2, world_size.x + 2, 0, 0);
    WINDOW *stat_window = newwin(stat_height, stat_width, 0, world_size.x + 2);
    WINDOW *legend = newwin(12, 34, stat_height, world_size.x + 2);

    box(game_window, 0, 0);

    interface->world_size = world_size;
    interface->game_window = game_window;
    interface->stat_window = stat_window;
    interface->legend = legend;
}

void print_legend(WINDOW *w, int Y, int X) {
    mvwprintw(w, Y + 0, X + 0, "Legend:");
    wattron(w, COLOR_PAIR(PLAYER));
    mvwprintw(w, Y + 1, X + 0, "1234");
    wattron(w, COLOR_PAIR(DEFAULT));
    mvwprintw(w, Y + 1, X + 5, "- players");
    wattron(w, COLOR_PAIR(WALL));
    mvwprintw(w, Y + 2, X + 0, " ", MAP_WALL);
    wattron(w, COLOR_PAIR(DEFAULT));
    mvwprintw(w, Y + 2, X + 5, "- wall");
    wattron(w, COLOR_PAIR(BUSHES));
    mvwprintw(w, Y + 3, X + 0, "%c", MAP_BUSHES);
    wattron(w, COLOR_PAIR(DEFAULT));
    mvwprintw(w, Y + 3, X + 5, "- bushes (slow down)");
    wattron(w, COLOR_PAIR(ENEMY));
    mvwprintw(w, Y + 4, X + 0, "%c", MAP_BEAST);
    wattron(w, COLOR_PAIR(DEFAULT));
    mvwprintw(w, Y + 4, X + 5, "- wild beast");
    wattron(w, COLOR_PAIR(COIN));
    mvwprintw(w, Y + 5, X + 0, "%c", MAP_COIN_1);
    wattron(w, COLOR_PAIR(DEFAULT));
    mvwprintw(w, Y + 5, X + 5, "- one coin");
    wattron(w, COLOR_PAIR(COIN));
    mvwprintw(w, Y + 6, X + 0, "%c", MAP_COIN_10);
    wattron(w, COLOR_PAIR(DEFAULT));
    mvwprintw(w, Y + 6, X + 5, "- treasure (10 coins)");
    wattron(w, COLOR_PAIR(COIN));
    mvwprintw(w, Y + 7, X + 0, "%c", MAP_COIN_50);
    wattron(w, COLOR_PAIR(DEFAULT));
    mvwprintw(w, Y + 7, X + 5, "- large treasure (50 coins)");
    wattron(w, COLOR_PAIR(COIN));
    mvwprintw(w, Y + 8, X + 0, "%c", MAP_COIN_DROPPED);
    wattron(w, COLOR_PAIR(DEFAULT));
    mvwprintw(w, Y + 8, X + 5, "- dropped treasure");
    wattron(w, COLOR_PAIR(CAMPSITE));
    mvwprintw(w, Y + 9, X + 0, "%c", MAP_CAMPSITE);
    wattron(w, COLOR_PAIR(DEFAULT));
    mvwprintw(w, Y + 9, X + 5, "- campsite");
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
    signal(SIGPIPE, SIG_IGN);
    
    key_info* info = (key_info *)pkey;

    // Listen for keyboard input

    int key = 0;

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
