/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int main()
{
	kprintf("\n\nHello World, Xinu@QEMU lives\n\n");

    /* The hook to shutdown QEMU for process-like execution of XINU.
        * This API call terminates the QEMU process.
        */
    // u_llong a = 0x08888888;//0000 1000 1000 1000 1000 1000 1000 1000

    // int k, tmpp;
    // for(k = 0; k < NPROC; k++) {
    //     if((a >> k) & 1) {
    //         kprintf("%d\t", k);
    //     }
    // }
    // kprintf("\n");

    int a = 10;
    int b = 5;

    int b = b < a ? : a;

    kprintf("%d\n", b);
    
    shutdown();
}


