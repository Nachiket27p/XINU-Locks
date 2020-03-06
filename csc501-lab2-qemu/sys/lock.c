/* lock.c - lock */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

void prioInheritance();

int lock(int loc, int type, int priority)
{
    // If invalid lock type return SYSERR
    if(type != WRITE && type != READ)
        return(SYSERR);

    STATWORD ps;    
	struct	lentry	*lptr;
	struct	pentry	*pptr;

    disable(ps);// disable interrupts
    lptr = &locks[loc];
    if(BADLOCK(loc) || lptr->lState == LFREE || lptr->lState == DELETED) {
        restore(ps);
        return(SYSERR);
    }
    
    pptr = &proctab[currpid];
    // check if the lock has been deleted by another process
    if((pptr->lDeleted >> loc) & 1) {
        restore(ps);
        return(SYSERR);
    }

    u_long currTime = ctr1000;

    if(type == WRITE) { // write lock
        lptr->lWriters--;
        if(lptr->lState == WRITE || lptr->lState == READ) {
            pptr->lockTime[loc] = currTime; //record the time this process tries to acquire the lock
            insert(currpid, lptr->wQHead, priority);
            pptr->pstate = PRWAIT;
            // keep track of the highest priority process waiting in the queue
            if(pptr->pprio > lptr->highPrio)
                lptr->highPrio = pptr->pprio;
            // update the PCB of lock whose wait queue this process is waiting in
            pptr->lBlocked = loc;
            // check if the process currently holding the lock has a lower priority than
            // the one trying to acquire the lock at the moment
            prioInheritance(currpid);
            // reschedule with the new process priority
            resched();
            // !!! This is where the process will return from resched !!!
        } else {
            lptr->lTracker &= (1 << currpid);
            pptr->lHeld &= (1 << loc);// set mask in pcb to indicate this process is holding the lock
            lptr->lState = WRITE;
        }
    } else { // read lock
        lptr->lReaders--;
        if(lptr->lState == WRITE || (lptr->lState == READ && lastkey(lptr->wQTail) >= priority)) {
            pptr->lockTime[loc] = currTime; //record the time this process tries to acquire the lock
            insert(currpid, lptr->rQHead, priority);
            pptr->pstate = PRWAIT;
            // keep track of the highest priority process waiting in the queue
            if(pptr->pprio > lptr->highPrio)
                lptr->highPrio = pptr->pprio;
            // update the PCB of lock whose wait queue this process is waiting in
            pptr->lBlocked = loc;
            // check if the process currently holding the lock has a lower priority than
            // the one trying to acquire the lock at the moment
            prioInheritance(currpid);
            // reschedule with the new process priority
            resched();
            // !!! This is where the process will return from resched !!!
        } else {
            lptr->lTracker &= (1 << currpid);
            pptr->lHeld &= (1 << loc);// set mask in pcb to indicate this process is holding the lock
            lptr->lState = READ;
        }
    }
    // this is where the process that was put on wait will return from resched
    if((pptr->lDeleted >> loc) & 1) {
        restore(ps);
        return(DELETED);
    }

    restore(ps);
    return(OK);
}

void prioInheritance(int cpid)
{
    struct	pentry	*tmppptr = &proctab[cpid];
    struct	lentry	*tmplptr = &locks[tmppptr->lBlocked];
    int k;
    // k starts from 1 because '0' would mean the null process is holding the lock
    // which is not possible
    for(k = 1; k < NPROC; k++) {
        if(((tmplptr->lTracker >> k) & 1 ) && 
                    (proctab[k].pinh < tmppptr->pprio || proctab[k].pprio < tmppptr->pprio)) {            
            proctab[k].pinh = tmppptr->pprio;
            
            // now if the process with pid k is waiting on a lock than than
            // perform the same operation using recursion to perform priority
            //inheirtance

            prioInheritance(k);
        }
    }
}
