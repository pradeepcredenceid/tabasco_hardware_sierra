/*************
 *
 * Filename: amtimeout_sdk.c
 *
 * Purpose:  AM package Timeout task and associated functions
 *
 * Copyright: © 2012-2013 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "ci/ciudefs.h"
#include "sdk/sdkudefs.h"
#include "su/suudefs.h"
#include "qm/qmerrno.h"
#include "am/amidefs.h"
#include <stdio.h>

/* Defines */
#define AMTICKMS 100

/* A dummy event block sent from the IC package timer task service to the QM
 * task at a fixed interval.
 */
local enum amtimeouttype amtimerevt = AM_TMRTICK_EVT;

/*************
 *
 * Name: amreqtimeout
 *
 * Purpose: Called if the ammdmtmrwait timer expires, meaning that the
 *          modem has not responded to an outstanding QMI request
 *          in the time specified by the caller of the API. Send back
 *          a timeout indication to the API side
 *
 * Parms:   eventbkp - Pointer to the AM event block structure
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void
amreqtimeout(
    struct amevtblock *qeventbkp)
{
    swi_uint16 amlength, amtype, amhdrversion;
    enum eQCWWANError amresultcode;
    /* Define and allocate the task control block */
    struct amipccb *amipccbp = amgetipccbp();
    /* Storage for incoming IPC packet buffer pointer */
    swi_uint8 *inipcmsgp = qeventbkp->amdatap;

    dlLog1 ( &amipccbp->amdlcb, DLCLASSALL, "AM request timeout %p\n",
                (swi_uint32)qeventbkp);

    /* Parse received packet header */
    amparseamheader( &amtype,
            &amhdrversion,
            &amresultcode,
            &amlength,
            &inipcmsgp);

    /* Send a packet back to the caller */
    amShortResp( amtype,
            eQCWWAN_ERR_SWICM_TIMEOUT,
            qeventbkp->amipcchannel );
}

/*************
 *
 * Name:    amtick
 *
 * Purpose: Called once each time a timer tick message is received
 *          by the AM timeout task. This function takes care of managing
 *          internal timers, decrementing them for each call. When
 *          the timer expires (reaches zero) the expiry function
 *          is called
 *
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void
amtick(void)
{
    struct amipccb *amipccbp = amgetipccbp();
    struct qllink *curlinkp;
    struct amevtblock *qeventbkp = NULL;

#ifdef ICTTTICK_DEBUG
    ictttickdbg();
#endif

    amQueueLock();

    curlinkp = &(amipccbp->ameventheadq);

    /* point to the first link in the queue */
    qeventbkp = (struct amevtblock *)qlgetnext(curlinkp);

    while(qeventbkp)
    {

        if( qeventbkp->ammdmwaittmr && (0 == --qeventbkp->ammdmwaittmr) )
        {
           /* the timer is active and has just timed out */
            amreqtimeout(qeventbkp);

            /* remove the node from link list */
            qlremcur(&qeventbkp->ameqlnk);

            /* Free up incoming blocks */
            amreleventbk (qeventbkp);
        }
        qeventbkp = (struct amevtblock *)qlgetnext( 
                &qeventbkp->ameqlnk );
    }
    amQueueUnlock();
}

/*************
 *
 * Name: amtimeouttask
 *
 * Purpose: Implements the main part of the timeout task. This task
 *          exists solely to block waiting for timer tick message and
 *          then timed out request in AM queue
 *
 * Parms:   paramp - pointer to data that may be passed in when this task
 *                   is started.
 *
 * Return:  never returns
 *
 * Abort:   none
 *
 * Notes:   This task require concurrent access to AM queue
 *          Thus locking is necessary
 *
 **************/
void amtimeouttask( void *paramp )
{
    struct icsmtcb      amtmrmsgque;
    struct icttregqblk  amtmrthndl;
    /* Define and allocate the task control block */
    struct amipccb *amipccbp = amgetipccbp();

    UNUSEDPARAM( paramp );

     /* Tell the world we've started up */
    dlLog0 ( &amipccbp->amdlcb, DLCLASSALL, "Timeout Task started\n" );

    /* Initialize the SMT Requests message Q */
    icsmt_create( &amtmrmsgque, "AMTIMEOUT", AMTMRDEPTH );

    /* Register with the IC package to receive periodic timer ticks */
    ictttickreg( &amtmrmsgque,
                 (swi_uint32)&amtimerevt,
                 &amtmrthndl,
                 AMTICKMS );

    /* Main timeout task loop */
    swi_uint32 loop;
    for(loop=0;;loop++)
    {
        /* Wait to receive  a timer message */
        swi_uint32 reqmsg;
        icsmt_rcv( &amtmrmsgque, AM_TOUT_FOREVER, &reqmsg );
        if(reqmsg)
        {
            enum amtimeouttype *type = (enum amtimeouttype *) reqmsg;
            switch( *type )
            {
                case AM_TMRTICK_EVT:
                    /* Periodic timer tick message */
                    amtick();
                default:
                    break;
            }
        }
    }
}

/*************
 *
 * Name: amupdatetimeout
 *
 * Purpose: update timeout remaining for QM layer to process
 *
 * Parms:   amqmipacketp[IN]     - pointer to the AM QMI Request packet
 *          remaining ticks[IN]  - timeout remaining in number of AM ticks
 *
 * Return:  none
 *
 * Abort:   none
 *
 **************/
void amupdatetimeout(
        swi_uint8   *amqmipacketp,
        swi_uint32  remaining_ticks )
{
    /* Define and allocate the task control block */
    struct amipccb *amipccbp = amgetipccbp();

    /* skip amqmisvctype */
    amqmipacketp++;

    dlLog2 ( &amipccbp->amdlcb, DLCLASSALL,
            "timeout org/mod: %d/%d",
            piget32_noinc(&amqmipacketp),
            remaining_ticks * AMTICKMS);

    /* update amqmireqtimeout */
    piput32(remaining_ticks * AMTICKMS, &amqmipacketp);
}
/*************
 *
 * Name:    amreqinittmr
 *
 * Notes:   This function starts the AM request timer using
 *          the timeout value specified by the caller in the API
 *
 * Parms:   eventbkp - Pointer to the AM event block structure
 *
 * Return:  none
 *
 * Abort:   Stop and wait violation
 *
 *
 **************/
void
amreqinittmr( struct amevtblock *eventbkp)
{
    struct amqmirrparms qmparm;
    swi_uint16 amlength, amtype, amhdrversion;
    enum eQCWWANError amresultcode;
    /* Define and allocate the task control block */
    struct amipccb *amipccbp = amgetipccbp();
    /* Storage for incoming IPC packet buffer pointer */
    swi_uint8 *inipcmsgp = eventbkp->amdatap;

    /* Parse received packet header */
    amparseamheader( &amtype,
            &amhdrversion,
            &amresultcode,
            &amlength,
            &inipcmsgp);

    amparseqmixactionheader( &inipcmsgp, &qmparm );

    if( 0 == qmparm.amqmireqtimeout ) 
    {
        char errmsg[100];
        snprintf(errmsg, sizeof(errmsg),
                 "%s:%d zero timeout not supported",
                 (char *)__func__, __LINE__);
        erAbort( errmsg, qmparm.amqmireqtimeout );
    }

    /* number of ticks to use for the given timeout (one tick more to cover worst case) */
    swi_uint32 ticks = (qmparm.amqmireqtimeout / AMTICKMS) + 1;

    dlLog1( &amipccbp->amdlcb, DLCLASSALL,
            "AM request init timer as %d ticks\n",
            (unsigned int)ticks);

    /* Start the timer */
    eventbkp->ammdmwaittmr = ticks;
}
