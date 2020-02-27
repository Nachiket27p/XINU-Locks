/* linit.c - linit */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>


int	nextlock;
struct lentry locks[NLOCKS];
struct lentry *lptr;

/*
This function is called from the initialize.c - sysinit() fucntion
to initialize the readers/writers sempahores (locks).
*/
void linit()
{
	nextlock = NLOCKS - 1;
    
    for(i = 0; i < NLOCKS; i++) {
		(lptr = &locks[i])->lstate = LFREE;
		lptr->lreaders = 0;
		lptr->lwriters = 0;
		lptr->lqtail = 1 + (sptr->lqhead = newqueue());
	}
}

