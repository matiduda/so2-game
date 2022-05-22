#include "server_func.h"

void checkArguments(int argc, char **argv, int* v, int* f) {

    // Options: -v -> verbose
    //          -f -> force clear FIFO folder

    for(int i = 1; i < argc; ++i) {
        if(strcmp(argv[i], "-v"))
            *v = 1;
        if(strcmp(argv[i], "-f"))
            *f = 1;
    }
}