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

    key_t read_key = ftok("repository.c", 'T');
    int shm_id1 = shmget(read_key, 128, IPC_CREAT | 0666);
    int *read_count = (int*)shmat(shm_id1, (void*)0, 0);

    key_t key1 = ftok("writer.c", 'W');
    key_t key2 = ftok("writer.c", 'S');

    int rw_mutex = semget(key1, 1, IPC_CREAT | 0666);
    int mutex = semget(key2, 1, IPC_CREAT | 0666);
    int rc;

    // do {

        sem_wait(mutex); 

        rc = *read_count;
        *read_count = rc + 1;

        if(*read_count == 1){
            printf("Trying to acquire lock...\n");
            sem_wait(rw_mutex); 
            printf("Lock acquired.\n");
        }

        sem_signal(mutex);

        printf("\n%s\n", shm_addr);
        printf("\nReading done. Press enter to exit.\n");
        getchar();

        sem_wait(mutex);

        rc = *read_count;
        *read_count = rc - 1;

        if(*read_count == 0){
            sem_signal(rw_mutex);
            printf("Lock released.\n");
        }

        sem_signal(mutex);

    // } while(1);

    shmdt(shm_addr); 
    shmdt(read_count);

    return 0;

}