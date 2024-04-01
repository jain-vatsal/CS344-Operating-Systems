#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define sz 400 // Shared data and synchronization objects
#define rep(i, a, b) for (int i = a; i < b; i++)
pthread_mutex_t mutex[sz][sz];
int A[sz][sz], B[sz][sz], res[sz][sz];
void *thread_function(void *arg) // Thread function that waits for a condition to be met
{
    int *ptr = (int *)(arg);
    int num = *ptr, temp1[sz][sz / 4], temp2[sz / 4][sz];
    rep(i, 0, sz) rep(j, (sz / 4) * num, (sz / 4) * num + sz / 4)
        temp1[i][j - (sz / 4) * num] = A[i][j];
    rep(i, (sz / 4) * num, (sz / 4) * num + sz / 4) rep(j, 0, sz)
        temp2[i - (sz / 4) * num][j] = B[i][j];
    printf("Data has been to thread %d...\n", num);
    rep(i, 0, sz) rep(j, 0, sz)
    {
        int temp_ans = 0;
        rep(k, 0, sz / 4)
            temp_ans += (temp1[i][k] * temp2[k][j]);
        pthread_mutex_lock(&mutex[i][j]);
        res[i][j] += temp_ans;
        pthread_mutex_unlock(&mutex[i][j]);
    } // Do some work with the shared data
    printf("Thread: %d finished...\n", num);
    pthread_exit(NULL);
}
int main()
{
    srand(time(NULL));
    pthread_t thread[4];
    int val[4];
    rep(i, 0, sz) rep(j, 0, sz)
    {
        res[i][j] = 0;
        A[i][j] = rand() % 10; 
        // A[i][j]=1;
        B[i][j] = rand() % 10; 
        // B[i][j] = 2;
    }
    rep(i, 0, sz) rep(j, 0, sz) if (pthread_mutex_init(&mutex[i][j], NULL) != 0)
    {
        perror("Mutex initialization failed");
        return 1;
    }
    rep(i, 0, 4) // Create the thread
    {
        val[i] = i;
        if (pthread_create(&thread[i], NULL, thread_function, (void *)&val[i]) != 0)
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }
    rep(i, 0, 4) if (pthread_join(thread[i], NULL) != 0) // Create the thread
    {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }
    FILE *file1 = fopen("firstMatrix.txt", "w");
    if (file1 == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    rep(i, 0, sz)
    {
        rep(j, 0, sz)
            fprintf(file1, "%d ", A[i][j]);
        fprintf(file1, "\n");
    }
    FILE *file2 = fopen("secondMatrix.txt", "w");
    if (file2 == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    rep(i, 0, sz)
    {
        rep(j, 0, sz)
            fprintf(file2, "%d ", B[i][j]);
        fprintf(file2, "\n");
    }
    FILE *file = fopen("result.txt", "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    rep(i, 0, sz)
    {
        rep(j, 0, sz)
            fprintf(file, "%d ", res[i][j]);
        fprintf(file, "\n");
    }
    rep(i, 0, sz) rep(j, 0, sz) pthread_mutex_destroy(&mutex[i][j]);
    int res1[sz][sz];
    rep(i, 0, sz) rep(j, 0, sz)
    {
        res1[i][j] = 0;
        rep(k, 0, sz)
            res1[i][j] += (A[i][k] * B[k][j]);
        if (res1[i][j] != res[i][j])
        {
            printf("%d %d %d %d\n", i, j, res[i][j], res1[i][j]);
            printf("Answer is wrong in thread method...\n");
            exit(0);
        }
    }
    printf("Answer  is same in both ways of multiplication. Success!\n");
    printf("The first and second matrices are printed in seperate files has been printed in result.txt file!\n\n");
    printf("The resultant matrix has been printed in result.txt file!\n\n");
    return 0;
}

// gcc -o code1 code1.c -pthread
// ./code1
