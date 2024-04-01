#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

union semun {
    int val; /* used for SETVAL only */
    struct semid_ds *buf; /* for IPC_STAT and IPC_SET */
    unsigned short *array; /* used for GETALL and SETALL */
};

int main() {

    key_t key = ftok("repository.c", 'U');
    int shm_id = shmget(key, 1024, IPC_CREAT | 0666);
    char *shm_addr = (char*)shmat(shm_id, (void *)0, 0);
    if(shm_addr == (char*)(-1)){
        perror("shmat");
        exit(1);
    }

    char hello[] = "#include<stdio.h>\nint main(){\nprintf(\"Hello World!\");\n}";

    strcpy(shm_addr, hello);

    key_t read_key = ftok("repository.c", 'T');
    int shm_id1 = shmget(read_key, 128, IPC_CREAT | 0666);
    int *read_count = (int*)shmat(shm_id1, (void *)0, 0);
    if(read_count == (int*)(-1)){
        perror("shmat");
        exit(1);
    }
    *read_count = 0;

    key_t key1 = ftok("writer.c", 'W');
    key_t key2 = ftok("writer.c", 'S');

    int rw_mutex = semget(key1, 1, IPC_CREAT | 0666);
    int mutex = semget(key2, 1, IPC_CREAT | 0666);
    
    union semun arg;

    arg.val = 1;

    if (semctl(rw_mutex, 0, SETVAL, arg) == -1) {
        perror("semctl");
        exit(1);
    }

    if (semctl(mutex, 0, SETVAL, arg) == -1) {
        perror("semctl");
        exit(1);
    }

    printf("Repository running. Press enter to exit.\n");
    getchar();

    // Detach shared memory

    shmdt(shm_addr); 
    shmdt(read_count);

    // Delete semaphores and shared memory

    if (semctl(rw_mutex, 0, IPC_RMID, arg) == -1) {
        perror("semctl");
        exit(1);
    }

    if (semctl(mutex, 0, IPC_RMID, arg) == -1) {
        perror("semctl");
        exit(1);
    }

    shmctl(shm_id, IPC_RMID, NULL);
    shmctl(shm_id1, IPC_RMID, NULL);

    return 0;

}