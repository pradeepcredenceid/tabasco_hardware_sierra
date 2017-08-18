/*************
 *
 * $Id: hptxtask_sdk.c,v 1.21 2010/06/09 23:47:07 epasheva Exp $
 *
 * Filename: hptxtask_sdk.c
 *
 * Purpose:  HIP Transmit task and associated functions
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "mm/mmudefs.h"
#include "sdk/sdkudefs.h"
#include "su/suudefs.h"
#include "us/usudefs.h"
#include "hp/hpidefs.h"

/* Defines */

/* Local Structure Definitions */
/* This next structure is a dummy event block which will
 * be sent to this HP TX task at a fixed interval by the
 * IC package timer task service. 
 */
struct hptxevtblock hptxtimerevt = { HPTXTICK, NULL, 0, 0, NULL, 0};


/* Local storage */

/* HP Tx Control block storage */
struct hptxcb hptxcbk;

/*************
 *
 * Name: hpgettxcbp
 *
 * Purpose: Returns a pointer to the HIP Transmit Task control block
 *          structure.
 *             
 * Parms:   none
 *
 * Return:  pointer to locally defined control block
 *
 * Abort:   none
 *
 * Notes:   None
 *
 **************/
package struct hptxcb *hpgettxcbp( void )
{
    return &hptxcbk;
}

/*************
 *
 * Name: hptxRelEventbk
 *
 * Purpose: To release the resources assocated with the hptx event block,
 *          passed to this task from the CnS task and encapsulated in an 
 *          event block within HP global  entry point
 *             
 * Parms:   eventbkp - Pointer to the hptx Event Block structure, that 
 *                     contains the buffers received from the CnS task
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   This function releases the resources associated with the 
 *          hptx event block as follows: first the memory buffer containing
 *          the newly received hptx packet is released. Next the event buffer
 *          itself is released. Both are allocated from pools created at
 *          system startup time by this HP TX task. 
 *
 **************/
local void hptxRelEventbk( struct hptxevtblock *eventbkp )
{
    /* Free the hptx message buffer first */
    mmbufrel( eventbkp->hptxmemfreep );

    /* Finally, free the event block itself */
    mmbufrel( (swi_uint8 *)eventbkp );
}

/*************
 *
 * Name: hptxtask
 *
 * Purpose: The HIP Transmit task main entry point
 *             
 * Parms:   paramp - pointer to data that may be passed in when this task
 *                   is started.
 *
 * Return:  never returns
 *
 * Abort:   none
 *
 * Notes:   None
 *
 *
 **************/
local void hptxtask( void *paramp)
{
    swi_uint32 hdrsize;
    struct hptxevtblock  *eventbkp; /* Incoming event blk for HP TX task */
    swi_uint32           rmsg;      /* ptr to incoming msg from SMT msg que */
    swi_uint32           droppedpackets;
    swi_uint32           loop;    
    
    /* Define and allocate the task control block */
    struct hptxcb *hptxcbp = hpgettxcbp();

    /* Initialize task structures */
    hptxinit();

    /* Tell SU it's OK to let the next task in line go */
    sutaskinitcb( SDK_HPTXPKG );

    /* Main line of processing begins next */
    for(loop=0;;loop++)
    {
        /* Wait for an event in the shared memory queue 
         * Incoming messages from the HIP receive task are 
         * stored in MM pool buffers. We need to release this
         * storage when we've done with it. 
         * Note, the memory pointed to by rmsg needs 
         * to be released when we're done processing 
         * the incoming packet
         */
        icsmt_rcv( &hptxcbp->hptxicmsgque, HPTXSMTRCVTOUT, &rmsg );
 
        if (rmsg)
        {
            /* Got a proper response */
            eventbkp = (struct hptxevtblock *) rmsg;

            /* Dispatch control based on the incoming event */
            switch (eventbkp->hptxeventtype)
            {
                /* Outgoing packets to modem */
                case HPTXHIP: 
                    /* Construct HIP packet */
                    hdrsize = hpconstruct(eventbkp->hippacketp, 
                                          eventbkp->msgdesignator, 
                                          eventbkp->msgparm, 
                                          eventbkp->paylength, NULL );

                   /* Send the complete HIP packet on its way to the modem, 
                    * passing through the HD package first to frame it. 
                    */
                    if (!hdTxPacket( eventbkp->hippacketp, 
                                    (eventbkp->paylength)+hdrsize, NULL, NULL ))
                    {
                        droppedpackets = hdTxGetDroppedPktCnt();                    
                        dlLog ( &hptxcbp->hptxdluser, DLCLASSALL, 
                                "hdTxPacket dropped packets: %d",
                                (swi_uint32)droppedpackets,
                                (swi_uint32)NULL,
                                (swi_uint32)NULL,
                                (swi_uint32)NULL);
                    }
                    break;  
                 default:
                    /* ignore unknown events */
                    dlLog ( &hptxcbp->hptxdluser, DLCLASSALL,
                            "Ignoring unknown event %x\n", 
                            (swi_uint32)eventbkp->hptxeventtype,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL);
                    break;   
            }
            
            if( eventbkp )
            {
                /* Release the event block and the data buffers  */
                hptxRelEventbk( eventbkp );                    
            }

        }

    } /* for loop */
}

/*************
 *
 * Name: hptxtaskinit
 *
 * Purpose: Creates the HP Transmit task by making it known to the os
 *          package.
 *             
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Must be called *after* hpinit() has been called
 *
 **************/
global void hptxtaskinit( void )
{
    /* Create the HP Transmit task */
    swi_ossdkthreadcreate( hptxtask, NULL );
}

/*
 * $Log: hptxtask_sdk.c,v $
 */

