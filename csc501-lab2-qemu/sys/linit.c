/* linit.c - linit */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>


int	nextLock;
struct lentry locks[NLOCKS];
struct lentry *lptr;

/*
This function is called from the initialize.c - sysinit() fucntion
to initialize the readers/writers sempahores (locks).
*/
void linit()
{
	nextLock = NLOCKS - 1;
    
    for(i = 0; i < NLOCKS; i++) {
		(lptr = &locks[i])->lstate = LFREE;
		lptr->lReaders = NPROC;
		lptr->lWriters = 1;
		lptr->rQTail = 1 + (sptr->rQHead = newqueue());
		lptr->wQTail = 1 + (sptr->wQHead = newqueue());
	}
}

