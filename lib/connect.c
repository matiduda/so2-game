
#include "connect.h"

int nextFilepath(char *dest, size_t size, const char *s1, const char *s2) {
    // Check the length of the strings
    if(strlen(s1) + strlen(s2) + 1 >= size)
        return 1;

    sprintf(dest, "%s/%s", s1, s2);

    return 0;
}

int clearFIFO() {

    // Source: https://stackoverflow.com/questions/11007494/    how-to-delete-all-files-in-a-folder-but-not-delete-the-folder-using-nix-standar

    DIR *theFolder = opendir(FIFO_LOCATION_FOLDER);
    if(theFolder == NULL)
        return 1;

    struct dirent *next_file;
    char filepath[LEN];

    while ( (next_file = readdir(theFolder)) != NULL )
    {
        if (!strcmp(next_file->d_name, ".") || !strcmp(next_file->d_name, ".."))
            continue;

        if (nextFilepath(filepath, LEN, FIFO_LOCATION_FOLDER, next_file->d_name))
            return 2;

        printf("Removing %s...\n", filepath);
        unlink(filepath);
    }

    closedir(theFolder);
}
