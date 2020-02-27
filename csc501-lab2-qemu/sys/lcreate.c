#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>

int lcreate()
{
    STATWORD ps;    
	int	loc;

	disable(ps);

    if(count < 0 || (loc = newLock() == SYSERR ) {
        restore(ps);
        return(SYSERR);
    }

    restore(ps);
    return(loc);
}

/*
Used to create a find an available lock and return it.
If there are no locks avaialbe than return SYSERR.
*/
LOCAL int newLock()
{
    int loc;
    int i = NLOCKS;
    while(i) {
        i--;

        if(nextLock < 0)
            nextLock = NLOCKS - 1;

        if(locks[loc].lState == LFREE || locks[loc].lState == DELETED) {
            locks[loc].lState = LUSED;
            return(loc);
        }
    }
    return(SYSERR);
}
