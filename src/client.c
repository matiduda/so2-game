#include "../lib/common.h"

int main(int argc, char *argv[])
{
    // Open shared memory
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);

    if (fd == -1)
        return perror(SHM_NAME), 2;

    printf("Otworzono pamiec wspoldzielona.\n");

    // Map memory
    struct payload_t *pdata = (struct payload_t *)mmap(NULL, sizeof(struct payload_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (pdata == NULL)
        return shm_unlink(SHM_NAME), perror("mmap"), 4;

    printf("Klient dolaczyl do pamieci wspoldzielonej.\n");

    char path[FILENAME_SIZE];
    printf("Podaj nazwe pliku: ");

    fgets(path, FILENAME_SIZE, stdin);
    strtok(path, "\n");

    sem_wait(&pdata->server_reply);

    sem_wait(&pdata->cs);
    strcpy(pdata->message, path);
    sem_post(&pdata->cs);

    sem_post(&pdata->client_query);
    sem_wait(&pdata->server_reply);

    if (pdata->content_size == 0)
    {
        printf("Could not open file `%s`.\n", path);
    }
    else
    {
        // Receive file

        printf("Receiving file...\n");

        FILE *f = fopen(path, "wb");

        int i = 0;

        while (1)
        {
            sem_wait(&pdata->cs);
            fwrite(pdata->file_content, 1, pdata->content_size, f);
            if (pdata->content_size == 0)
                break;

            printf("Package %d (%d bytes)\n", i, pdata->content_size);
            sem_post(&pdata->cs);

            sem_post(&pdata->client_query);
            sem_wait(&pdata->server_reply);

            ++i;
        }

        fclose(f);
    }

    munmap(pdata, sizeof(struct payload_t));
    close(fd);
    shm_unlink(SHM_NAME);

    return 0;
}
