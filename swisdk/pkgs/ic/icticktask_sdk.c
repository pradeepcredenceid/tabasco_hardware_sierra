/*************
 *
 * $Id: icticktask_sdk.c,v 1.10 2010/06/09 23:49:05 epasheva Exp $
 *
 * Filename:  icticktask_sdk 
 *
 * Purpose:   Contains Time Tick task and associated functions
 *            
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "mm/mmudefs.h"
#include "ql/qludefs.h"
#include "sdk/sdkudefs.h"
#include "su/suudefs.h"
#include "us/usudefs.h"
#include "ic/icidefs.h"

/* Defines */

/* Local Structure Definitions */

/* Local storage */

/* IC Time Tick Task  Control block storage */
struct icttcb icttcbk;


/*************
 *
 * Name:    icgetttcbp
 *
 * Purpose: Returns a pointer to the IC Time Tick Task control block
 *          structure.
 *             
 * Parms:   None
 *
 * Return:  Pointer to locally defined control block
 *
 * Abort:   None
 *
 * Notes:   None
 *
 **************/
package struct icttcb *icgetttcbp( void )
{
    return &icttcbk;
}

/*************
 *
 * Name:    icttinit 
 *
 * Purpose: Initializes the Time Tick Task of the IC package. 
 *
 * Parms:   None
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   Must be called once at initialization time before the 
 *          other tasks start registering for time ticks.
 *
 **************/
local void icttinit( void )
{
    struct icttcb *icttcbp = icgetttcbp();

    /* Initialize waiter structure */ 
    swi_ossdkwaiterinit( &(icttcbp->icttwaiter) );

    /* Initialize semaphore */
    swi_ossdksemaphoreinit( &(icttcbp->icttsem) );

    /* Initialize the head of registration queue */
    qlinitlnk(&(icttcbp->icttregqhp.icttqlink), NULL);
         
    /* Register with the logger process */
    dlregister ("ICTT", &icttcbp->icttdluser, FALSE);

}

/*************
 *
 * Name:    ictttask
 *
 * Purpose: The IC Time Tick task main entry point
 *             
 * Parms:   paramp - pointer to data that may be passed in when this task
 *                   is started. 
 *
 * Return:  never returns
 *
 * Abort:   None
 *
 * Notes:   None
 *
 *
 **************/
local void ictttask( void *paramp )
{
    swi_bool result;
    swi_bool tickloop = TRUE;/* loop variable */
    swi_uint32 loopcounter;  /* Counter for logging/measurement purposes */
    struct icttregqblk *regqblkp = NULL;

    /* Define and allocate the task control block */
    struct icttcb *icttcbp = icgetttcbp();

    /* Initialize task structures */
    icttinit();

    /* Log this task's startup */
    dlLog(  &icttcbp->icttdluser, ICLOG_CLASSA, 
            "IC Tick Task Started",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);

    /* Now that the startup is logged, disable logging for IC */
    dlmasterenable( &icttcbp->icttdluser, FALSE );

    /* Report that the task has been created - let the next task start */
    sutaskinitcb(SDK_ICPKG);

    /* Main line of processing begins next */
    for(loopcounter=0;;loopcounter++)
    {
        /* Wait for tick */
        result = swi_ossdkwaiterwait( &(icttcbp->icttwaiter), ICTTTICK );

        /* Log a message periodically to help with tick timer measurement */
        if( !(loopcounter % ICLOGINTERVAL) ) 
        {
            /* This log will occur once every n passes through this loop
             * which will be useful for measuring the accuracy of the 
             * waiter sleep duration 
             */
            dlLog(  &icttcbp->icttdluser, ICLOG_CLASSA, 
                    "IC Tick Task loop # %d", 
                    (swi_uint32)loopcounter,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
        }


        if (!result) 
        {
            /* On timeout (tick of 100 ms)  go through registration queue and
             *  send the registered tick message to all registered users.
             */
            /* get pointer to first link in the queue */
            regqblkp = (struct icttregqblk *)qlgetnext(
                        &(icttcbp->icttregqhp.icttqlink));
            
            /* loop through queue */
            tickloop = TRUE;
            while(tickloop)
            {
                /* if this is not the header link then send registered tick  */
                if (regqblkp != NULL)
                {
                    icsmt_send( (struct icsmtcb *)(regqblkp->ictticsmtp), 
                        (swi_uint32)(regqblkp->ictttickmsg) );
                    regqblkp = (struct icttregqblk *)qlgetnext(
                               &regqblkp->icttqlink);
                }
                /* completed loop through all links */
                else
                {
                    tickloop = FALSE;
                }
            }
        }
    }/* end main for loop */
}

/*************
 *
 * Name:    ictttaskinit
 *
 * Purpose: Creates the IC Time Tick task by making it known to the os
 *          package.
 *             
 * Parms:   None
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   Must be called *after* icinit() has been called
 *
 **************/
global void ictttaskinit( void )
{
    /* Create the IC Time Tick task */
    swi_ossdkthreadcreate( ictttask, NULL );
}

/*************
 *
 * Name:    ictttickreg 
 *
 * Purpose: Entry point to Time Tick Task of the IC package.
 *          Register callers for time tick notifications.
 *
 * Parms:   icsmtp  -  pointer to intertask message queue
 *          tickmsg -  the message to be send with each tick
 *          callerregblk - caller supplied memory - registration block
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   Callers task must have been initialized.
 *          Caller must assure memory for callerregblk is permanently 
 *          allocated
 *
 **************/                         
global void ictttickreg( 
    struct icsmtcb     *icsmtp, 
    swi_uint32         tickmsg,
    struct icttregqblk *callerregblk )
{
    struct icttregqblk *regblk; /* local copy */
    struct icttcb *icttcbp = icgetttcbp();
    
    regblk = callerregblk;
    
    /* Register the caller for time ticks */
    /* Lock  semaphore */
    swi_ossdksemaphorelock( &(icttcbp->icttsem) );
    
    /* Fill the registration queue block supplied by the caller */
    regblk->ictticsmtp = icsmtp;
    regblk->ictttickmsg = tickmsg;
    
    /* Initialize queue link */
    qlinitlnk(&regblk->icttqlink, regblk);
    
    /* Add the registration to head of queue */
    qladdprev(&(icttcbp->icttregqhp.icttqlink), &regblk->icttqlink);
    
    /* Unlock  semaphore */
    swi_ossdksemaphoreunlock( &(icttcbp->icttsem) );    
}

/*
 * $Log: icticktask_sdk.c,v $
 */

