#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 9
#define NUM_THREADS 11 // 1 for rows, 1 for columns, and 9 for subgrids

int sudoku[SIZE][SIZE];

typedef struct {
    int row;
    int column;
} parameters;

int results[NUM_THREADS]; // This will store the results from the threads

void *check_columns(void *params) {
    for (int i = 0; i < SIZE; i++) {
        int numbers[SIZE] = {0};
        for (int j = 0; j < SIZE; j++) {
            int num = sudoku[j][i];
            if (num < 1 || num > 9 || numbers[num - 1] == 1) {
                results[i] = 0; // Invalid column
                pthread_exit(NULL);
            }
            numbers[num - 1] = 1;
        }
    }
    results[SIZE] = 1; // Valid columns
    pthread_exit(NULL);
}

void *check_rows(void *params) {
    for (int i = 0; i < SIZE; i++) {
        int numbers[SIZE] = {0};
        for (int j = 0; j < SIZE; j++) {
            int num = sudoku[i][j];
            if (num < 1 || num > 9 || numbers[num - 1] == 1) {
                results[i] = 0; // Invalid row
                pthread_exit(NULL);
            }
            numbers[num - 1] = 1;
        }
    }
    results[SIZE + 1] = 1; // Valid rows
    pthread_exit(NULL);
}

void *check_subgrid(void *param) {
    parameters *params = (parameters *)param;
    int rowStart = params->row;
    int colStart = params->column;
    int numbers[SIZE] = {0};

    for (int i = rowStart; i < rowStart + 3; i++) {
        for (int j = colStart; j < colStart + 3; j++) {
            int num = sudoku[i][j];
            if (num < 1 || num > 9 || numbers[num - 1] == 1) {
                results[rowStart + colStart / 3] = 0; // Invalid subgrid
                free(params);
                pthread_exit(NULL);
            }
            numbers[num - 1] = 1;
        }
    }
    results[rowStart + colStart / 3] = 1; // Valid subgrid
    free(params);
    pthread_exit(NULL);
}

void validate_sudoku() {
    pthread_t threads[NUM_THREADS];
    int threadIndex = 0; // Index for threads array

    // Create thread for checking rows
    pthread_create(&threads[threadIndex++], NULL, check_rows, NULL);

    // Create thread for checking columns
    pthread_create(&threads[threadIndex++], NULL, check_columns, NULL);

    // Create threads for checking subgrids
    for (int i = 0; i < SIZE; i += 3) {
        for (int j = 0; j < SIZE; j += 3) {
            parameters *gridData = (parameters *)malloc(sizeof(parameters));
            gridData->row = i;
            gridData->column = j;
            pthread_create(&threads[threadIndex++], NULL, check_subgrid, (void *)gridData);
        }
    }

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


// gcc -o method1 method1.c -pthread
// ./method1
