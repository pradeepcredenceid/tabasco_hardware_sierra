/*************
 *
 * $Id: hprxtask_sdk.c,v 1.17 2010/06/09 23:46:16 epasheva Exp $
 *
 * Filename: hprxtask_sdk.c
 *
 * Purpose:  HIP Receive task and associated functions
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "sdk/sdkudefs.h"
#include "su/suudefs.h"
#include "us/usudefs.h"
#include "hp/hpidefs.h"

/* Defines */

/* Local Structure Definitions */

/* Local storage */
struct hprxcb hprxcbk;

/*************
 *
 * Name:     hpgetrxcbp
 *
 * Purpose: Returns a pointer to the HIP Receive Task control block
 *          structure.
 *             
 * Parms:   None
 *
 * Return:  pointer to locally defined control block
 *
 * Abort:   None
 *
 * Notes:   None
 *
 **************/
package struct hprxcb *hpgetrxcbp( void )
{
    return &hprxcbk;
}

/*************
 *
 * Name:    hprxtask
 *
 * Purpose: The HIP Receive task main entry point
 *             
 * Parms:   paramp - pointer to data that may be passed in when this task
 *                   is started.
 *
 * Return:  never returns
 *
 * Abort:   None
 *
 * Notes:   The HIP Receive task coordinates the reception of HIP 
 *          messages from the modem by dispatching them to the 
 *          registered recipient, if there is one. 
 *
 **************/
local void hprxtask( void *paramp)
{
    swi_uint32 timeout       = 300;    /* Periodically wake up */
    /* Note: The same timeout value is applied to AMIPC  task.
     * Based on our testing  an adequate system response time 
     * is provided with this setup.
     */
    swi_uint32 vendid, prodid;         /* Used to call US to check status */
    swi_bool   result        = FALSE;  /* return status variable */
    swi_size_t resplength; 
    swi_uint8 *inbufp;                 /*  receive buffer pointer */
    struct usbep hpep = {0, 0}; 

    /* Define and allocate the task control block */
    struct hprxcb *hprxcbp   = hpgetrxcbp();

    /* Initialize task structures */
    hprxinit();

    /* Initialize this task's waiter structure */
    swi_ossdkwaiterinit( &hprxcbp->hprxwaiter );

    /* Get an input buffer from the pool */
    inbufp = (swi_uint8 *)mmbufget( &hprxcbp->hpRxPool );

    /* Should never run out of memory */
    if( inbufp == NULL )
    {
        erAbort("hprxtask: no memory", 0 );
    }
    /* Note: It is safe to reuse the same input buffer from the pool 
     * over and over again. We do not recycle it either.
     * We use pool structure for input buffer for future extensions.
     */

    /* Tell SU it's OK to let the next task in line be started */
    sutaskinitcb( SDK_HPRXPKG );

    /* Main line of processing begins next */
    for(;;)
    {
        /* If the USB system is not alive yet */
        if( !usgetinfo( &vendid, &prodid ) )
        {
            /* Log that we're disconnected */
            dlLog(  &hprxcbp->hprxdluser, HPLOG_CLASSA, 
                    "No mdm, pausing for %d ms", 
                    (swi_uint32)HPRXSCANDELAY,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);

            /* Invoke our waiter */
            swi_ossdkwaiterwait( &hprxcbp->hprxwaiter, HPRXSCANDELAY ); 

            /* restart from the top of the loop */
            continue;
        }
        /* Reset this every time through the loop */
        resplength = HPRXMSGSZ;
        slmemset( (char *) inbufp, 0, HPRXMSGSZ );

        if (usgetep (USEP_HIP, &hpep))
            /* Wait for incoming messages from the HIP Receive Endpoint */
            result = usrcvtty( hpep.usreadep, inbufp, &resplength, timeout);

        /* When message received, pass it to the HD layer to start
         * deframing it. 
         */
        if (result == TRUE)
        {
            /* If not a timeout */
            if( resplength )
            {
                /* It is safe to reuse the same buffer (see Note above),
                 * so we do not recycle it.
                 */
                hdRxPacket( inbufp, (swi_uint16)resplength );
            }
        }
        else
        {
            /* Note that if the result was false, it may well be a hardware 
             * failure here or the modem was removed while we were attempting 
             * to read from it.
             * Update dropped packets counter. No logging is attempted here as
             * removing and inserting a modem could result in a blast of dropped
             * packets from the modem
             */
            hprxcbp->hprxpktsdropped++;
        }
    } /* end for loop */
}

/*************
 *
 * Name:    hprxtaskinit
 *
 * Purpose: Creates the HP Receive task by making it known to the os
 *          package.
 *             
 * Parms:   None
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   Must be called *after* hpinit() has been called
 *
 **************/
global void hprxtaskinit( void )
{
    /* Create the HPRX task */
    swi_ossdkthreadcreate( hprxtask, NULL );
}

/*
 * $Log: hprxtask_sdk.c,v $
 */

