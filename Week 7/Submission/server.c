#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <wait.h>

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
    int shm_id, sem_id;
    char* data;

    union semun arg;

    // Obtain a key for shared memory
    // Create shared memory segment of size SHM_SIZE
    shm_key = ftok("server.c", 'R');
    shm_id = shmget(shm_key, SHM_SIZE, 0644 | IPC_CREAT);
    if(shm_id == -1){
        perror("shmget");
        exit(0);
    }

    // Obtain a key for semaphores
    // Declare appropriate semaphores
    sem_key = ftok("client.c", 'E');
    sem_id = semget(sem_key, 1, 0666 | IPC_CREAT);
    if(sem_id == -1){
        perror("semget");
        exit(0);
    }
 

    // Initialize the semaphores
    arg.val = 1;
    if (semctl(sem_id, 0, SETVAL, arg) == -1) {
        perror("semctl");
        exit(1);
    }

    data = shmat(shm_id, (void *)0, 0);
    if (data == (char *)(-1)) {
        perror("shmat");
        exit(1);
    }

    for(int i=0;i<25;i++){
        *(data + i) = 0;
    }
 

    // Create two child processes
    for (int i = 0; i < 2; i++) {
        pid_t child_pid = fork();
        if (child_pid == 0) {

            // Attach the shared memory segment
            char* ptr = shmat(shm_id, (void *)0, 0);
            if (ptr == (char *)(-1)) {
                perror("shmat");
                exit(1);
            }

            char* cnt = ptr + 23;
            char* front = ptr + 24;

            while ((*cnt)<100) {

                // Perform appropriate semaphore operations (wait)
                sem_wait(sem_id);

                // Dequeue from shared memory and print the HTTP request number
                if(*(ptr + *front) != 0){
                    int http_req_num = *(ptr + *front);
                    printf("Child %d: %d\n", i + 1, http_req_num);
                    *(ptr + *front) = 0;
                    (*front)++;
                    (*front) %= 23;
                    (*cnt)++;
                }

                // Perform appropriate semaphore operations (signal)
                sem_signal(sem_id);


                sleep(0.2);
            }
            
            // Detach the shared memory
            if (shmdt(ptr) == -1) {
                perror("shmdt");
                exit(1);
            }
            exit(0);
        }
    }

    wait(NULL);
    wait(NULL);

    // Remove the semaphores
    if (semctl(sem_id, 0, IPC_RMID, arg) == -1) {
        perror("semctl");
        exit(1);
    }

    // Detach the shared memory
    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
    }

    shmctl(shm_id, IPC_RMID, NULL);
 

    return 0;
}