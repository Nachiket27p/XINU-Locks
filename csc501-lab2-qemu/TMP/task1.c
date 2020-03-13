#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

#define DEF_LOCK_PRIO 50
#define DEFSEM 70
#define RWLOCK 71

#define assert(x,error) \
    if(!(x)){ \
        kprintf(error);\
        return;\
    }

void timedProcess(char msg, int lck, u_long *time, int syncType)
{
    kprintf ("\tProcess %c to acquire lock, start timer\n", msg);
    *time = ctr1000;
    
    if(syncType == RWLOCK)
        lock (lck, WRITE, DEF_LOCK_PRIO);
    else
        wait(lck);
    
    kprintf ("\tProcess %c acquired lock\n", msg);
    kprintf ("\tProcess %c to release lock, stop timer\n", msg);
    
    if(syncType == RWLOCK)
        releaseall (1, lck);
    else
        signal(lck);

    *time = ctr1000 - (*time);
}

void process(char msg, int lck, int syncType)
{
    kprintf ("\tProcess %c to acquire lock\n", msg);
    
    if(syncType == RWLOCK)
        lock (lck, WRITE, DEF_LOCK_PRIO);
    else
        wait(lck);
    
    kprintf ("\tProcess %c acquired lock, do some work\n", msg);
    int a = MAXINT;
    while(a--);
    kprintf ("\tProcess %c to release lock\n", msg);

    if(syncType == RWLOCK)
        releaseall (1, lck);
    else
        signal(lck);
}

void rdmProcess(char msg)
{
    kprintf("\tProcess %c is doing some work.\n", msg);
    int a = MAXINT;
    while(a--);
    sleep(1);
    a = MAXINT;
    while(a--);
    kprintf("\tProcess %c has completed.\n", msg);
}

u_long prioInh()
{
    int     lck;
    int     p1, p2, p3;

    kprintf("\n>> Priority inheritance sub-test started.\n");

    lck  = lcreate ();
    assert (lck != SYSERR, "!!! Test Failed: lock could not be created. !!!\n");

    u_long time;
    p1 = create(timedProcess, 2000, 18, "A", 4, 'A', lck, &time, RWLOCK);
    p2 = create(process, 2000, 16, "B", 3, 'B', lck, RWLOCK);
    p3 = create(rdmProcess, 2000, 17, "C", 1, 'C');

    kprintf(">> Resume process B with priority = 16\n");
    resume(p2);
    sleep(1);

    kprintf(">> Resume process C with priority = 17\n");
    resume(p3);
    sleep(1);

    kprintf(">> Resume process A with priority = 18\n");
    resume(p1);
    sleep(3);

    kprintf(">> Waiting for processes to finish.\n");
    sleep(30);
    
    kprintf (">> PrioInh Completed\n\n");

    return time;
}

u_long noPrioInh()
{
    int     lck;
    int     p1, p2, p3;

    kprintf("\n>> No priority inheritance sub-test started.\n");
    
    lck  = screate(1);
    assert (lck != SYSERR, "Test Failed: lock could not be created\n");

    u_long time;
    p1 = create(timedProcess, 2000, 18, "A", 4, 'A', lck, &time, DEFSEM);
    p2 = create(process, 2000, 16, "B", 3, 'B', lck, DEFSEM);
    p3 = create(rdmProcess, 2000, 17, "C", 1, 'C');

    kprintf(">> Resume process B with priority = 16\n");
    resume(p2);
    sleep(1);

    kprintf(">> Resume process C with priority = 17\n");
    resume(p3);
    sleep(1);
    
    kprintf(">> Resume process A with priority = 18\n");
    resume(p1);

    kprintf(">> Waiting for processes to finish.\n");
    sleep(30);
    
    kprintf (">> noPrioInh Completed\n\n");

    return time;
}

int main()
{
    char *testMessage = "\nTest Case to show priority inversion probelm & solution:\n\
        Shows the difference between no priority inheritance (semaphore)\n\
        and priority inheritance (readers & writers lock). In this test\n\
        there are three processes A, B, and C. A is the process which\n\
        is held up by B, which acquires the lock before it but has a lower\n\
        priority. Process C does not hold any lock but has a higher priority\n\
        than process B so it gets scheduled more than B. Process A starts a timer\n\
        when it first gets scheduler, than stops the timer when its finished. With\n\
        no priority inheritance we should see a longer time for the process to\n\
        complete compared to with priority inheritance.";

    
    kprintf("%s\n", testMessage);

    u_long npi = noPrioInh();
    u_long pi = prioInh();

    kprintf("Time for Process A to complete without priority inheritance: %lu ms\n", npi);
    kprintf("Time for Process A to complete with priority inheritance   : %lu ms\n", pi);

    return 0;
}
