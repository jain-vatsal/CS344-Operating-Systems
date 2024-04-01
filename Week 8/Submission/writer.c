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

// Function to perform semaphore wait
int sem_wait(int sem_id) {
    struct sembuf sops = {0, -1, 0};

    if (semop(sem_id, &sops, 1) == -1) {
        perror("sem_wait failed");
        return -1;
    }


    return 0;
}

// Function to perform semaphore signal
int sem_signal(int sem_id) {
    struct sembuf sops = {0, 1, 0};

    if (semop(sem_id, &sops, 1) == -1) {
        perror("sem_signal failed");
        return -1;
    }


    return 0;
}

int main() {

    key_t key = ftok("repository.c", 'U');
    int shm_id = shmget(key, 1024, IPC_CREAT | 0666);
    char *shm_addr = (char*)shmat(shm_id, (void*)0, 0);

    key_t key1 = ftok("writer.c", 'W');
    key_t key2 = ftok("writer.c", 'S');

    int rw_mutex = semget(key1, 1, IPC_CREAT | 0666);
    int mutex = semget(key2, 1, IPC_CREAT | 0666);

    // do {
        printf("Trying to acquire lock...\n");
        sem_wait(rw_mutex); 
        printf("Lock acquired.\n");

        int ind = 0;

        while(shm_addr[ind]!='}'){
            ind++;
        }

        sprintf(shm_addr + ind, "printf(\"Hello written by pid: %d\");\n}",getpid());
        printf("Output line added. Press enter to exit.\n");
        getchar();

        // if(strlen(shm_addr)>900) break;

        // Signal that writing is complete

        sem_signal(rw_mutex);
        printf("Lock released.\n");

    // } while(1);

    // Detach shared memory

    shmdt(shm_addr);

    return 0;

}