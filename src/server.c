#include "../lib/common.h"

// TODO: All of this mess goes to seperate file


int clearFIFO();
int nextFilepath(char *dest, size_t size, const char *s1, const char *s2);
void checkArguments(int argc, char **argv, int* v, int* f);

int main(int argc, char **argv) {

    // 
    // Options: -v -> verbose
    //          -f -> force clear FIFO folder
    // 

    // Disable printf bufering
    setvbuf(stdout, NULL, _IONBF, 0);

    int IS_VERBOSE = 0;
    int IS_FORCE_CLEAR = 0;
    checkArguments(argc, argv, &IS_VERBOSE, &IS_FORCE_CLEAR);

    if(IS_FORCE_CLEAR) {
        printf("Successfully cleared all FIFOs\n"); // TODO: Make this verbose
        clearFIFO(FIFO_LOCATION_FOLDER);
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
    
    char rec = 0;
    while(rec != 'q') {
        int r = read(fd, &rec, sizeof(char));
        if(r == -1) {
            // FIFO Reading error
            close(fd);
            return 3;
        }

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

    unlink(path);
    return 0;
}

int clearFIFO(const char *dirpath) {

    // Source: https://stackoverflow.com/questions/11007494/    how-to-delete-all-files-in-a-folder-but-not-delete-the-folder-using-nix-standar

    DIR *theFolder = opendir(dirpath);
    if(theFolder == NULL)
        return 1;

    struct dirent *next_file;
    char filepath[LEN];

    while ( (next_file = readdir(theFolder)) != NULL )
    {
        if (!strcmp(next_file->d_name, ".") || !strcmp(next_file->d_name, ".."))
            continue;

        if (nextFilepath(filepath, LEN, dirpath, next_file->d_name))
            return 2;

        printf("Removing %s...\n", filepath);
        unlink(filepath);
    }

    closedir(theFolder);
}

int nextFilepath(char *dest, size_t size, const char *s1, const char *s2) {
    // Check the length of the strings
    if(strlen(s1) + strlen(s2) + 1 >= size)
        return 1;

    sprintf(dest, "%s/%s", s1, s2);

    return 0;
}

void checkArguments(int argc, char **argv, int* v, int* f) {
    for(int i = 1; i < argc; ++i) {
        if(strcmp(argv[i], "-v"))
            *v = 1;
        if(strcmp(argv[i], "-f"))
            *f = 1;
    }
}