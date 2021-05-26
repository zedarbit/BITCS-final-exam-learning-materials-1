#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int glob = 3;
int main()
{
    pid_t pid;
    int loc = 3;
    printf("before fork, glob=%d, loc=%d.\n", glob, loc);
    // if((pid=vfork())<0) // 1.子进程先运行 2.共享地址空间
    if((pid=fork())<0)
    {
        printf("fork() failed.\n");
        exit(0);
    }
    else if(pid==0)
    {
        glob++;
        loc--;
        printf("child process changes glob and loc\n");
        printf("glob=%d, loc=%d\n", glob, loc);
    }
    else
    {
        printf("parent process doesn’t change glob and loc\n");
        printf("glob=%d, loc=%d\n", glob, loc);
    }
    exit(0); 
}