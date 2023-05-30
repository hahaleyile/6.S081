#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int divisor, num, pid, read_pipe;
    int p[2];
    divisor = 2;
    printf("prime %d\n", divisor);

    pipe(p);
    pid = fork();
    if (pid)
    {
        close(p[0]);
        for (num = 3; num <= 35; num++)
        {
            // maybe little end problem, so write 4 bytes
            write(p[1], &num, 4);
        }
        close(p[1]);
        wait((int *)0);
    }
    else
    {
        read_pipe = dup(p[0]);
        close(p[0]);
        close(p[1]);

        // 不加这个判断最后一个子进程会返回 -1，很奇怪
        // 调试发现是 read_pipe 变成 -1 了
        // 继续调试发现是 dup 失败
        // 并且是 fdalloc 返回了 -1
        // p[0] 是有值的，3，只有 dup 失败了
        // 发现原来是管道到达 fd 数量上限 NOFILE 了，所以无法创建出来
        while (read(read_pipe, &num, 4) == 4)
        {
            // printf("pipe id: %d\n",read_pipe);
            if (num % divisor == 0)
                continue;

            // indicate that this is the last child
            if (pid == 0)
            {
                printf("prime %d\n", num);
                divisor = num;

                pipe(p);
                pid = fork();
                // child process init
                if (pid == 0)
                {
                    // 不加这条会导致 fd 浪费
                    // close(read_pipe);
                    read_pipe = dup(p[0]);

                    close(p[0]);
                    close(p[1]);
                }
                else
                {
                    close(p[0]);
                }
            }
            else
            {
                write(p[1], &num, 4);
            }
        }

        close(read_pipe);
        if (pid)
        {
            close(p[1]);
            wait((int *)0);
        }
    }

    exit(0);
}