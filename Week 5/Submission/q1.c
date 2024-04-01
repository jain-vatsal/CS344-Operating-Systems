#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHM_SIZE 1024*602 
// make it a 1K shared memory segment

int main()
{
    key_t key;
    int shmid;
    char data[1024 * 602];
    int mode;

    /* make the key: */
    if ((key = ftok("week05-ML2-input.txt", "r")) == -1)
    {
        perror("ftok");
        exit(1);
    }
    /* connect to (and possibly create) the segment: */
    shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT);

    /* attach to the segment to get a pointer to it: */
    char *data1 = shmat(shmid, (void *)0, 0);
    if (data1 == (char *)(-1))
    {
        perror("shmat");
        exit(1);
    }
    /* read or modify the segment, based on the command line: */
    FILE *f1 = fopen("week05-ML2-input.txt", "r");
    int lines;
    fscanf(f1, "%d", &lines);
    lines++;
    while (lines--)
    {
        char c[1024];
        fgets(c, 1024,f1);
        strcat(data, c);
    }
    strcpy(data1, data);
    FILE *f2 = fopen("ex.txt", "w");
    fprintf(f2, "%s", data1);

    /* detach from the segment: */
    if (shmdt(data1) == -1)
    {
        perror("shmdt");
        exit(1);
    }
    return 0;
}