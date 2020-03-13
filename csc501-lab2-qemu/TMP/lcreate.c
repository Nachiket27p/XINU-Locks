#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

LOCAL int newLock();

SYSCALL lcreate()
{
    STATWORD ps;    
	int	loc;

	disable(ps);

    // if((loc = newLock()) == SYSERR ) {
    //     restore(ps);
    //     return(SYSERR);
    // }
    loc = newLock();

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
        loc = nextLock--;

        if(nextLock < 0)
            nextLock = NLOCKS - 1;

        if(locks[loc].lState == LFREE || locks[loc].lState == DELETED) {
            locks[loc].lState = LUSED;
            return(loc);
        }
    }
    return(SYSERR);
}
