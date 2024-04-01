#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <signal.h>

void start_daemon()
{
    int pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }
    umask(0);
    pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }
    FILE *f = fopen("hello.txt", "w");
    int pid2 = getpid();
    fprintf(f, "%d\n", pid2);
    fclose(f);
    setsid();
    chdir("/");
    for (int x_fd = sysconf(_SC_OPEN_MAX); x_fd >= 0; x_fd--)
    {
        close(x_fd);
    }
    openlog("Logs", LOG_PID, LOG_USER);
    while (1)
    {
        pid = fork();
        if (pid <= 0)
        {
            int n = getpid();
            printf("Child PID : %d\n", getpid());
            syslog(LOG_INFO,"%d",n);
            while (n != 1)
            {
                if (n % 2 == 0)
                {
                    n = n / 2;
                }
                else
                {
                    n = 3 * n + 1;
                }
                syslog(LOG_INFO,"%d",n);
            }
            exit(0);
        }
        sleep(2);
    }
    closelog();
}

void stop_daemon()
{
    FILE *f = fopen("hello.txt", "r");
    int pid2;
    fscanf(f, "%d", &pid2);
    fclose(f);
    kill(pid2, SIGTERM);
    printf("Daemon stopped successfully\n");
    syslog(LOG_INFO,"Daemon stopped successfully");
    return;
}

int main(int argv, char *argc[])
{
    if (strcmp(argc[1], "start") == 0)
    {
        start_daemon();
    }
    else
    {
        stop_daemon();
    }
    return 0;
}