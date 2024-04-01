#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 9
#define NUM_THREADS (SIZE + SIZE + SIZE) // 9 for columns, 9 for rows, and 9 for 3x3 subgrids

int sudoku[SIZE][SIZE];

typedef struct {
    int topRow;
    int leftCol;
} parameters;

int results[NUM_THREADS] = {0};

void *check_column(void *param) {
    parameters *params = (parameters *)param;
    int col = params->leftCol;
    int numbers[SIZE + 1] = {0};

    for (int i = 0; i < SIZE; i++) {
        int num = sudoku[i][col];
        if (numbers[num] != 0) {
            results[col] = 0;
            return NULL;
        }
        numbers[num] = 1;
    }
    results[col] = 1;
    return NULL;
}

void *check_row(void *param) {
    parameters *params = (parameters *)param;
    int row = params->topRow;
    int numbers[SIZE + 1] = {0};

    for (int i = 0; i < SIZE; i++) {
        int num = sudoku[row][i];
        if (numbers[num] != 0) {
            results[SIZE + row] = 0;
            return NULL;
        }
        numbers[num] = 1;
    }
    results[SIZE + row] = 1;
    return NULL;
}

void *check_subgrid(void *param) {
    parameters *params = (parameters *)param;
    int numbers[SIZE + 1] = {0};
    int startRow = params->topRow;
    int startCol = params->leftCol;

    for (int row = startRow; row < startRow + 3; row++) {
        for (int col = startCol; col < startCol + 3; col++) {
            int num = sudoku[row][col];
            if (numbers[num] != 0) {
                results[SIZE + SIZE + (startRow / 3) * 3 + (startCol / 3)] = 0;
                return NULL;
            }
            numbers[num] = 1;
        }
    }
    results[SIZE + SIZE + (startRow / 3) * 3 + (startCol / 3)] = 1;
    return NULL;
}

int main() {

    FILE* f = fopen("a.txt","r");
    for(int i=0;i<9;i++)for(int j=0;j<9;j++)fscanf(f,"%d",&sudoku[i][j]);

    
    pthread_t threads[NUM_THREADS];
    parameters thread_data[NUM_THREADS];
    int threadIndex = 0;

    // Create threads for checking columns
    for (int i = 0; i < SIZE; i++) {
        thread_data[threadIndex].leftCol = i;
        pthread_create(&threads[threadIndex], NULL, check_column, &thread_data[threadIndex]);
        threadIndex++;
    }

    // Create threads for checking rows
    for (int i = 0; i < SIZE; i++) {
        thread_data[threadIndex].topRow = i;
        pthread_create(&threads[threadIndex], NULL, check_row, &thread_data[threadIndex]);
        threadIndex++;
    }

    // Create threads for checking 3x3 subgrids
    for (int i = 0; i < SIZE; i += 3) {
        for (int j = 0; j < SIZE; j += 3) {
            thread_data[threadIndex].topRow = i;
            thread_data[threadIndex].leftCol = j;
            pthread_create(&threads[threadIndex], NULL, check_subgrid, &thread_data[threadIndex]);
            threadIndex++;
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
            return 0;
        }
    }
    printf("Sudoku solution is valid!\n");
    return 0;
}
// gcc -pthread -o method3 method3.c
// ./method3
