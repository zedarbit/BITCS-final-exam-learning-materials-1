#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"


int
main(int argc, char *argv[])
{
    int n, id;
    double b = 1;
    if(argc < 2) //if user does not provide a value
        n = 1; 
    else
        n = atoi( argv[1] ); 
    
    if(n<0 || n>20) n = 2;

    id = 0;
    
    for (int k=0; k<n; k++) {
        id = fork (); 
        if ( id < 0 ) {
            printf(1, "%d failed!\n", getpid() );
        } else if(id == 0 ) { // child
            printf(1, "child %d created\n",getpid() );
            //dummy calc
            for (int i=0; i<1000000000111; i+=1) {
                b+=0.001;
                b = b * 101010101.1 - 0.005 / 10.0;
            }
            break;
        } else { //parent
            printf(1, "Parent %d creating child %d\n", getpid(), id );
            wait ();
        }
    }
    exit();
}
