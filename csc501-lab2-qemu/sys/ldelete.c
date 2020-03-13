/* ldelete.c - ldelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

SYSCALL ldelete(int loc)
{
    STATWORD ps;
    int pid;
    struct lentry *lptr;

    disable(ps);
    lptr = &locks[loc];
    if(BADLOCK(loc) || lptr->lState == LFREE || lptr->lState == DELETED) {
        restore(ps);
        return(SYSERR);
    }
    
    lptr->lState = DELETED;

    // check the reqders queue for waiting processes
    if (nonempty(lptr->rQHead)) {
		while( (pid=getfirst(lptr->rQHead)) != EMPTY)
		{
            // set mask in pcb that this lock was deleted
            proctab[pid].lDeleted |= (u_llong)1 << loc; 
		    ready(pid,RESCHNO);
		}
		resched();
	}
    
    // check the writers queue for waiting processes
    if(nonempty(lptr->wQHead)) {
        while( (pid=getfirst(lptr->wQHead)) != EMPTY)
		{
		    // set mask in pcb that this lock was deleted
            proctab[pid].lDeleted |= (u_llong)1 << loc;
            //kprintf("%d\n", pid);
		    ready(pid,RESCHNO);
		}
        resched();
    }
    
    // check the lock tracker for current processes holding this lock
    // inform them through pcb that this lock has beeen deleted
    int k;
    for(k = 1; k < NPROC; k++) {
        if((locks[loc].pTracker >> k) & (u_llong)1) {
            // kprintf("ldel %d %d\n", k, loc);
            proctab[k].lDeleted |= (u_llong)1 << loc;
        }
    }

    
    restore(ps);
    return(OK);
}