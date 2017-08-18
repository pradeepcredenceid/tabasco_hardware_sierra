/*************
 *
 * $Id: hploopback.c,v 1.7 2010/06/09 23:44:49 epasheva Exp $
 *
 * Filename: hploopback.c
 *
 * Purpose:  This file contains all the logic required to implement
 *           HIP loopback requests
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "hp/hpidefs.h"

/*************
 *
 * Name:    hpsetlbtimer
 *
 * Purpose: Start and stop the HIP Loopback timeout timer 
 *             
 * Parms:   timeout  - 0: Stop the timer, 
 *                     n: The timeout to use for the timer in ms
 *
 * Return:  None
 *           
 * Abort:   None
 *
 * Notes:   This function runs inside the HPPH task context. 
 *
 **************/
package void hpsetlbtimer( swi_uint32 timeout )
{
    swi_uint32 ticks = 0;

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Only modify ticks if there is a non-zero timeout */
    if( timeout )
    {
        /* Convert the interval into ticks */
        ticks = (timeout/ICTTTICK) + 1;  /* 1 tick is the minimum */
    }

    /* Write either zero or the timeout in ticks */
    hpphcbp->hpphloopcb.hpphlbtimer = ticks;
}

/*************
 *
 * Name:    hplbclearreq
 *
 * Purpose: Clear the settings for the current loopback request
 *          stored in the loopback control block
 *             
 * Parms:   None
 *
 * Return:  None
 *           
 * Abort:   None
 *
 * Notes:   This function runs inside the HPPH task context. 
 *
 **************/
local void hplbclearreq(void)
{
    /* Pointer to loopback control structure */
    struct hpphloopbk *hploopcbp;

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* initialize the loopback control structure pointer */
    hploopcbp = &hpphcbp->hpphloopcb;

    /* Clear out the entries for the current request */
    hploopcbp->hploopbusy = FALSE;
    hploopcbp->hpphdatap = NULL;
    hploopcbp->hpphdatasize = 0;
    hploopcbp->hpphuser = NULL;
    hploopcbp->hplbrescbfp = NULL;

    /* Stop the loopback timeout timer */
    hpsetlbtimer( 0 );
}

/*************
 *
 * Name:    hplooptimer
 *
 * Purpose: Implements the logic required to service the loopback timer
 *          and to handle when the timer expires. 
 *             
 * Parms:   None
 *
 * Return:  None
 *           
 * Abort:   None
 *
 * Notes:   This function runs inside the HPPH task context.
 *
 **************/
package void hplooptimer( void)
{
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Decrement the timer if it's active */
    if( hpphcbp->hpphloopcb.hpphlbtimer )
    {
        if( --(hpphcbp->hpphloopcb.hpphlbtimer) == 0 )
        {
            /* Handle a timeout condition */
            if( hpphcbp->hpphloopcb.hploopbusy )
            {
                /* If a callback function has been defined */
                if( hpphcbp->hpphloopcb.hplbrescbfp )
                {
                    /* call it */
                    hpphcbp->hpphloopcb.hplbrescbfp( HPTIMEOUT, 
                                                     FALSE,
                                        hpphcbp->hpphloopcb.hpphuser );

                    /* Clear the loopback parameters */
                    hplbclearreq();

                    dlLog(  &hpphcbp->hpphdluser, HPLOG_CLASSA, 
                            "LB timed out awaiting loopback",
                            (swi_uint32)NULL,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL);
                }
                /* Unusual situation, log it */
                else
                {
                    dlLog(  &hpphcbp->hpphdluser, HPLOG_CLASSA, 
                            "LB timeout with no callback",
                            (swi_uint32)NULL,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL);
                }
            /* Timer expired but loopback not busy, do nothing */
            }
            else
            {
                dlLog(  &hpphcbp->hpphdluser, HPLOG_CLASSA, 
                        "LB timeout but no longer busy",
                        (swi_uint32)NULL,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL);
            }
        /* Timer active, but not yet zero, then do nothing */
        }
    /* No timer active, then do nothing */
    }
}

/*************
 * Name:    hpphlbrx
 *
 * Purpose: Callback for HIP Loopback Response packets
 *
 * Parms:   msgid     - The received message ID
 *          msgparm   - The contents of the HIP parameter field
 *          freemem   - Pointer to memory to be reased by caller
 *          payloadp  - Pointer to the payload part of the message
 *          paylength - The length of the payload 
 *          errorflg  - flag for message validity
 *
 * Return:  None
 *
 * Note:    This callback will invoke HPPH task, called from a 
 *          context outside the HPPH task
 *
 *          This functions in-context counterpart is hpphprocloopresp()
 */
package void hpphlbrx(   swi_uint8 msgid, 
                         swi_uint8 msgparm,
                         swi_uint8 *freemem,
                         swi_uint8 *payloadp, 
                         swi_uint16 paylength, 
                         swi_bool errorflg )
{
    struct hpphevtblock  *eventbkp;    /* event block for HPPH task */
    struct hphipevt      *hphipevtp;   /* Pointer to union member */

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Get an event block pointer */
    eventbkp = (struct hpphevtblock *) mmbufget( &hpphcbp->hpphevtpool);

    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        /* Get the union member for this type of event */
        hphipevtp = &eventbkp->hpevtmember.hpmdmpkt;
    
        /* Fill the event with Loopback response message */
        eventbkp->hpphevent          = HPPHLOOPRESP;
        eventbkp->hpphmemfreep       = freemem;   
        hphipevtp->hpphmsgdesignator = msgid;
        hphipevtp->hpphmsgparm       = msgparm;
        hphipevtp->hpphdatap         = payloadp;
        hphipevtp->hpphdatasize      = paylength;
        hphipevtp->hppherrorflg      = errorflg;
        
        /* Send the new event to the HPPH task  for processing */
        icsmt_send( &hpphcbp->hpphicmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("hpphlbrx: Out of memory", 0);
    }
} 

/*************
 *
 * Name:    hplooprequest
 *
 * Purpose: Entry point for other packages to request a loopback test
 *          with the modem. If there isn't already one in progress, 
 *          the request will be accepted and processed immediately. 
 *          The caller will be informed of the outcome later via the
 *          caller-supplied callback function.
 *             
 * Parms:   
 *          hiploopdatap    - pointer to the payload of the loopback message
 *          hiploopsize     - size of the loopback message
 *          responsecallbkp - Caller's callback function. See below
 *          userp           - pointer to user parameter
 *
 * Return:  None
 *           
 * Abort:   None
 *
 * Notes:   This call runs outside the HPPH task context
 *
 *          The prototype of the callback function is as follows:
 *
 *          hplbcbkfp( enum hpphstatus resultcode, 
 *                     swi_bool        runningapp
 *                     swi_uint32      userp
 *                   )
 *
 *          Where:
 *          ======
 *          resultcode: one of those defined by the enumeration. See
 *                      hpudefs.h for more information
 *          runningapp: TRUE  - Modem is running in the application
 *                      FALSE - Modem is running in boot-and-hold
 *          uparm     : The value the caller passed in at the request
 *                      is returned in this argument
 *
 *          This functions in-context counterpart is hpphprocloopreq()
 *
 **************/
global void hplooprequest(
    swi_uint8 *hiploopdatap,
    swi_uint32 hiploopsize,
    void (*hplbcbkfp)(  
         enum hpphstatus resultcode, 
         swi_bool runningapp,
         void *userp),
    void *userp )
{
    struct hpphevtblock  *eventbkp;    /* event block for HPPH task */
    struct hplbreqevt    *hplbreq;     /* Union member of event block */

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Get an event block pointer */
    eventbkp = (struct hpphevtblock *) mmbufget( &hpphcbp->hpphevtpool);
    
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        /* Get a local pointer to the loopback part of the event */
        hplbreq = &eventbkp->hpevtmember.hplbreq;

        /* Fill the event with Loopback request message */
        eventbkp->hpphevent        = HPPHLOOPREQ; 
        eventbkp->hpphmemfreep     = NULL; 
        hplbreq->hpphdatap         = hiploopdatap;
        hplbreq->hpphdatasize      = hiploopsize;
        hplbreq->hplbrescbfp       = (void *)hplbcbkfp;
        hplbreq->hpphuser          = userp;
        
        /* Send the new event to the HP LB task  */
        icsmt_send( &hpphcbp->hpphicmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("hploopreq: Out of memory", 0);
    }
}

/*************
 *
 * Name:    hpphprocloopresp
 *
 * Purpose: Process modem responses to loopback requests
 *             
 * Parms:   hipevtp  - Pointer to the HIP union member of the
 *                     incoming event block 
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This is the callback for loopback response executed 
 *          in the HPPH task's context
 *
 *          Currently this function does nothing to validate that
 *          the response from the modem matches the data sent. If 
 *          this is important we can add it in future. It does 
 *          make sure that the number of bytes received is the 
 *          same as the number of bytes sent
 *
 **************/
package void hpphprocloopresp(struct hphipevt *hipevtp)
{
    enum hpphstatus status;
    swi_bool runningapp;

    /* Local pointer to Loopback control structure */
    struct hpphloopbk *hploopcbp;

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();   
    
    /* Initialize the loopback control structure pointer */
    hploopcbp = &hpphcbp->hpphloopcb;

    /* If there is a registered callback activate the callback */
    if ( hploopcbp->hplbrescbfp != NULL )
    {
        if (hipevtp->hppherrorflg) 
        {
            dlLog(  &hpphcbp->hpphdluser, HPLOG_CLASSA, 
                  "Loopback pkt received with errors, Sent %d bytes, received %d",
                  (swi_uint32)hipevtp->hpphdatasize, 
                  (swi_uint32)hploopcbp->hpphdatasize,
                  (swi_uint32)NULL,
                  (swi_uint32)NULL);

            status = HPFAILED;
        }
        else if(hipevtp->hpphdatasize != hploopcbp->hpphdatasize)
        {
            dlLog(  &hpphcbp->hpphdluser, HPLOG_CLASSA, 
                    "Loopback mismatch: Sent %d bytes, received %d",
                    (swi_uint32)hipevtp->hpphdatasize, 
                    (swi_uint32)hploopcbp->hpphdatasize,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);

            status = HPFAILED;
        }
        else
        {
            status = HPOK;
        }

        /* Initialize the running app value */
        if(hipevtp->hpphmsgparm)
            runningapp = TRUE;
        else
            runningapp = FALSE;

        /* Call the caller-supplied callback routine */
        if( hploopcbp->hplbrescbfp )
        {
            hploopcbp->hplbrescbfp( status, runningapp, hploopcbp->hpphuser );
        }

    }
    /* When we get a loopback response it is fine
     * to clear out the loopback control structure
     * entries
     */
    hplbclearreq();
}

/*************
 *
 * Name:    hpphprocloopreq
 *
 * Purpose: Handles loopback requests received from external packages
 *          
 * Parms:   lbevtp - Pointer to the loopback request data union member of
 *                    the event block containing the caller's loopback req
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This function runs inside the HPPH task context
 *
 **************/
package void hpphprocloopreq(struct hplbreqevt *lbevtp)
{  
    swi_uint8 *bufp;
    swi_uint8 *payloadp; 
    
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Only accept this request if there isn't one already being handled */
    if( !hpphcbp->hpphloopcb.hploopbusy )
    {
        /* Now flag that we are busy and start the timeout timer */
        hpphcbp->hpphloopcb.hploopbusy = TRUE;

        /* Standard timeout */
        hpsetlbtimer( HPLBTIMEOUT );

        /* Register callback to call when response received */
        hpphcbp->hpphloopcb.hplbrescbfp  = lbevtp->hplbrescbfp;
        hpphcbp->hpphloopcb.hpphuser  = lbevtp->hpphuser;
     
        /* Get a HIP transmission buffer */
        bufp = hpgetbuf();
    
        /* Get a pointer to the payload portion */
        payloadp = hpgetpayp( bufp, HPLOOPREQ );

        /* Request the transmission of a HIP loopback request */
        slmemcpy( payloadp, lbevtp->hpphdatap, lbevtp->hpphdatasize );

        /* Save the caller-specified data pointer and length */
        hpphcbp->hpphloopcb.hpphdatap = lbevtp->hpphdatap;
        hpphcbp->hpphloopcb.hpphdatasize = lbevtp->hpphdatasize;

        /* Send the loopback request on its way */
        hpTxPacket( HPLOOPREQ, 0x00, bufp, lbevtp->hpphdatasize );

        dlLog ( &hpphcbp->hpphdluser, DLCLASSALL, 
                "Sent loopback request to modem",
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL);
    }
    else
    {
        /* Already processing another request, tell the caller NO */
        lbevtp->hplbrescbfp( HPLBBUSY, FALSE, lbevtp->hpphuser );
    }
} 

/*************
 *
 * Name:    hplbinit
 *
 * Purpose: Initialize the Loopback subsystem in the HPPH task
 *             
 * Parms:   None
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   Must be called from inside the HPPH Task during system startup
 *
 **************/
package void hplbinit( void )
{
    /* This one call does it all for us */
    hplbclearreq();
}

/*
 * $Log: hploopback.c,v $
 */

