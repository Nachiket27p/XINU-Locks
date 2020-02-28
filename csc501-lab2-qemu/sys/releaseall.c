/* releaseall.c - releaseall */

int releaseall(int numlocks, ...)
{
    STATWORD ps;
	register struct	lentry	*lptr;
    struct	pentry	*pptr;

    disable(ps);

    // create array to store variable arguments
    int lockArgs[numlocks]
    // use adres of 'numlocks' variable to obtain other arguments
    unsigned long *arg = (unsigned long *)(&numlocks) + (numlocks);
    int args = 0;
    while(args < numlocks) {
        lockArgs[args++] = *arg;// save the stack variable argument
        arg--;// move pointer to the next stack variable argument
    }

    /* 
    release all the semaphores and check if the requested releases
    are semaphores associated with this process
    */
    int relRtn = OK;
    
    for(args = 0; args < numlocks; args++) {
        lptr = &locks[args];
        pptr = &proctab[currpid];
        if(pptr->lockTrack[args] == READ) {
            lptr->lReaders++;
        } else if(pptr->lockTrack[args] == WRITE) {
            lptr->lWriters++;
        } else {
            relRtn = SYSERR;
        }
        //update tracker to indicate the lock is no longer used by this process
        pptr->lockTrack[args] = LNOUSE;
    }

    disable(ps);
    return(relRtn);
}