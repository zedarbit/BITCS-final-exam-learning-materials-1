#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  if(argc<2){
    printf(1, "Expected 2 arguments, got %d\n", argc);
    exit();
  }
  int f=fork();
  if(f==0){
      exec(argv[1], argv+1);
  }else{
    int status, wtime, rtime;
    status=waitx(&wtime, &rtime);
    // 1 to print to stdout
    printf(1, "\nPID: %d\nTotal Time=%d\nRun Time=%d\nWait Time=%d\n", status, rtime+wtime, rtime, wtime);
  }
  exit();
}
