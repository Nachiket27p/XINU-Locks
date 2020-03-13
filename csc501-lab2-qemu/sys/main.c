#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

#define DEFAULT_LOCK_PRIO 20

#define assert(x,error) \
    if(!(x)){ \
        kprintf(error);\
        return;\
    }

int mystrncmp(char* des,char* target,int n){
    int i;
    for (i=0;i<n;i++){
        if (target[i] == '.') continue;
        if (des[i] != target[i]) return 1;
    }
    return 0;
}

/*--------------------------------Test 1--------------------------------*/
 
void reader1 (char *msg, int lck)
{
	lock (lck, READ, DEFAULT_LOCK_PRIO);
	kprintf ("  %s: acquired lock, sleep 2s\n", msg);
	sleep (2);
	kprintf ("  %s: to release lock\n", msg);
	releaseall (1, lck);
}

void test1 ()
{
	int	lck;
	int	pid1;
	int	pid2;

	kprintf("\nTest 1: readers can share the rwlock\n");
	lck  = lcreate ();
	assert (lck != SYSERR, "Test 1 failed");

	pid1 = create(reader1, 2000, 20, "reader a", 2, "reader a", lck);
	pid2 = create(reader1, 2000, 20, "reader b", 2, "reader b", lck);

	resume(pid1);
	resume(pid2);
	
	sleep (5);
	ldelete (lck);
	kprintf ("Test 1 ok\n");
}

/*----------------------------------Test 2---------------------------*/
char output2[10];
int count2;
void reader2 (char msg, int lck, int lprio)
{
        int     ret;

        kprintf ("  %c: to acquire lock\n", msg);
        lock (lck, READ, lprio);
        output2[count2++]=msg;
        kprintf ("  %c: acquired lock, sleep 3s\n", msg);
        sleep (3);
        output2[count2++]=msg;
        kprintf ("  %c: to release lock\n", msg);
	releaseall (1, lck);
}

void writer2 (char msg, int lck, int lprio)
{
	kprintf ("  %c: to acquire lock\n", msg);
        lock (lck, WRITE, lprio);
        output2[count2++]=msg;
        kprintf ("  %c: acquired lock, sleep 3s\n", msg);
        sleep (3);
        output2[count2++]=msg;
        kprintf ("  %c: to release lock\n", msg);
        releaseall (1, lck);
}

void test2 ()
{
        count2 = 0;
        int     lck;
        int     rd1, rd2, rd3, rd4;
        int     wr1;

        kprintf("\nTest 2: wait on locks with priority. Expected order of"
		" lock acquisition is: reader A, reader B, reader D, writer C & reader E\n");
        lck  = lcreate ();
        assert (lck != SYSERR, "Test 2 failed");

	rd1 = create(reader2, 2000, 20, "reader2", 3, 'A', lck, 20);
	rd2 = create(reader2, 2000, 20, "reader2", 3, 'B', lck, 30);
	rd3 = create(reader2, 2000, 20, "reader2", 3, 'D', lck, 25);
	rd4 = create(reader2, 2000, 20, "reader2", 3, 'E', lck, 20);
        wr1 = create(writer2, 2000, 20, "writer2", 3, 'C', lck, 28);
	
        kprintf("-start reader A, then sleep 1s. lock granted to reader A\n");
        resume(rd1);
        sleep (1);

        kprintf("-start writer C, then sleep 1s. writer waits for the lock\n");
        resume(wr1);
        sleep (1);


        kprintf("-start reader B, D, E. reader B is granted lock.\n");
        resume (rd2);
	resume (rd3);
	resume (rd4);


        sleep (15);
        kprintf("output=%s\n", output2);
        assert(mystrncmp(output2,"ABABCCDEED",10)==0,"Test 2 FAILED\n");
        kprintf ("Test 2 OK\n");
}

/*----------------------------------Test 3---------------------------*/
void reader3 (char *msg, int lck)
{
        int     ret;

        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, READ, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock\n", msg);
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void writer3 (char *msg, int lck)
{
        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock, sleep 10s\n", msg);
        sleep (10);
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void test3 ()
{
        int     lck;
        int     rd1, rd2;
        int     wr1;

        kprintf("\nTest 3: test the basic priority inheritence\n");
        lck  = lcreate ();
        assert (lck != SYSERR, "Test 3 failed");

        rd1 = create(reader3, 2000, 25, "reader3", 2, "reader A", lck);
        rd2 = create(reader3, 2000, 30, "reader3", 2, "reader B", lck);
        wr1 = create(writer3, 2000, 20, "writer3", 2, "writer", lck);

        kprintf("-start writer, then sleep 1s. lock granted to write (prio 20)\n");
        resume(wr1);
        sleep (1);

        kprintf("-start reader A, then sleep 1s. reader A(prio 25) blocked on the lock\n");
        resume(rd1);
        sleep (1);
        //kprintf("%d",getprio(wr1));
	assert (getprio(wr1) == 25, "Test 3 failed 1");

        kprintf("-start reader B, then sleep 1s. reader B(prio 30) blocked on the lock\n");
        resume (rd2);
	sleep (1);
	assert (getprio(wr1) == 30, "Test 3 failed 2");
	
	kprintf("-kill reader B, then sleep 1s\n");
	kill (rd2);
	sleep (1);
	assert (getprio(wr1) == 25, "Test 3 failed 3");

	kprintf("-kill reader A, then sleep 1s\n");
	kill (rd1);
	sleep(1);
	assert(getprio(wr1) == 20, "Test 3 failed 4");

        sleep (8);
        kprintf ("Test 3 OK\n");
}

void writer4 (char *msg, int lck)
{
        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        kprintf ("  %s: acquired lock, sleep 10s\n", msg);
        sleep (10);
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
}

void test4 ()
{
        int     lck;
        int     w1, w2, w3;

        kprintf("\nTest 4: test the priority inheritence when chprio() called\n");
        lck  = lcreate ();
        assert (lck != SYSERR, "Test 4 failed");

        w1 = create(writer4, 2000, 20, "writer4", 2, "writer A", lck);
        w2 = create(writer4, 2000, 25, "writer4", 2, "writer B", lck);
        w3 = create(writer4, 2000, 30, "writer4", 2, "writer C", lck);

        kprintf("-start writer, then sleep 1s. lock granted to write (prio 20)\n");
        resume(w1);
        sleep (1);

        kprintf("-start reader A, then sleep 1s. reader A(prio 25) blocked on the lock\n");
        resume(w2);
        sleep (1);
        //kprintf("%d",getprio(wr1));
	assert (getprio(w1) == 25, "Test 4 failed 1");

        kprintf("-start reader B, then sleep 1s. reader B(prio 30) blocked on the lock\n");
        resume (w3);
	sleep (1);
	assert (getprio(w1) == 30, "Test 4 failed 2");
        
        // change the priority of the currently running process but nothing should change
        // as it has an inherited priority
        chprio(w1, 27);
        assert (getprio(w1) == 30, "Test 4 failed 3");

        // now change priority higher than inherited priority
        chprio(w1, 35);
        assert (getprio(w1) == 35, "Test 4 failed 4");

        sleep (10);
        kprintf ("Test 4 OK\n");
}

void writer5a (char *msg, int lck)
{
        kprintf ("  %s: to acquire lock\n", msg);
        lock (lck, WRITE, DEFAULT_LOCK_PRIO + 10);
        kprintf ("  %s: acquired lock, sleep 1s\n", msg);
        sleep(3);
        kprintf ("  %s: to release lock\n", msg);
        releaseall (1, lck);
        ldelete(lck);
            
}

void writer5b (char *msg, int lck, int *res)
{
        kprintf ("  %s: to acquire lock\n", msg);
        int lres = lock (lck, WRITE, DEFAULT_LOCK_PRIO + 5);
        if(lres != DELETED) {
            kprintf ("  %s: acquired deleted lock, FAIL!\n", msg);
            *res = 0;
        } else  {
            kprintf ("  %s: could not acquire deleted lock\n", msg);
            *res = 1;
        }
}

void writer5c (char *msg, int lck, int *res)
{
        kprintf ("  %s: to acquire lock\n", msg);
        int lres = lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        if(lres != SYSERR) {
            kprintf ("  %s: acquired deleted lock, FAIL!\n", msg);
            *res = 0;
        } else  {
            kprintf ("  %s: could not acquire deleted lock\n", msg);
            *res = 1;
        }
}

void test5 ()
{
        int lck;
        int w1, w2, w3;
        int res, res1;

        kprintf("\nTest 5: Test trying to acquire deleted lock\n");
        lck  = lcreate ();
        assert (lck != SYSERR, "Test 5 failed");

        w1 = create(writer5a, 2000, 25, "writer5a", 2, "writer 5a", lck);
        w2 = create(writer5b, 2000, 25, "writer5b", 3, "writer 5b", lck, &res);
        w3 = create(writer5c, 2000, 25, "writer5c", 3, "writer 5c", lck, &res1);

        kprintf("-start writer 5a, then sleep 1s\n");
        resume(w1);
        sleep(1);
        kprintf("-start reader 5b, then sleep 1s (%d)\n", w2);
        resume(w2);
        sleep(1);

        sleep (10);
	    assert (res == 1, "Test 5 failed 1");

        resume(w3);
        assert (res1 == 1, "Test 5 failed 2");

        kprintf ("Test 5 OK\n");
}


int main( )
{
    /* These test cases are only used for test purposes.
        * The provided results do not guarantee your correctness.
        * You need to read the PA2 instruction carefully.
        */
	test1();
	test2();
	test3();
    test4();
    test5();

    /* The hook to shutdown QEMU for process-like execution of XINU.
        * This API call exists the QEMU process.
        */
    shutdown();
}


