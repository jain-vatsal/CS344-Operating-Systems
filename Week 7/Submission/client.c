#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define SHM_SIZE 25
#define NUM_REQUESTS 100

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
    key_t shm_key, sem_key;
    int shm_id, sem_id, semid=-1;
    char* data;

    union semun arg;

    // Obtain a key for shared memory
    // Get shared memory segment
    shm_key = ftok("server.c", 'R');
    shm_id = shmget(shm_key, SHM_SIZE, 0644 | IPC_CREAT);
 

    // Obtain a key for semaphores
    // Declare appropriate semaphores
    sem_key = ftok("client.c", 'E');
    sem_id = semget(sem_key, 1, 0666 | IPC_CREAT);

    // Initialize the semaphores
    arg.val = 1;
    if (semctl(sem_id, 0, SETVAL, arg) == -1) {
        perror("semctl");
        exit(1);
    }

    // Attach the shared memory object
    data = shmat(shm_id, (void *)0, 0);
    if (data == (char *)(-1)) {
        perror("shmat");
        exit(1);
    }

    int rear = 0;
    int cnt = 0;

    while(cnt<100){
        sem_wait(sem_id);

        if(*(data + rear) == 0){
            cnt++;
            *(data + rear) = cnt;
            printf("Enqueued %d\n", cnt);
            rear++;
            rear %= 23;
        }
        
        sem_signal(sem_id);
    }



    // Remove the semaphores
    semctl(semid, 0, IPC_RMID, arg);
 

    // Detach the shared memory
    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
    }


    return 0;
}