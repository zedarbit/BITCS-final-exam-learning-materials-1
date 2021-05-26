#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

int main(int argc, char* argv[])
{
    int n;
    pid_t pid;
    clock_t t;
    if(argc < 2)
    {
        printf("ERROR!");
    }
    for(n=0; n<argc; n++)
    {
        printf("arg[%d]:%s\n", n, argv[n]);
    }
    if((pid=vfork())<0)
    {
        printf("fork() failed.\n");
        exit(0);
    }
    else if(pid==0)
    {
        t = clock();
        printf("-[INFO] child process running.\n");
        execlp("./hello",  argv[1], "-1", NULL); 
        exit(1);
    }
    else
    {
        printf("-[INFO] parent process running.\n");
    }
    printf ("-[INFO] It took %f seconds to call %s.\n", ((float)t)/CLOCKS_PER_SEC, argv[1]);
    exit(0); 
    return 0;
}
