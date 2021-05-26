#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"



int
main(int argc, char *argv[])
{
    int k, n, id;
    int a = 0;
    double b = 1;
    if(argc < 2) //if user does not provide a value
        n = 1; 
    else
        n = atoi ( argv[1] ); 
    
    //if user enters a negative value for no. of forks
    //or a number above 20 we default it to 2
    if(n<0 || n>20) {n = 2;}  
    id = 0;
    for ( k = 0; k < n; k++ ) {
        id = fork (); 
        if ( id < 0 ) {
            printf(1, "%d failed in fork!\n", getpid() );
        }else if(id == 0 ) { // child
            printf(1, "Child %d created\n",getpid() );
            //USELESS calculations
            for ( a = 0; a < 1000000000001; a += 1 )
            {
                b+=0.001;
                b = b * 101010101.1 - 0.005 / 10.0;
            }
            //USELESS calculations end
            break;
        }else { //parent
            printf(1, "Parent %d creating child %d\n", getpid(), id );
            wait ();
        }
    }
    exit();
}
