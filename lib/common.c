#include "common.h"

int create_fifo_path(char *dest, int id, char *type) {
    // Check the length of the strings
    if(strlen(FIFO_LOCATION_FOLDER) + strlen(type) + 2 >= MAXLEN)
        return 1;

    sprintf(dest, "%s/%s_%d", FIFO_LOCATION_FOLDER, type, id);

    return 0;
}