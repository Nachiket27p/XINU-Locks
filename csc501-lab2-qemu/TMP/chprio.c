/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}

	// pa2
	// if(pptr->pstate != PRSLEEP && pptr->pstate != PRSUSP && pptr->pstate != PRWAIT) {
	if(pptr->pOrig) {
		if(newprio < pptr->pprio) {
			pptr->pOrig = newprio;
		} else {
			pptr->pprio = newprio;
			pptr->pOrig = 0;
		}
	} else {
		pptr->pprio = newprio;
	}
	// } else {
	// 	kprintf("HERE");
	// 	pptr->pprio = newprio;
	// }
	
	if(pptr->pstate == PRWAIT && pptr->lBlocked &&
				locks[pptr->lBlocked].highPrio < newprio) {
		locks[pptr->lBlocked].highPrio = newprio;
		// perform priority inheritance is the highest priority changes
		prioInheritance(pid);
	}
	// pa2
	restore(ps);
	return(newprio);
}
