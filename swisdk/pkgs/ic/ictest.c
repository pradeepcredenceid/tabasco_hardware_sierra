/*
 *  $Id: ictest.c,v 1.3 2009/06/01 23:27:40 blee Exp $
 *
 *  Filename: ictest.c
 *
 *  Purpose:  Contains code to test the IC package
 *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
 *
 * Note:      This file contains code that tests the IC package.
 *            The test is simple: create 2 threads, have each 
 *            thread send the other a message while the other 
 *            waits for it. Thread 1 sends first while thread 2
 *            listens, then they swap. The two threads output 
 *            their status to a log file in /var/log/xxx, where
 *            xxx depends upon the Distro of Linux you are using. 
 *            For Ubuntu, xxx is user.log. 
 *
 *            When you dump the log you should see alternating 
 *            reports from each thread. Thread 2 should report
 *            first then thread1. This repeats until the main()
 *            for loop times out on the waiter.
 *
 *            If you ^C or signal the program from a shell, you
 *            will see an abort in the log as well. This is normal
 *            behaviour.
 *
 *            At the moment this tests only the SMT mechanism, not
 *            the IPC mechanism that is also part of this package
 *
 *            If you don't touch the times in the IC calls in the
 *            threadx loops, you should see successive logs from 
 *            each thread every 4 seconds.
 */

/* Linux definitions */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/* include files */
#include "os/swi_ossdk.h"
#include "ic/icudefs.h"
#include "aa/aaglobal.h"

char *th1msg = "1111 Message from Thread1 1111";
char *th2msg = "2222 Message from Thread2 2222";

/* 
 * Name: 
 *
 * Purpose: 
 *
 * Members: 
 *
 * Note:    None
 *
 */

struct swi_oswaiter mainwaiter;

/* Two message queues, one for each thread */
struct icsmtcb th1tcb;
struct swi_oswaiter th1waiter;

struct icsmtcb th2tcb;
struct swi_oswaiter th2waiter;

/**************
 *
 * Name:     thread1
 *
 * Purpose:  Thread number 1 main loop. This thread sends a message to
 *           thread number 2, then waits for a response. This cycle 
 *           repeats every 4 seconds or so.
 *           
 * Parms:   paramp - pointer to data that may be passed in when this task
 *                   is started.
 *
 * Return:   never returns
 *
 * Notes:    none
 *
 **************/
local void thread1( void *paramp)
{
    char logmessage[256];
    swi_uint32 mesg;  /* To receive the incoming message */

    /* Init the OS structures */
    icsmt_create( &th1tcb, "Th1", 3 );
    swi_ossdkwaiterinit( &th1waiter );

    /* Initially, no messages received */
    mesg = 0;

    /* Log that we started */
    swi_ossdklog( "Thread1 started" );

    /* Infinite loop */
    for( ;; )
    {
        /* Wait 2 seconds */
        swi_ossdkwaiterwait( &th1waiter, 2000 );

        /* Send to thread 2 */
        icsmt_send( &th2tcb, (swi_uint32) th1msg );

        /* Wait for message from thread 2 - infinite timeout */
        icsmt_rcv( &th1tcb, 0, &mesg );

        if( mesg )
        {
            snprintf(logmessage, sizeof( logmessage ), 
                     "THREAD1 Rx'd: %s", (char *) mesg );

            swi_ossdklog( logmessage );
        }

        swi_ossdklog( "Thread1 looping" );
    }

}

/**************
 *
 * Name:     thread2
 *
 * Purpose:  Thread number 2 main loop. This thread waits for a message
 *           from thread number 1 and then sends it a response.
 *           
 * Parms:    paramp - pointer to data that may be passed in when this task
 *                   is started.
 *
 * Return:   never returns
 *
 * Notes:    none
 *
 **************/
local void thread2( void *paramp)
{
    char logmessage[256];
    swi_uint32 mesg;  /* To receive the incoming message */

    /* Initialize the OS structures */
    icsmt_create( &th2tcb, "Th2", 3 );
    swi_ossdkwaiterinit( &th2waiter );

    /* Initially, no messages received */
    mesg = 0;

    /* Log that this thread is started */
    swi_ossdklog( "Thread2 started" );

    /* Infinite loop */
    for( ; ; )
    {
        /* Wait for a message from thread 1 - infinite timeout */
        if( icsmt_rcv( &th2tcb, 0, &mesg ) )
        {
            if( mesg )
            {
                snprintf(logmessage, sizeof( logmessage ), 
                         "THREAD2 Rx'd: %s", (char *) mesg );
    
                swi_ossdklog( logmessage );
            }
            else
                swi_ossdklog("th2: no message rx'd" );
        }
        else
            swi_ossdklog("th2: false ret'd from _rcv" );

        /* Wait 2 seconds before sending a response */
        if( swi_ossdkwaiterwait( &th2waiter, 2000 ) )
            swi_ossdklog("Th2 waiter didn't time out" );

        icsmt_send( &th1tcb, (swi_uint32) th2msg );

        swi_ossdklog( "Thread2 looping" );
    }
}

/**************
 *
 * Name:     main
 *
 * Purpose:  Entry point where testing of the IC package starts. 
 *           
 * Parms:    argc   - Not used
 *           argv[] - Not used
 * Return:   0 - Normal termination
 *           1 - Problem with the execution
 *
 * Notes:    The test involves creating two threads and then sending 
 *           messages between them and verifying that those messages
 *           were received properly.
 *
 **************/
int main( int argc, char *argv[] )
{

    /* Initialize the SDK process - We use it for our test environment */
    swi_ossdkprocessinit(); /* Must be called first */

    /* Create the threads */
    swi_ossdkthreadcreate( thread1, NULL );
    swi_ossdkthreadcreate( thread2, NULL );

    swi_ossdkwaiterinit( &mainwaiter );
    
    for( ;; )
    {
        /* Wait here, we need to give the threads
         * time to do the work. Timeout value gives
         * the threads a lot of time to ping-pong 
         * their messages back and forth.
         */
        swi_ossdkwaiterwait( &mainwaiter, 20000 );

        /* With a timeout of zero, we should never return */
        return 0;
    }

    /* When this thread ends, the program ends */
}

/*
 * $Log: ictest.c,v $
 */
