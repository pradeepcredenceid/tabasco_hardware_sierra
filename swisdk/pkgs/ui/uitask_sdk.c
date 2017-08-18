/*************
 *
 * $Id: uitask_sdk.c,v 1.5 2010/06/09 23:54:32 epasheva Exp $
 *
 * Filename: uitask_sdk.c
 *
 * Purpose:  UI package task and associated functions
 *
 * Copyright: © 2009 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "ui/uiidefs.h"

/* Defines */

/* Local storage */
/* UI task control block */
struct uitaskcb uicbk;

/*************
 *
 * Name: uigetcbp
 *
 * Purpose: Function to get a pointer to the UI task's
 *          control block structure.
 *             
 * Parms:   none
 *
 * Return:  pointer to struct uitaskcb
 *
 * Abort:   none
 *
 * Notes:   None
 *
 **************/
package struct uitaskcb *uigetcbp( void )
{
    return &uicbk;
}

/*************
 *
 * Name:    uireleventbk
 *
 * Purpose: To release the resources assocated with the UI event block.
 *         
 * Parms:   eventbkp - Pointer to the UI event block structure
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This function releases the resources associated with the 
 *          UI event block as follows: first the memory buffer 
 *          containing the newly received UI packet is released. 
 *          Next the event buffer itself is released. Both are 
 *          allocated from pools created at system startup time 
 *          by this UI task. 
 *
 **************/
local void uireleventbk( struct uievtblock *eventbkp )
{
    /* Free the UI message buffer (if any) first */
    if ( eventbkp->uimemfreep != NULL )
    {
        mmbufrel( eventbkp->uimemfreep );
    }

    /* Finally, free the event block itself */
    mmbufrel( (swi_uint8 *)eventbkp );
}

/*************
 *
 * Name:    uisenduireqevt
 *
 * Purpose: This function sends the event UIAPPREQUEST to UI task
 *             
 * Parms:   msgp    - pointer to message being passed in
 *          datalen - length of data passed in 
 *
 * Return:  none
 *
 * Abort:   When UI is out of event blocks
 *
 * Notes:   none
 * 
 **************/
local void uisenduireqevt(swi_uint8 *msgp, swi_uint16 datalen)
{  
    struct uievtblock  *eventbkp;    /* event block pointer for UI task */

    /* Get pointer to the task control block */
    struct uitaskcb *uicbp = uigetcbp();

    /* Allocate event block to send event to UI task */
    eventbkp = (struct uievtblock *) mmbufget( &uicbp->uievtpool);
           
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        /* Fill the event with UI request message */
        eventbkp->uievent         = UIAPPREQUEST;
        eventbkp->uimemfreep      = NULL;  
        /* Fill the rest of the event block */
        eventbkp->uidatalen = datalen;
                                                    
        /* Copy additional data not defined in AM */
        slmemset ((char *)&(eventbkp->uireqstr[0]), 0, UIBUFSZ);
        slmemcpy (&(eventbkp->uireqstr[0]), msgp, datalen);
        
        /* Send the new event to the UI task for processing */
        icsmt_send( &uicbp->uiicmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("UI: Out of memory", 0);
    }
} 

/*************
 *
 * Name: uiipcrcvhandler
 *
 * Purpose: Generic IPC Receive Handler which is invoked for incoming  data 
 *          from UI Service IPC channel. 
 *             
 * Parms:   inipcmsgp    - Pointer to received AM packet payload.
 *          inipcmsglen  - length of received message in bytes
 *          memfreep     - Pointer to be used when freeing memory 
 *                         for this request
 *          ipcchannel   - IPC channel over which this message came in - unused
 *                         here
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   The UI service must be initialized and the corresponding IPC 
 *          channel open before the handler is invoked.
 *          The supported protocol is described in IDD_UIMessaging.doc
 *          This function must release memfreep after sending data to
 *          the endpoint.
 *          We want UI to be in charge of the buffer so that it can be released
 *          here or somewhere else under the control of UI. This could be used 
 *          for future improvements, e.g. avoiding copying the message.
 * 
 * 
 **************/
local void uiipcrcvhandler( 
    swi_uint8 *inipcmsgp, 
    swi_uint32 inipcmsglen,
    swi_uint8 *memfreep,
    swi_size_t ipcchannel)
{
    swi_uint16 amtype;
    swi_uint16 amheaderversion;
    swi_uint16 ampacketlength;
    swi_uint8  amresultcode;
    swi_uint8 *msgp;           /* Local copy of input IPC message pointer to be
                                * used in parsing 
                                */                               
    swi_uint16 datalen;
        
    msgp = inipcmsgp;  
 
    /* Parse the received message */
    /* First parse the header */
    amparseamheader( &amtype, 
                    &amheaderversion, 
                    &amresultcode, 
                    &ampacketlength,
                    &msgp);
    /* Note: header parameter check not implemented */
    switch (amtype)
    {
        case AMUIREQUEST:
            /* Parse UI Request */
            amparseuirequest( &msgp, &datalen);
             
            uisenduireqevt ( msgp, datalen);
            break;

        default:
            /* Do nothing */          
            break;
    }
    
    /* Free message buffer here since we either copied the message or will
     * ignore the message if unknown
     */

    if (memfreep != NULL)
    {
        mmbufrel( memfreep );
    }
 }

/*************
 *
 * Name:    uitask
 *
 * Purpose: The UI task is used to coordinate user requests
 *             
 * Parms:   paramp - pointer to data that may be passed in when this task
 *                   is started.
 *
 * Return:  never returns
 *
 * Abort:   none
 *
 * Notes:   The UI Task runs on the SDK side only.
 *
 **************/
local void uitask( void *paramp )
{
    swi_uint32         loop     = 0;     /* Helps force log messages to print */
    struct uitaskcb   *uicbkp   = NULL;
    struct uievtblock *eventbkp = NULL; /* Ptr to incoming event blk */
    swi_uint32         rmsg     = 0;    /* Ptr to incoming msg from 
                                         * SMT msg que 
                                         */

    /* Define and allocate the task control block */
    uicbkp = uigetcbp();
    
    /* Initialize this package's ability to generate logs, initially enabled */
    dlregister( "UI", &uicbkp->uidlcb, TRUE );
    
    /* Create the IPC Pool used for exchanging traffic with the IPC layer */
    mmpcreate( &(uicbkp->uibufpool), "uiPool", UIIPCPOOLSZ, UIIPCBUFSZ );
    
    /* Create the Event Pool for events received by UI task */
    mmpcreate( &uicbkp->uievtpool,
               "UIEVTPOOL", 
               UIEVPOOLSZ, 
               sizeof(struct uievtblock) );
    
    /* Initialize the SMT Message queue structure  */
    icsmt_create( &uicbkp->uiicmsgque, "UIMESSAGEQ", UIMSGDEPTH );
     
     /* Activate a UI instance for DS service */
    uicbkp->uiipcshell.dsipccbfcnp = uiipcrcvhandler;
    uicbkp->uiipcshell.dsipcshellnamep = (swi_uint8 *)CIIPCUI;
    swi_ossdkthreadcreate( dslaunchshell, &uicbkp->uiipcshell );

    /* Tell the world we've started up */
    dlLog(  &uicbkp->uidlcb, UILOG_CLASSA,
            "User Interface Task started\n",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);            


    /* Tell SU it's OK to let the next task go */
    sutaskinitcb( SDK_UIPKG );

    /* Main line of processing begins next */
    for(loop=0;;loop++)
    {
        /* Wait to receive  a control message from  the Application
         * via DS IPC and UI receive handler
         */
        icsmt_rcv( &uicbkp->uiicmsgque, UISMTRCVTOUT, &rmsg );

        if (rmsg)
        {
            /* event received and will be processed */
            eventbkp = (struct uievtblock *) rmsg;
            
            /* Dispatch control based on the incoming event */
            switch (eventbkp->uievent)
            {
                /* Request from the Application */
                case UIAPPREQUEST:
                     uiprocuidlreq(eventbkp->uireqstr);
                    break;
                default:
                    /* ignore unknown events */
                    dlLog ( &uicbkp->uidlcb, DLCLASSALL,
                            "Ignoring unknown event %x\n", 
                            (swi_uint32)eventbkp->uievent,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL);            
                    break;
            }
            
            if( eventbkp )
            {
                /* Release the event block */
                uireleventbk( eventbkp );
            }
        }
    }
}

/*************
 *
 * Name: uiinit
 *
 * Purpose: Initialize internal variables within the UI package
 *          at SDK startup
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
global void uiinit( void )
{ 
    /* Nothing at this time */ 
    
}

/*************
 *
 * Name: uitaskinit
 *
 * Purpose: Creates the task by making it known to the os package.
 *             
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Must be called *after* uiinit() has been called
 *
 **************/
global void uitaskinit( void )
{
    /* Create the User Interface task */
    swi_ossdkthreadcreate( uitask, NULL );
}

/*
 * $Log: uitask_sdk.c,v $
 */

