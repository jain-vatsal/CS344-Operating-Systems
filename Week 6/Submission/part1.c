// gcc part1.c -o part1
// ./part1
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

struct RowCol
{
    long mtype;
    int row;
    int col;
};

int main()
{
    struct RowCol buf;
    int msqid;
    key_t key;

    if ((key = ftok("part1.c", 'D')) == -1)
    {
        perror("ftok");
        exit(1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) == -1)
    {
        perror("msgget");
        exit(1);
    }

    buf.mtype = 1; /* we don't really care in this case */
    for (int i = 1; i <= 256; i++)
    {
        for (int j = 1; j <= 256; j++)
        {
            buf.row = i;
            buf.col = j;
            if (msgsnd(msqid, (struct msgbuf *)&buf, sizeof(struct RowCol), 0) == -1)
                perror("msgsnd");
        }
    }
    sleep(10);
    if (msgctl(msqid, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
        exit(1);
    }

    return 0;
}