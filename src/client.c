#include "../lib/common.h"



// TEST

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


int main(int argc, char *argv) {



    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);

    scrollok(stdscr, TRUE);

    char q = 0;

    const char* path= "pipes/connect_client";
    int fd = open(path, O_WRONLY);
    if (fd < 0)
        return perror("Could not open FIFO"), errno;

    printw("Opened FIFO");

    while (q != 'q') {
        if (kbhit()) {
            q = getch();
            printw("Key pressed! It was: %d\n", q);
            // SEND
            int w = write(fd, &q, sizeof(char));
            if(w == -1) {
                // FIFO Write error
                close(fd);
                return 1;
            }
            refresh();
        }
    }


    close(fd);
    return 0;
}
