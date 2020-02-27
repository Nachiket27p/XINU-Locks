/* lock.c - lock */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>

int lock(int loc, int type, int priority)
{
    // If invalid lock type return SYSERR
    if(type != WRITE && type != READ)
        return(SYSERR);

    STATWORD ps;    
	struct	lentry	*lptr;
	struct	pentry	*pptr;

    disable(ps);
    lptr = &locks[loc];
    if(BADLOCK(loc) || lptr->lState == LFREE || lptr->lState == DELETED) {
        restore(ps);
        return(SYSERR);
    }
    
    // update pcb to that the semaphore is requested by the process
    pptr = &proctab[currpid];
    pptr->lockTrack[loc] = LUSI;

    // If there is a write lock than put process into appropriate queue
    // if there are readers currently reading than the writer has to wait
    if(lptr->lWriters <= 0 || (type == WRITE && lptr->lReaders < NPROC)) {
        pptr->pstate = PRWAIT;
        if(type == WRITE) {
            lptr->lWriters--;
            insert(currpid, lptr->wQHead, priority);
        } else {
            lptr->lReaders--;
            insert(currpid, lptr->rQHead, priority);
        }
        resched();
    } else { // no writers --> check if process wants a write or read lock
        if(type == WRITE) {
            lptr->lWriters--;
        } else {
            lptr->lReaders--;
        }
    }

    restore(ps);
    return(OK);
}