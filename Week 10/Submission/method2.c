#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 9
#define NUM_THREADS (SIZE + SIZE + 1) // 9 for each column, 9 for each row, and 1 for all subgrids

int sudoku[SIZE][SIZE];

typedef struct {
    int index;
} parameters;

int results[NUM_THREADS] = {0}; // Results from the threads

void *check_column(void *param) {
    parameters *params = (parameters *)param;
    int col = params->index;
    int numbers[SIZE] = {0};
    
    for (int i = 0; i < SIZE; i++) {
        int num = sudoku[i][col];
        if (num < 1 || num > SIZE || numbers[num - 1] == 1) {
            results[col] = 0; // Invalid column
            pthread_exit(NULL);
        }
        numbers[num - 1] = 1;
    }
    results[col] = 1; // Valid column
    pthread_exit(NULL);
}

void *check_row(void *param) {
    parameters *params = (parameters *)param;
    int row = params->index;
    int numbers[SIZE] = {0};
    
    for (int i = 0; i < SIZE; i++) {
        int num = sudoku[row][i];
        if (num < 1 || num > SIZE || numbers[num - 1] == 1) {
            results[SIZE + row] = 0; // Invalid row
            pthread_exit(NULL);
        }
        numbers[num - 1] = 1;
    }
    results[SIZE + row] = 1; // Valid row
    pthread_exit(NULL);
}

void *check_subgrids(void *param) {
    for (int x = 0; x < SIZE; x += 3) {
        for (int y = 0; y < SIZE; y += 3) {
            int numbers[SIZE] = {0};
            for (int i = x; i < x + 3; i++) {
                for (int j = y; j < y + 3; j++) {
                    int num = sudoku[i][j];
                    if (num < 1 || num > SIZE || numbers[num - 1] == 1) {
                        results[SIZE + SIZE] = 0; // Invalid subgrid
                        pthread_exit(NULL);
                    }
                    numbers[num - 1] = 1;
                }
            }
        }
    }
    results[SIZE + SIZE] = 1; // All subgrids valid
    pthread_exit(NULL);
}

void validate_sudoku() {
    pthread_t threads[NUM_THREADS];
    parameters thread_data[NUM_THREADS];

    // Create threads for checking columns
    for (int i = 0; i < SIZE; i++) {
        thread_data[i].index = i;
        pthread_create(&threads[i], NULL, check_column, (void *)&thread_data[i]);
    }

    // Create threads for checking rows
    for (int i = 0; i < SIZE; i++) {
        thread_data[SIZE + i].index = i;
        pthread_create(&threads[SIZE + i], NULL, check_row, (void *)&thread_data[SIZE + i]);
    }

    // Create thread for checking all subgrids
    pthread_create(&threads[SIZE + SIZE], NULL, check_subgrids, NULL);

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Check results
    for (int i = 0; i < NUM_THREADS; i++) {
        if (results[i] == 0) {
            printf("Sudoku solution is invalid!\n");
            return;
        }
    }
    printf("Sudoku solution is valid!\n");
}

int main() {
    FILE* f = fopen("a.txt","r");
    for(int i=0;i<9;i++)for(int j=0;j<9;j++)fscanf(f,"%d",&sudoku[i][j]);
    validate_sudoku();
    return 0;
}
// gcc -o method2 method2.c -pthread
// ./method2
