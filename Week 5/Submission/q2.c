#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHM_SIZE 100000
// Size of the shared memory segment

struct score
{
    char battter[256];
    char bowl[256];
    int runs;
    int balls;
    int out;
};

int main()
{

    struct score team[11];

    int shm_id;
    key_t key = ftok("week05-ML2-input.txt", "r");
    // Use the same key as the 1st program

    // Get access to the existing shared memory segment
    shm_id = shmget(key, SHM_SIZE, 0666);
    if (shm_id == -1)
    {
        perror("shmget");
        exit(1);
    }

    // Attach the shared memory created by the 1st program
    char *data = shmat(shm_id, NULL, 0);
    if (data == (char *)-1)
    {
        perror("shmat");
        exit(1);
    }

    // Read the commentary data from shared memory and compute the scorecard
    // You'll need to implement your scorecard logic here

    // Print the computed scorecard
    data = data + 4;
    int count = 0;
    int index;
    int new = 0;
    while (count != 601)
    {

        while (*data != ' ')
        {
            data++;
        }
        data++;

        char bowl[256], battter[256], happens[2560];

        int i = 0;

        while (*data != ' ')
        {
            bowl[i] = *data;
            i++;
            data++;
        }

        bowl[i] = '\0';

        i = 0;
        data = data + 4;

        while (*data != ',')
        {
            battter[i] = *data;
            i++;
            data++;
        }
        data++;
        data++;
        battter[i] = '\0';
        i = 0;

        while (*data != ',' && *data != '\n')
        {
            happens[i] = *data;
            i++;
            data++;
        }
        happens[i] = '\0';
        int r = 0;
        int w = 0;

        if (strcmp(happens, "FOUR") == 0)
        {
            r = 4;
        }
        else if (strcmp(happens, "SIX") == 0)
        {
            r = 6;
        }
        else if (strcmp(happens, "1 run") == 0 || strcmp(happens, "1 run ") == 0)
        {
            r = 1;
        }
        else if (strcmp(happens, "2 runs") == 0)
        {
            r = 2;
        }
        else if (strcmp(happens, "3 runs") == 0)
        {
            r = 3;
        }
        else if (strcmp(happens, "1 wide") == 0)
        {
            r = 0;
            w = 1;
        }
        else if (strcmp(happens, "OUT") == 0)
        {
            r = -1;
        }
        else if (strcmp(happens, "no run") == 0)
        {
            r = 0;
        }

        i = 0;
        index = 0;
        for (i = 0; i < 11; i++)
        {
            if (strcmp(team[i].battter, battter) == 0)
            {
                index = i;
                break;
            }
        }

        if (i == 11)
        {
            team[new].out = 0;
            strcpy(team[new].battter, battter);
            if (r == -1)
            {
                strcpy(team[new].bowl, bowl);
                team[new].runs = 0;
                team[new].out = 1;
            }
            else
            {
                team[new].runs = r;
            }
            team[new].balls = 1;
            if (w == 1)
            {
                team[new].balls = 0;
            }
            new ++;
        }
        else
        {
            team[index].balls++;
            if (r == -1)
            {
                strcpy(team[index].bowl, bowl);
                team[index].out = 1;
            }
            else
            {
                team[index].runs += r;
            }
            if (w == 1)
            {
                team[index].balls--;
            }
        }

        while (*data != '\n')
        {
            data++;
        }
        data++;
        count++;
    }

    for (int i = 0; i < new; i++)
    {
        if (team[i].out == 1)
        {
            printf("%s b %s %d %d\n", team[i].battter, team[i].bowl, team[i].runs, team[i].balls);
        }
        else
        {
            printf("%s * %d %d\n", team[i].battter, team[i].runs, team[i].balls);
        }
    }

    // Free the created shared memory segment
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
