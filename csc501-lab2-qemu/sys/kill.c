/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	int templok;
	switch (pptr->pstate) {

		case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			// pa2
			// release all the locks this process was holding
			for(templok = 0; templok < NLOCKS; templok++) {
				if((pptr->lHeld >> templok) & 1) {
					releaseall(1, templok);
				}
			}
			//pptr->lROrW = 0;
			pptr->pOrig = 0;
			pptr->lBlocked = -1;
			pptr->lDeleted = 0;
			pptr->lHeld = 0;
			// pa2
			resched();

		case PRWAIT:	semaph[pptr->psem].semcnt++;
			// pa2
			if(pptr->lBlocked) {
				dequeue(pid);

				// if the process being is the one with the highest priority
				// than update the highest wait process priority for the lock
				// also update the priority of all the processes currently
				// holding this lock
				if(locks[pptr->lBlocked].highPrio == pptr->pprio) {
					struct lentry *lptr = &locks[pptr->lBlocked];
					// check if the highest prioritu needs to be reset
					newPrioHigh(lptr);
					// update the priority of all the processes currently using this lock
					// check if it holding a lock with inheirited priority
					// for all processes holding this lock
						// check all other locks its holding
							// check the lptr->highPrio and change the priority accordingly
					int k, x;
					struct pentry *tpptr;
					for(k = 0; k < NPROC; k++) {
						if((lptr->pTracker >> k) & (u_llong)1) {
							tpptr = &proctab[k];
							if(lptr->highPrio == 0) {
								tpptr->pprio = tpptr->pOrig;
								tpptr->pOrig = 0;
							} else {
								tpptr->pprio = lptr->highPrio;
							}

							// not check if its inheriting from other locks its holding
							for(x = 0; x < NLOCKS; x++) {
								if(((tpptr->lHeld >> x) * (u_llong)1) && (locks[x].highPrio > tpptr->pprio)) {
									tpptr->pprio = locks[x].highPrio;
								}
							}
						}
					}
				}
			}
			
			// int rlk = lastkey(locks[pptr->lBlocked].rQTail);
			// int wlk = lastkey(locks[pptr->lBlocked].wQTail);
			// locks[pptr->lBlocked].highPrio = rlk > wlk ? rlk : wlk;
			//pptr->lROrW = 0;
			pptr->pOrig = 0;
			pptr->lBlocked = -1;
			pptr->lDeleted = 0;
			pptr->lHeld = 0;
			// pa2

		case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			// !!! TODO
			for(templok = 0; templok < NLOCKS; templok++) {
				if((pptr->lHeld >> templok) & 1) {
					releaseall(1, templok);
				}
			}
			pptr->pOrig = 0;
			pptr->lBlocked = -1;
			pptr->lDeleted = 0;
			pptr->lHeld = 0;
			// !!! TODO
			break;

		case PRSLEEP:
		case PRTRECV:	unsleep(pid);
						/* fall through	*/
		default:	pptr->pstate = PRFREE;
	}

	
	restore(ps);
	return(OK);
}
