#include "common.h"


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
            // printw("KEY PRESSED!\n");
            pthread_mutex_lock(&info->mutex);
            info->key = key;
            pthread_mutex_unlock(&info->mutex);
        }
    }

    return NULL;
}