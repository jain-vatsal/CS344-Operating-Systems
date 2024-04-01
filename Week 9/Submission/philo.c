#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>

// You can Change sleeptime from here
#define SLEEP 1
// Step a: Implement sem wait function
void sem_wait(int semid, int sem_num) {
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;
    semop(semid, &sem_op, 1);
}

// Step b: Implement sem signal function
void sem_signal(int semid, int sem_num) {
    struct sembuf sem_op;
    sem_op.sem_num = sem_num;
    sem_op.sem_op = 1;
    sem_op.sem_flg = 0;
    semop(semid, &sem_op, 1);
}

// Step c: Declare database tables
struct Table {
    char name[50];
    int pid[10];
};
struct Table* tables[5];

// Function to simulate a philosopher's behavior during a round
void philosopher_round(int id, int round, int* semaphores) {
    int left_table = (id + round - 1)%5;
    int right_table = (id + round) % 5;

    // Step k, i: Philosopher picks two adjacent tables (equivalent to forks)
    // Assuming philosopher sits in different positions during each round
    if ((id + round - 1) % 2 == 0) {
        // Even rounds: Pick left table first
        // printf("Transaction %d: waiting on Table %d \n", getpid(), left_table);
        sem_wait(semaphores[left_table], 0);
        // printf("Transaction %d: waiting on Table %d \n", getpid(), right_table);
        sem_wait(semaphores[right_table], 0);
    } else {
        // Odd rounds: Pick right table first
        // printf("Transaction %d: waiting on Table %d \n", getpid(), right_table);
        sem_wait(semaphores[right_table], 0);
        // printf("Transaction %d: waiting on Table %d \n", getpid(), left_table);
        sem_wait(semaphores[left_table], 0);
    }

    // Step k, ii: Acquire appropriate semaphores
    printf("Transaction %d holding the semaphore\n", getpid());

    // Step k, iii: Perform the transaction
    printf("Transaction %d: operating on Table %d - Table %d in round %d\n", getpid(), left_table, right_table, round);
    int indexleft = tables[left_table]->pid[2*(round-1)] == -1 ? 2*(round - 1) : 2*round-1;
    int indexright = tables[right_table]->pid[2*(round-1)] == -1 ? 2*(round - 1) : 2*round-1;

    tables[left_table]->pid[indexleft] = getpid(); // Array index is index
    tables[right_table]->pid[indexright] = getpid();

    // Step k, iv: Sleep for SLEEP seconds
    sleep(SLEEP);
    // Release the semaphores
    if ((id + round - 1) % 2 == 0) {
        printf("Transaction %d: releasing on Table %d - Table %d\n", getpid(), right_table, left_table);
        sem_signal(semaphores[left_table], 0);
        sem_signal(semaphores[right_table], 0);
    }
    else{
        printf("Transaction %d: releasing on Table %d - Table %d\n", getpid(), left_table, right_table);
        sem_signal(semaphores[right_table], 0);
        sem_signal(semaphores[left_table], 0);
    }
}

void initializeSemaphores(int semaphores[],key_t key) {
    for (int i = 0; i < 5; i++) {
        semaphores[i] = semget(key + i, 1, IPC_CREAT | 0666);
        if (semaphores[i] == -1) {
            perror("Failed to create semaphore");
            exit(1);
        }

        // Initialize the semaphore using semctl
        union semun {
            int val;
            struct semid_ds *buf;
            unsigned short *array;
        } arg;
        arg.val = 1; // Set initial semaphore value to 1

        if (semctl(semaphores[i], 0, SETVAL, arg) == -1) {
            perror("Failed to initialize semaphore");
            exit(1);
        }
    }
}

int main() {
    // Step d: Obtain a key
    key_t key = ftok("keyfile", 'A');

    // Step e: Declare 5 semaphores
    int semaphores[5];

    // Step f: Initialize all the semaphores
    initializeSemaphores(semaphores, key);

    // Step g: Create shared memory for 5 database tables
    int shmid[5];

    for (int i = 0; i < 5; i++) {
        shmid[i] = shmget(key + 10 + i, sizeof(struct Table), IPC_CREAT | 0666);
        if (shmid[i] == -1) {
            perror("Failed to create shared memory");
            exit(1);
        }
    }

    // Step h: Attach the shared memory
    for (int i = 0; i < 5; i++) {
        tables[i] = (struct Table*)shmat(shmid[i], 0, 0);
        if (tables[i] == (struct Table*)-1) {
            perror("Failed to attach shared memory");
            exit(1);
        }
    }

    // Step i: Initialize each table with table number
    for (int i = 0; i < 5; i++) {
        snprintf(tables[i]->name, sizeof(tables[i]->name), "%d", i);
        for (int j = 0; j < 10; j++) {
            tables[i]->pid[j] = -1; // Initialize pid array
        }
    }

    // Step j: Create five children, each corresponding to one philosopher
    for (int i = 0; i < 5; i++) {
        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("Failed to fork");
            exit(1);
        }

        if (child_pid == 0) {
            for (int round = 1; round <= 5; round++) {
                // Step k: Philosopher child sits and eats 
                philosopher_round(i, round, semaphores);
                sleep(6*SLEEP);
            }
            exit(0);
        }
    }
    //Waiting for child processes to complete rounds
    for (int i = 0; i < 5; i++) {
        wait(NULL);
    }
    // Step l: Print the contents of every table
    for (int i = 0; i < 5; i++) {
        printf("Table %d:\n", i);
        for (int j = 0; j < 10; j++) {
            printf("\tRound %d - Philosopher %d\n", (j + 2)/2, tables[i]->pid[j]);
        }
        printf("\n");
    }

    // ... (Clean up)
    for (int i = 0; i < 5; i++) {
        if (semctl(semaphores[i], 0, IPC_RMID) == -1) {
            perror("Failed to remove semaphore");
            exit(1);
        }
    }

    // Detach shared memory
    for (int i = 0; i < 5; i++) {
        if (shmdt(tables[i])== -1) {
            perror("Failed to removeshared memory");
            exit(1);
        }
    }
    return 0;
}

// commands to run - 
// gcc philo.c -o philo -lpthread
// ./philo