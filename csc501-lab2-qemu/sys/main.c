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
        
        //releaseall(3, 10, 15, 20);

        shutdown();
}
