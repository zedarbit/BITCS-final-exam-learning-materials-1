#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
	static char buf[2000];
	printf(1,"wolfie sys call returns %d\n",wolfie((void*) buf,2000));

	printf(1,"%s",buf);
	exit();
}
