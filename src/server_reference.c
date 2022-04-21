#include "../lib/common.h"

int main(int argc, char *argv[])
{
    printf("size of payload: %d\n", (int)sizeof(struct payload_t));
    printf("Start serwera...\n");

    // Create shared memory
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);

    if (fd == -1)
        return perror(SHM_NAME), 2;

    if (ftruncate(fd, sizeof(struct payload_t)) == -1)
        return perror("ftruncate"), 3;

    printf("Utworzono pamiec wspoldzielona.\n");

    // Map memory
    struct payload_t *pdata = (struct payload_t *)mmap(NULL, sizeof(struct payload_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (pdata == NULL)
        return shm_unlink(SHM_NAME), perror("mmap"), 4;

    sem_init(&pdata->cs, 1, 1);
    sem_init(&pdata->client_query, 1, 0);
    sem_init(&pdata->server_reply, 1, 0);

    // File name
    printf("Oczekiwanie na nazwe pliku...\n");

    sem_post(&pdata->server_reply);
    sem_wait(&pdata->client_query);

    char filepath[FILENAME_SIZE + 10];
    strcpy(filepath, "server/");
    strcat(filepath, pdata->message);

    printf("Otwieram plik `%s`...\n", filepath);
    FILE *f = fopen(filepath, "rb");

    if (!f)
    {
        sem_wait(&pdata->cs);
        pdata->content_size = 0;
        sem_post(&pdata->cs);

        sem_post(&pdata->server_reply);
    }
    else
    {
        printf("Sending file...\n");

        int i = 0;

        while (!feof(f))
        {
            sem_wait(&pdata->cs);
            pdata->content_size = fread(pdata->file_content, 1, FILEPACK_SIZE, f);
            printf("Package %d (%d bytes)\n", i, pdata->content_size);
            sem_post(&pdata->cs);

            sem_post(&pdata->server_reply);
            sem_wait(&pdata->client_query);

            ++i;
        }

        sem_wait(&pdata->cs);
        pdata->content_size = 0;
        sem_post(&pdata->cs);

        sem_post(&pdata->server_reply);

        fclose(f);
    }

    munmap(pdata, sizeof(struct payload_t));
    close(fd);
    shm_unlink(SHM_NAME);

    return 0;
}
