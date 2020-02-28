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
    

    //!!!!!!!!!!!!!!!!!!!!!! INCOMPLETE !!!!!!!!!!!!!!!!!!!
    // still need to obtain clarification on how lock should
    // behave based on if a read or write lock is already acquired.
    //!!!!!!!!!!!!!!!!!!!!!! INCOMPLETE !!!!!!!!!!!!!!!!!!!
    pptr = &proctab[currpid];
    // check if the lock has been deleted by another process
    if(pptr->lockTrack[loc] == DELETED) {
        pptr->lockTrack[loc] = LNOUSE;
        restore(ps);
        return(SYSERR);
    }

    if(type == WRITE) { // write lock
        lptr->lWriters--;
        pptr->lockTrack[loc] = WRITE;// update proctable
        //if(lptr->lWriters < 0 || lptr->lReaders < NPROC) {
        if(lptr->lState == WRITE || lptr->lState == READ)
            insert(currpid, lptr->wQHead, priority);
            pptr->pstate = PRWAIT;
            resched();
            // this is where the process that was put on wait will return from resched
            if(pptr->lockTrack[loc] == DELETED) {
                restore(ps);
                return(DELETED);
            }
        } else {
            lptr->lState = WRITE;
        }
    } else { // read lock
        lptr->lReaders--;
        pptr->lockTrack[loc] = READ;// update proctable
        //if(lptr->lWriters <= 0) {
        if(lptr->lState == WRITE || (lptr->lState == READ && lastkey(lptr->wQTail) >= priority))
            insert(currpid, lptr->rQHead, priority);
            pptr->pstate = PRWAIT;
            resched();
            // this is where the process that was put on wait will return from resched
            if(pptr->lockTrack[loc] == DELETED) {
                restore(ps);
                return(DELETED);
            }
        } else {
            lptr->lState = READ;
        }
    }

    restore(ps);
    return(OK);
}