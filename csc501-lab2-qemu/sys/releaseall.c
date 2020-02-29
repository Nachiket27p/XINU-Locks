/* releaseall.c - releaseall */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

int releaseall(int numlocks, ...)
{
    STATWORD ps;
	register struct	lentry	*lptr;
    struct	pentry	*pptr;
    
    disable(ps);

    // create array to store variable arguments
    int lockArgs[numlocks];
    // use adres of 'numlocks' variable to obtain other arguments
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
    char locVal;
    int rp = 0;
    int wp = 0;
    int rwt, wwt;
    unsigned int cTime;
    for(args = 0; args < numlocks; args++) {
        lptr = &locks[args];
        pptr = &proctab[currpid];
        locVal = pptr->lockTrack[args];

        if(locVal == READ)
            lptr->lReaders++;
        else if(locVal == WRITE)
            lptr->lWriters++;
        else
            relRtn = SYSERR;

        //update tracker to indicate the lock is no longer used by this process
        pptr->lockTrack[args] = LNOUSE;
        
        // if a lock is released than
        if(locVal == READ || locVal == WRITE) {
            // get the current time
            cTime = ctr1000;
            // if ther is a process waiting to acquire the lock a reader or a writer
            // put that process into the ready queue.
            /* CRITERIA FOR RELEASING PROCESS
                > Lock should be given to the process which has the highest priosity
                > If a reader and a writer have the same priosity:
                    # Award the process which has been waiting waiting the longest
                        uese the lockTime[] in the PCB of the process to determine this
            */
            if(nonempty(lptr->rQHead))
                rp = lastkey(lptr->rQTail);
            if(nonempty(lptr->wQHead))
                wp = lastkey(lptr->wQTail);

            if(rp > wp) {
                // is a reader given the lock let all other readers that have a higher
                // priosity than the highest writer process to access the lock
                while((q[lptr->rQTail].qprev < NPROC) && (rp > wp)) {
                    lptr->lReaders--;
                    ready(getlast(lptr->rQTail), RESCHNO);
                    rp = lastkey(lptr->rQTail);
                }
                resched();
            } else if ((wp > rp) && (lptr->lReaders == NPROC)) {// only give lock to writer if all readers releases the lock
                lptr->lWriters--;
                ready(getlast(lptr->wQTail), RESCHNO);
                resched();
            } else if (!rp && !rp) { // if waiting reader and writer has the same priority choose based on wait time
                // get amount of time writer/reader has been waiting for
                rwt = cTime - proctab[q[lptr->rQTail].qprev].lockTime[args];
                wwt = cTime - proctab[q[lptr->wQTail].qprev].lockTime[args];
                
                // give lock to reader if reader has been waiting for longer than
                // writer, additionaly if ther writer has been waiting for less than
                // 0.4s longer than the reader. Give lock to reader.
                // example:
                //          rt = 5000; wt = 5300
                //          (wt-rt = 300) < 400    ----> so reader gets lock
                if((rwt > wwt) && ((wwt - rwt) < 400)) {
                    lptr->lReaders--;
                    ready(getlast(lptr->rQTail), RESCHNO);
                } else {
                    lptr->lWriters--;
                    ready(getlast(lptr->wQTail), RESCHNO);
                }
                resched();
            }
        }
    }

    restore(ps);
    return(relRtn);
}