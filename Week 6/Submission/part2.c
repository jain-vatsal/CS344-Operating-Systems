// gcc part2.c -o part2 -lm
// ./part2
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <math.h>
#include <syslog.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
struct Message_of_rowcol
{
    long mtype;
    int row;
    int col;
};

int main(void)
{

    int msqid;
    key_t key;
    FILE *f1 = fopen("input.txt", "r");
    FILE *f2 = fopen("output.txt", "w");
    int n, n1;
    fscanf(f1, "%d", &n);
    fscanf(f1, "%d", &n1);
    int kx[n][n1];

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n1; j++)
        {
            fscanf(f1, "%d", &kx[i][j]);
        }
    }

    int m, m1;
    fscanf(f1, "%d", &m);
    fscanf(f1, "%d", &m1);
    int ky[m][m1];

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < m1; j++)
        {
            fscanf(f1, "%d", &ky[i][j]);
        }
    }

    int r, c;
    fscanf(f1, "%d", &r);
    fscanf(f1, "%d", &c);
    int image[r][c];

    for (int i = 0; i < r; i++)
    {
        for (int j = 0; j < c; j++)
        {
            fscanf(f1, "%d", &image[i][j]);
            // fprintf(f2,"%d ",image[i][j]);
        }
    }

    if ((key = ftok("part1.c", 'D')) == -1)
    {
        perror("ftok");
        exit(1);
    }

    if ((msqid = msgget(key, 0666)) == -1)
    { /* connect to the queue */
        perror("msgget");
        exit(1);
    }

    // printf("spock: ready to receive messages, captain.\n");

    int numberchild = 0;
    int total = 256 * 256;

    while (total > 0)
    {
        if (numberchild < 8)
        {
            struct Message_of_rowcol buf;
            if (msgrcv(msqid, (struct Message_of_rowcol *)&buf, sizeof(struct Message_of_rowcol), 0, 0) == -1)
            {
                perror("msgrcv");
            }

            int row = buf.row;
            int col = buf.col;
            pid_t pid;
            numberchild++;
            if ((pid = fork()) < 0)
                perror("can't fork");
            else if (pid != 0) /* parent */
            {
                numberchild--;
                total--;
                wait(NULL);
            }
            else
            { // clild

                // calculate edge value
                long double gx = 0;
                long double gy = 0;
                for (int i = -1; i <= 1; i++)
                {
                    for (int j = -1; j <= 1; j++)
                    {
                        gx += image[i + row][j + col] * kx[i + 1][j + 1];
                        gy += image[i + row][j + col] * ky[i + 1][j + 1];
                    }
                }
                long double g = sqrt(gx * gx + gy * gy);
                if (g > 255)
                    g = 255;
                fprintf(f2, "%d %d %Lf\n", row, col, g);

                exit(1);
            }
        }
    }
    fclose(f2);
    fclose(f1);

    // printing in sorted order
    FILE *f3 = fopen("output.txt", "r");
    long double order[256][256];
    for (int i = 1; i <= 256; i++)
    {
        for (int j = 1; j <= 256; j++)
        {
            int a, b;
            long double c;
            fscanf(f3, "%d %d %Lf", &a, &b, &c);
            order[a - 1][b - 1] = c;
        }
    }
    fclose(f3);
    FILE *f4 = fopen("output.txt", "w");
    for (int i = 1; i <= 256; i++)
    {
        for (int j = 1; j <= 256; j++)
        {
            fprintf(f4, "(%d, %d): %Lf\n", i, j, order[i - 1][j - 1]);
        }
    }
    fclose(f4);
    return 0;
}