/* releaseall.c - releaseall */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

void newPrioHigh(struct lentry *tlptr);

SYSCALL releaseall(int numlocks, ...)
{
    STATWORD ps;
	register struct	lentry	*lptr;
    struct	pentry	*pptr;
    
    disable(ps);

    // create array to store variable arguments
    int lockArgs[numlocks];
    // use address of 'numlocks' variable to obtain other arguments
    int args = 0;
    while(args < numlocks) {
        lockArgs[args] = *((&numlocks)+ args + 1);// save the stack variable argument
        args++;
    }

    /* 
    release all the semaphores and check if the requested releases
    are semaphores associated with this process
    */
    int relRtn = OK;
    char locStat;
    int rp = 0;
    int wp = 0;
    unsigned long rwt, wwt;
    int locarg;
    pptr = &proctab[currpid];
    int tpid;
    for(args = 0; args < numlocks; args++) {
        rp = 0;
        wp = 0;
        // get the lock descriptor
        locarg = lockArgs[args];
        // get the pointer to the lock
        lptr = &locks[locarg];
        //locStat = pptr->lockTrack[locarg];
        locStat = lptr->lState;
        
        if(locStat == READ) {
            lptr->lReaders++;
            if(lptr->lReaders == NPROC) {
                lptr->lState = LFREE;
            }
        } else if(locStat == WRITE) {
            lptr->lWriters++;
            lptr->lState = LFREE;
        } else {
            relRtn = SYSERR;
            continue;// skip to the next lock
        }

        // remove the inheirted priority for this process
        //pptr->pinh = 0;

        // remove this lock from the lock's tracker by resetting the bit mask for the 
        // appropriate position.
        lptr->pTracker &= (!((u_llong)1 << currpid));

        //update tracker to indicate the lock is no longer used by this process
        pptr->lHeld &= (!((u_llong)1 << locarg));
        
        // if ther is a process waiting to acquire the lock a reader or a writer
        // put that process into the ready queue.
        /* CRITERIA FOR RELEASING PROCESS
            > Lock should be given to the process which has the highest priosity
            > If a reader and a writer have the same priosity:
                # Award the process which has been waiting waiting the longest
                    gets the lockTime in the PCB of the process to determine this
        */
        if(nonempty(lptr->rQHead))
            rp = lastkey(lptr->rQTail);
        if(nonempty(lptr->wQHead))
            wp = lastkey(lptr->wQTail);

        if(rp > wp) {
            lptr->lState = READ;
            // is a reader given the lock let all other readers that have a higher
            // priosity than the highest writer process to access the lock
            while((q[lptr->rQTail].qprev < NPROC) && (rp > wp)) {
                lptr->lReaders--;
                ready(tpid = getlast(lptr->rQTail), RESCHNO);
                lptr->pTracker |= (u_llong)1 << tpid;
                rp = lastkey(lptr->rQTail);
            }
            // recalculate new high priority for this lock
            newPrioHigh(lptr);
            //resched();
        }
        // only give lock to writer if all readers releases the lock
        else if ((wp > rp) && (lptr->lReaders == NPROC)) {

            //kprintf("\nTR: %d    TW: %d\n", rp, wp);
            lptr->lState = WRITE;
            lptr->lWriters--;
            // remove the process from the wait queue, change its priority if
            // ther is a process with a higher process priority waiting in the queues.
            tpid = getlast(lptr->wQTail);

            //kprintf("\nhp: %d \n", lptr->highPrio);

            proctab[tpid].pOrig = proctab[tpid].pprio;
            proctab[tpid].pprio = lptr->highPrio;
            ready(tpid, RESCHNO);
            // kprintf("relall %d %d\n",tpid, locarg);
            lptr->pTracker |= (u_llong)1 << tpid;
            // recalculate new high priority for this lock
            newPrioHigh(lptr);
            //resched();
        }
        // if waiting reader and writer has the same priority choose based on wait time
        else if (rp != 0 && wp != 0 && rp == wp) {
            // get amount of time writer/reader has been waiting for
            rwt = ctr1000 - proctab[q[(lptr->rQTail)].qprev].lockTime;
            wwt = ctr1000 - proctab[q[(lptr->wQTail)].qprev].lockTime;
            
            // give lock to reader if reader has been waiting for longer than
            // writer, additionaly if ther writer has been waiting for less than
            // 0.4s longer than the reader. Give lock to reader.
            // example:
            //          rt = 500; wt = 530
            //          (wt-rt = 30) <= 40    ----> so reader gets lock
            if((wwt - rwt) <= 40) {
                lptr->lState = READ;
                lptr->lReaders--;
                ready(tpid = getlast(lptr->rQTail), RESCHNO);
                lptr->pTracker |= (u_llong)1 << tpid;
            } else {
                lptr->lState = WRITE;
                lptr->lWriters--;
                ready(tpid = getlast(lptr->wQTail), RESCHNO);
                lptr->pTracker |= (u_llong)1 << tpid;
            }
            // recalculate new high priority for this lock
            newPrioHigh(lptr);
            //resched();
        }
        
    }

    // reset process priority to max of all processes waiting in the queues of
    // all the locks this process is still holding
    int i, tmploc;
    int nphigh = 0;
    for(i = 0; i < NLOCKS; i++) {
        //tmploc = pptr->lockTrack[i];
        // check the bitmask lHeld to check which locks this process is holding
        tmploc = ((pptr->lHeld >> i) & (u_llong)1);
        if(tmploc && lptr->highPrio > nphigh) {
            // remember this as the new highest priority
            nphigh = lptr->highPrio;
        }
    }
    
    //pptr->pinh = nphigh;

    // based on the highest priority in the wait queues of all the locks this 
    // process is holding update its priority
    if(pptr->pOrig < nphigh) {
        pptr->pprio = nphigh;
    }
    // If not than restore this processes original priority
    else if(pptr->pOrig) {
        pptr->pprio = pptr->pOrig;
        pptr->pOrig = 0;
    }
    

    restore(ps);
    return(relRtn);
}


void newPrioHigh(struct lentry *tlptr) {
    int curr = firstid(tlptr->rQHead);
    int high = 0;

    while(q[curr].qkey != MAXINT) {
        if(proctab[curr].pprio > high) {
            high = proctab[curr].pprio;
        }
        curr = q[curr].qnext;
    }

    curr = firstid(tlptr->wQHead);
    while(q[curr].qkey != MAXINT) {
        if(proctab[curr].pprio > high) {
            high = proctab[curr].pprio;
        }
        curr = q[curr].qnext;
    }

    tlptr->highPrio = high;
}