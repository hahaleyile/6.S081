#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int p[2];
    int buf;
    pipe(p);
    if (fork() == 0)
    {
        read(p[0], &buf, 4);
        if (buf == 'a')
        {
            printf("%d: received ping\n", getpid());
            close(p[0]);
            write(p[1], "b", 4);
            close(p[1]);
        }
    }
    else
    {
        write(p[1], "a", 4);
        close(p[1]);
        read(p[0], &buf, 4);
        if (buf == 'b')
        {
            printf("%d: received pong\n", getpid());
            close(p[0]);
        }
    }

    exit(0);
}
