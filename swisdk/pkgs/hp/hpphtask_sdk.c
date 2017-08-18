/*************
 *
 * $Id: hpphtask_sdk.c,v 1.16 2010/06/09 23:45:20 epasheva Exp $
 *
 * Filename:  hpphtask_sdk 
 *
 * Purpose:   Contains Packet Handler task and associated functions
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

/* Local storage */
struct hpphevtblock hpphtickevt;

/* HP Packet Handler Task  Control block storage */
struct hpphcb hpphcbk;

/*************
 *
 * Name:    hpgetphcbp
 *
 * Purpose: Returns a pointer to the HP Packet Handler Task control block
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
package struct hpphcb *hpgetphcbp( void )
{
    return &hpphcbk;
}

/*************
 * Name:    hpdirhipcb
 *
 * Purpose: Callback for receipt of a HIP Device Information Response message
 *
 * Parms:   msgdesignator - The received message ID
 *          msgparm       - The contents of the HIP parameter field
 *          freemem       - Pointer to memory to be freed
 *          payloadp      - Pointer to the payload part of the message
 *          paylength     - The length of the payload 
 *          errorflg      - Error status from caller 
 *
 * Return:  None
 *
 * Note:    This callback will invoke HPPH task 
 *
 */
local void hpdirhipcb ( swi_uint32 msgdesignator, 
                        swi_uint8  msgparm,
                        swi_uint8  *freemem,
                        swi_uint8  *payloadp, 
                        swi_uint16 paylength, 
                        swi_bool   errorflg )
{
    struct hpphevtblock  *eventbkp;    /* event block for HPPH task */
    struct hphipevt      *hipevtp;     /* HIP union member of event block */

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Get an event block pointer */
    eventbkp = (struct hpphevtblock *) mmbufget( &hpphcbp->hpphevtpool);
    
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        hipevtp = &eventbkp->hpevtmember.hpmdmpkt;
        /* Fill the event with response message */
        eventbkp->hpphevent        = HPPHDIRRESP;
        eventbkp->hpphmemfreep     = freemem;   
        hipevtp->hpphmsgdesignator = msgdesignator;
        hipevtp->hpphmsgparm       = msgparm;
        hipevtp->hpphdatap         = payloadp;
        hipevtp->hpphdatasize      = paylength;
        hipevtp->hppherrorflg      = errorflg;
        
        /* Send the event to the HPPH task  for processing */
        icsmt_send( &hpphcbp->hpphicmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("hpdirrcv: Out of memory", 0);
    }
} 

/*************
 * Name:    hplsireceive
 *
 * Purpose: Callback for receipt of a HIP 
 *          Link Status Indication  message
 *
 * Parms:   msgdesignator - The received message ID
 *          msgparm       - The contents of the HIP parameter field
 *          freemem       - Pointer to memory to be freed
 *          payloadp      - Pointer to the payload part of the message
 *          paylength     - The length of the payload 
 *          errorflg      - Error status from caller 
 *
 * Return:  None
 *
 * Note:    This callback will invoke HPPH task 
 *
 */
local void hplsireceive(    swi_uint32 msgdesignator, 
                            swi_uint8  msgparm,
                            swi_uint8  *freemem,
                            swi_uint8  *payloadp, 
                            swi_uint16 paylength, 
                            swi_bool   errorflg )
{
    struct hpphevtblock  *eventbkp;    /* event block for HPPH task */
    struct hphipevt      *hipevtp;     /* HIP union member of event block */

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Get an event block pointer */
    eventbkp = (struct hpphevtblock *) mmbufget( &hpphcbp->hpphevtpool);
    
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        hipevtp = &eventbkp->hpevtmember.hpmdmpkt;
        /* Fill the event with response message */
        eventbkp->hpphevent        = HPPHLINKIND;
        eventbkp->hpphmemfreep     = freemem;   
        hipevtp->hpphmsgdesignator = msgdesignator;
        hipevtp->hpphmsgparm       = msgparm;
        hipevtp->hpphdatap         = payloadp;
        hipevtp->hpphdatasize      = paylength;
        hipevtp->hppherrorflg      = errorflg;
        
        /* Send the event to the HPPH task  for processing */
        icsmt_send( &hpphcbp->hpphicmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("hplsircv: Out of memory", 0);
    }
} 

/*************
 *
 * Name:    hpphprocesslinkstaind
 *
 * Purpose: Process link status indication
 *          
 *             
 * Parms:   eventbkp - Pointer to the HPPH event block structure
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This is link status indication handler (0x78) executed in HPPH task
 *          context
 *
 **************/
package void hpphprocesslinkstaind(struct hpphevtblock *eventbkp)
{
    /* Not implemented at this time */
}


/*************
 *
 * Name:    hpphRelEventbk
 *
 * Purpose: To release the resources assocated with the hpph event block.
 *
 *             
 * Parms:   eventbkp - Pointer to the HPPH event block structure
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This function releases the resources associated with the 
 *          hpph event block as follows: first the memory buffer containing
 *          the newly received hpphx packet is released. Next the event buffer
 *          itself is released. Both are allocated from pools created at
 *          system startup time by this HPPH task. 
 *
 **************/
local void hpphRelEventbk( struct hpphevtblock *eventbkp )
{
    /* Free the hptx message buffer (if any) first */
    if ( eventbkp->hpphmemfreep != NULL )
    {
        mmbufrel( eventbkp->hpphmemfreep );
    }

    /* Finally, free the event block itself */
    mmbufrel( (swi_uint8 *)eventbkp );
}


/*************
 *
 * Name:    hpphtick
 *
 * Purpose: This function handles the periodic timer tick messages for
 *          the HPPH task
 *             
 * Parms:   None
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   Ticks are handled within the context of the HP PH task
 *
 *          Add calls to individual package timers, similar to hplooptimer(),
 *          within this function as they are needed. 
 *
 **************/
local void hpphtick( void )
{
    /* Call the loopback timer handler function for this tick */
    hplooptimer();
    
    /* Call the firmware download timer handler function for this tick */
    hpfwdwntimer();
    
    /* Call Device Information request timer handler function for this tick */
    hpdirtimer();
}

/*************
 *
 * Name:    hpphtask
 *
 * Purpose: The HIP Loopback task main entry point
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
 **************/
local void hpphtask( void *paramp)
{
    struct hpphevtblock  *eventbkp; /* Ptr to incoming event blk */
    swi_uint32           rmsg;      /* Ptr to incoming msg from SMT msg que */
    swi_uint32           loop;      /* For debugging purposes */
    /* task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Initialize task structures */
    hpphinit();
    
    /* Initialize firmware download module */
    hpfwdownloadinit();
    
   /* Register for incoming HIP loopback responses 
    * with HP RX Task
    */
    hpregisterhip( HPLOOPRESP, (void *)hpphlbrx );
    dlLog ( &hpphcbp->hpphdluser, DLCLASSALL,
            "Registering callback for msgid %x\n", 
            (swi_uint32)HPLOOPRESP,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
           
    /* Register for incoming HIP Link Status Indication 
     *  with HP RX Task
     */
    hpregisterhip( HPLNKSTAIND, (void *)hplsireceive );
    dlLog ( &hpphcbp->hpphdluser, DLCLASSALL,
            "Registering callback for msgid %x\n", 
            (swi_uint32)HPLNKSTAIND,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);           

    /* Register for incoming HIP Device Information Response 
     *  with HP RX Task
     */
    hpregisterhip( HPHDEVRSP, (void *)hpdirhipcb );
    dlLog ( &hpphcbp->hpphdluser, DLCLASSALL,
            "Registering callback for msgid %x\n", 
            (swi_uint32)HPHDEVRSP,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);           

    /* Register the HP task to receive periodic timer ticks */
    ictttickreg( &hpphcbp->hpphicmsgque, 
                 (swi_uint32) &hpphtickevt, 
                 &hpphcbp->hpicregbk );

    /* Tell SU that it's ok to start the next task in line */
    sutaskinitcb( SDK_HPPHPKG );

    /* Main line of processing begins next */
    for(loop=0;;loop++)
    {
        /* Wait for incoming messages. The timeout value specified
         * should be zero (wait forever) because the ictttickreg()
         * call above guarantees we will get a periodic tick message.
         * Therefore, the only time we wake up is when there is a
         * valid incoming message. Can ignore result code of icsmt_rcv()
         * but check for NULL message.
         */
        icsmt_rcv( &hpphcbp->hpphicmsgque, HPPHSMTRCVTOUT, &rmsg );

        if (rmsg)
        {
            /* event received and will be processed */
            eventbkp = (struct hpphevtblock *) rmsg;
            
            /* Dispatch control based on the incoming event */
            switch (eventbkp->hpphevent)
            {
                /* Incoming response from the modem */
                case HPPHLOOPRESP:
                    /* Process loopback response */
                    hpphprocloopresp(&eventbkp->hpevtmember.hpmdmpkt);
                    break;
    
                /* Incoming request to perform loopback test */
                case HPPHLOOPREQ:
                    /* Process loopback request */
                    hpphprocloopreq(&eventbkp->hpevtmember.hplbreq);
                    break;

                /* Incoming HIP Link Status Indication Message */
                case HPPHLINKIND:
                    /* Process Link Status Indication */
                    hpphprocesslinkstaind(eventbkp);
                    break;
                    
                case HPPHAIRREQ:
                    /* Process the AM AIR Server request */
                    hpamairreq(eventbkp->hpevtmember.hppham.hpmsgdatap,
                               eventbkp->hpphipcchannel);
                    break;

                /* Incoming AirServer change indication request */
                case HPPHAIRCHG:
                    /* Handle an AirServer Change Notification request */
                    hpprocairchg( &eventbkp->hpevtmember.hpphairchg );
                    break;

                /* Registration requests for modem change notifications */
                case HPMDMCHGREQ:
                    hpprocmdmreg( &eventbkp->hpevtmember.hpmdmreg );
                    break;

                /* Modem state change event */
                case HPPHMODEMSTATECB:
                    hpmodestatechangemsg(eventbkp);
                    break;
                
                /* Periodic timer tick message */
                case HPPHTICK: 
                    /* Handle the incoming tick message */
                    hpphtick();

                    if( loop%100 == 0 )
                    {  
                        dlLog ( &hpphcbp->hpphdluser, HPLOG_CLASSA, 
                                "[%d] Tick", 
                                (swi_uint32)loop,
                                (swi_uint32)NULL,
                                (swi_uint32)NULL,
                                (swi_uint32)NULL );
                    }
                    /* NULL out the tick event so it won't be
                     * released after this switch statement. 
                     * Tick messages are not stored in MM pool
                     * buffers and can't be released
                     */
                    eventbkp = NULL;
                    break;
                
                 case HPPHFWDWNCB:
                    /* 
                     * Process a Firmware Download specific HIP 
                     * response or indication
                     */
                    hpfwdwncbmsg(eventbkp);
                    break;
                    
                 case HPPHFWDWNREQ:
                    /* Process the AM firmware download request */
                    hpamfwdwnreq(eventbkp->hpevtmember.hppham.hpmsgdatap);
                    break;
                    
                case HPPHPORTREQ:
                    /* Process the AM Port Name request */
                    hpamportreq(eventbkp->hpevtmember.hppham.hpmsgdatap, 
                                eventbkp->hpphipcchannel);
                    break;

                case HPPHDIRREQ:
                    /* Process the Device Information Request */
                    hpprocdirreq(&eventbkp->hpevtmember.hpdevinfo);
                    break;
                    
                case HPPHDIRRESP:
                    /* Process the Device Information Response */
                    hpprocdirresp(&eventbkp->hpevtmember.hpmdmpkt);
                    break;

                 default:
                    /* ignore unknown events */
                    dlLog ( &hpphcbp->hpphdluser, DLCLASSALL,
                            "Ignoring unknown event %x\n", 
                            (swi_uint32)eventbkp->hpphevent,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL); 
                    break;   
            }

            if( eventbkp )
            {
                /* Release the event block   */
                hpphRelEventbk( eventbkp );
            }
        }
    }/* end main for loop */
}

/*************
 *
 * Name:    hpphtaskinit
 *
 * Purpose: Creates the HP Packet Handler task by making it known to the os
 *          package.
 *             
 * Parms:   None
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   Must be DRTASK *after* hpinit() has been called
 *
 **************/
global void hpphtaskinit( void )
{
    /* Create the HP Packet Handler task */
    swi_ossdkthreadcreate( hpphtask, NULL );
}

/*************
 *
 * Name:    hpphinit 
 *
 * Purpose: Initializes the Packet Handler Task of the HP package. 
 *
 * Parms:   None
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   Must be called once, after the HPPH task starts
 *
 **************/
package void hpphinit( void )
{
    struct hpphcb *hpphcbp = hpgetphcbp();
    
    /* Set up the periodic tick event block */
    hpphtickevt.hpphevent = HPPHTICK;
    hpphtickevt.hpphmemfreep = NULL;
    hpphtickevt.hpevtmember.hpmdmpkt.hpphmsgdesignator = 0;
    hpphtickevt.hpevtmember.hpmdmpkt.hpphmsgparm = 0;

    /* Create the Event Pool for events received by HPPH task */
    mmpcreate( &hpphcbp->hpphevtpool,
               "HPPHEVTPOOL", 
               HPPHEVPOOLSZ, 
               sizeof(struct hpphevtblock) );

    /* Initialize the SMT Message queue structure  */
    icsmt_create( &hpphcbp->hpphicmsgque, 
                  "HPPHMESSAGEQ", 
                  HPPHMSGDEPTH );

    /* Initialize the Modem Change Structure */
    hpmdminit();
                
    /* Initialize the loopback parameter structure */
    hplbinit();

    /* Initialize Air Server status structure */
    hpphcbp->hpphairstate.hpairserverpresent = FALSE;
    hpphcbp->hpphairstate.hpairchgera        = 0;
    hpphcbp->hpphairstate.hpairtechtype      = 0;
    slmemset ((char *)&hpphcbp->hpphairstate.hpairprodstr[0], 0,
               AMPRODSTRSIZE);

    /* Initialize Air Server Device information structure */
    hpdirinit();
    
    /* Register with the logger process, initially disabled */
    dlregister ("HPPH", &hpphcbp->hpphdluser, FALSE);
}

/*
 * $Log: hpphtask_sdk.c,v $
 */

