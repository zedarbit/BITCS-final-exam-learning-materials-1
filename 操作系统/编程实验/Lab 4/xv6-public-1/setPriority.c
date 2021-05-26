#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  if(argc<3 || argc>3){
    printf(1, "Expected %d arguments. Got %d\n", 2, argc-1);
    exit();
  }
  int newPriority=atoi(argv[1]);
  int pid=atoi(argv[2]);
  int status=set_priority(newPriority, pid);
  if(status==-1){
    if(newPriority>100 || newPriority<0){
      printf(1, "Priority can be set only between 1 and 100. got %d\n", newPriority);
      printf(1, "Aborting...\n");
    }else{
      printf(1, "PID Doesn't exist\n");
      printf(1, "Aborting...\n");
    }
    exit();
  }
  exit();
}
