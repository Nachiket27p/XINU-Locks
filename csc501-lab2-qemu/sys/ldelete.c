/* ldelete.c - ldelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

int ldelete(int loc)
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
            proctab[pid].ldelete &= (1 << loc); 
		    ready(pid,RESCHNO);
		}
		resched();
	}
    
    // check the writers queue for waiting processes
    if(nonempty(lptr->wQHead)) {
        while( (pid=getfirst(lptr->wQHead)) != EMPTY)
		{
		    // set mask in pcb that this lock was deleted
            proctab[pid].ldelete &= (1 << loc);
		    ready(pid,RESCHNO);
		}
        resched();
    }
    
    restore(ps);
    return(OK);
}