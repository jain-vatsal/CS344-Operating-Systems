// including necessary header files
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

// main function
int main()
{

    // child process
    pid_t child;
    for (int i = 0; i < 3; i++) // three child processes
    {
        child = fork();
        if (child == 0)
        {
            if (i == 0)
            {
                printf("using execl\n");
                execl("./210101110-q1b","210101110-q1a",NULL);
            }

            // execl with 1 argument
            else if (i == 1)
            {
                printf("using execle\n");
                char* arge[2];
                arge[0] = "n=7";
                arge[1] = NULL;
                execle("./210101110-q1b","210101110-q1a",NULL,arge);
            }

            // execve with multiple arguments
            else
            {
                printf("using execve\n");
                char *argv[2];
                argv[0] = "210101110-q1b";
                argv[1] = NULL;
                char *arge[2];
                arge[0] = "n=3 5 10";
                arge[1] = NULL;
                execve("./210101110-q1b",argv,arge);
            }

            // exiting after completion
            exit(0);
        }

        // parent waits for the children to complete
        else if (child > 0)
            wait(NULL);

        // fork failed
        else
        {
            printf("Fork failed\n");
            exit(1);
        }
    }
    return 0;
}