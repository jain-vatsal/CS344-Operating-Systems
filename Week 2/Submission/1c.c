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
        // printf("i = %d\n",i);
        if (child == 0)
        {
            // execle with no arguments
            if (i == 0)
                execle("210101110-q1a", NULL);

            // execl with 1 argument
            else if (i == 1)
                execl("./210101110-q1a", "210101110-q1a", "3", NULL);

            // execve with multiple arguments
            else
            {
                char *argv[5];
                argv[0] = "210101110-q1a";
                argv[1] = "10";
                argv[2] = "20";
                argv[3] = "30";
                argv[4] = NULL;
                execve("./210101110-q1a", argv, NULL);
            }
            
            // exiting after completion
            exit(0);
        }

        // parent waits for the child to complete
        else if (child > 0) wait(NULL);

        // fork failed
        else
        {
            printf("Fork failed\n");
            exit(1);
        }
    }
    return 0;
}