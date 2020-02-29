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
    int i;
    for(i = 0; i < NLOCKS; i++) {
		(lptr = &locks[i])->lState = LFREE;
		lptr->lReaders = NPROC;
		lptr->lWriters = 1;
		lptr->rQTail = 1 + (lptr->rQHead = newqueue());
		lptr->wQTail = 1 + (lptr->wQHead = newqueue());
	}
}

