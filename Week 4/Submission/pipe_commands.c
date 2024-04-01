#include <stdio.h>

#include <sys/wait.h>

#include <sys/types.h>

#include <unistd.h>

#include <stdlib.h>

#include <syslog.h>

#include <fcntl.h>

#include <sys/resource.h>

#include <sys/stat.h>

#include <string.h>

 

int main()

{

    char s[500];

    // Reading from the input

    fgets(s, 500, stdin);

    char left[128], right[128], middle[128];

    // If user enters quit then return

    if (strncmp(s, "quit", 4) == 0)

    {

        exit(0);

    }

    int ind = 0;

    while (s[ind] != '|')

    {

        left[ind] = s[ind];

        ind++;

    }

    left[ind - 1] = '\0';

    ind += 2;

    int ind1 = 0;

    while (s[ind] != '|')

    {

        middle[ind1] = s[ind];

        ind1++;

        ind++;

    }

    middle[ind1 - 1] = '\0';

    ind1 = 0;

    ind += 2;

    while (s[ind] != '\0')

    {

        right[ind1] = s[ind];

        ind++;

        ind1++;

    }

    right[ind1 - 1] = '\0';

 

    int p[2];

    pid_t pid;

    if (pipe(p) < 0)

    {

        exit(1);

    }

 

    if ((pid = fork()) > 0)

    {

        close(p[0]);

        dup2(p[1], STDOUT_FILENO);

        close(p[1]);

        system(left);

        

    }

    else if (pid == 0)

    {

        pid_t pid1;

        int q[2];

        // Fork failed

        if (pipe(q) < 0)

        {

            exit(1);

        }

        close(p[1]);

        dup2(p[0], STDIN_FILENO);

        close(p[0]);

        // Parent process

        if ((pid1 = fork()) > 0)

        {

            close(q[0]);

            dup2(q[1], STDOUT_FILENO);

            close(q[1]);

            system(middle);

            // execlp("sh","sh","-c",middle,NULL);

        }

        // Child process

        else if (pid1 == 0)

        {

            close(q[1]);

            dup2(q[0], STDIN_FILENO);

            close(q[0]);

            system(right);

            // execlp("sh","sh","-c",right,NULL);

        }

    }

}