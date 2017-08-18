/*************
 *
 * $Id: drtask_sdk.c,v 1.14 2010/06/09 23:37:25 epasheva Exp $
 *
 * Filename: drtask_sdk.c
 *
 * Purpose:  DR package task and associated functions
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "dr/dridefs.h"

/* Defines */

/* Local storage */
/* DR task control block */
struct drtaskcb drcbk;

/*************
 *
 * Name: drgetcbp
 *
 * Purpose: Function to get a pointer to the Data Relay task's
 *          control block structure.
 *             
 * Parms:   none
 *
 * Return:  pointer to struct drtaskcb
 *
 * Abort:   none
 *
 * Notes:   None
 *
 **************/
package struct drtaskcb *drgetcbp( void )
{
    return &drcbk;
}

/*************
 *
 * Name:    drreleventbk
 *
 * Purpose: To release the resources assocated with the DR event block.
 *
 *             
 * Parms:   eventbkp - Pointer to the DR event block structure
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This function releases the resources associated with the 
 *          DR event block as follows: first the memory buffer 
 *          containing the newly received DR packet is released. 
 *          Next the event buffer itself is released. Both are 
 *          allocated from pools created at system startup time 
 *          by this DR task. 
 *
 **************/
local void drreleventbk( struct drevtblock *eventbkp )
{
    /* Free the DR message buffer (if any) first */
    if ( eventbkp->drmemfreep != NULL )
    {
        mmbufrel( eventbkp->drmemfreep );
    }

    /* Finally, free the event block itself */
    mmbufrel( (swi_uint8 *)eventbkp );
}

/*************
 * 
 * Name:    drdscallback
 *
 * Purpose: Callback for receipt of a HIP 
 *          de-mux message such as DIAG or NMEA from an endpoint
 *
 * Parms:   msgid     - The received message ID
 *          msgparm   - The contents of the HIP parameter field
 *          freemem   - Pointer to release memory from
 *          payloadp  - Pointer to the payload part of the message
 *          paylength - The length of the payload 
 *          errorflg  - Error status of the received packet
 *
 * Return:  None
 *
 * Note:    This callback invokes dssend() interface to pass the message up to
 *          the application.
 *
 **************/
local void drdscallback(   swi_uint8 msgid, 
                           swi_uint8 msgparm,
                           swi_uint8 *freemem,
                           swi_uint8 *payloadp, 
                           swi_uint16 paylength, 
                           swi_bool errorflg )
{
    swi_uint16  chanid;            /* initialize to 
                                    * DIAG device service 
                                    */
    enum drdevservicetype deviceservice = DRDIAG;
    swi_uint8 *hdrptr = NULL; /* AM data packet header pointer */
    swi_uint32 packetlen;     /* length of AM packet */
    struct drtaskcb   *drcbp;
    
    /* Get pointer to control block */
    drcbp = drgetcbp();
    
    /* do nothing if DR send interface is not enabled */
    if ( drcbp->drsendenabled )
    {
        /* Parse msgid to map to a specific device service */
        if (msgid == HPHSTACKIND)
        {
            chanid = cigetchannelnum(CIIPCDIAG);
        
            /* Convert to device service to be included in AM packet */
            deviceservice = DRDIAG;
        
            /* Get pointer to the AM header of data packet */
            hdrptr = amgethdrptr(payloadp, AMDEMUXDATAIND, freemem);
        
            /* build am packet starting from this header pointer */
            packetlen = ambuilddemuxdataind( &hdrptr, (swi_uint16)deviceservice);  
            
            /* Note: packlen is the number of bytes added by AM routine;
             * for total length one must add paylength
             */
        
            /* Call DS send inteface  for DIAG device service */
            dssend(hdrptr, (packetlen+paylength), chanid);
        }
        else
        {
            /* The following will be finalized when we have specifics
             * for NMEA device service 
             */
            chanid =  cigetchannelnum(CIIPCNMEA);
        
            /* Convert to device service to be included in AM packet */
            deviceservice = DRNMEA;
                
            /* Get pointer to the AM header of Demux data packet */
            hdrptr = amgethdrptr(payloadp, AMDEMUXDATAIND, freemem);
        
            /* build am packet starting from this header pointer */
            packetlen = ambuilddemuxdataind( &hdrptr, (swi_uint16)deviceservice);  
            
            /* Note: packlen is the number of bytes added by AM routine;
             * for total length one must add paylength
             */
        
            /* Call DS send inteface  for DIAG device service */
            dssend(hdrptr, (packetlen+paylength), chanid);
        }
    }
    else
    {
        /* Log a message */
        dlLog ( &drcbp->drdlcb, 
                DRLOG_CLASSC, /* Class C is for occasional messages */
                "Packet dropped; send disabled\n",
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL);            
    }
    /* Release memory */
    if (freemem != NULL)
    {
        mmbufrel( freemem );
    }
} 

/*************
 * Name:    drdevchgstatecb
 *
 * Purpose: Data Relay Module registered Modem State Change
 *          callback.  It will construct an event to send to 
 *          the device service via the DR task.
 *
 * Parms:   modemstate - new device state (ONLINE/OFFLINE)
 *          apprunning - TRUE, the modem is executing its application image
 *                       FALSE the modem is executing its boot loader image
 *          devtype    - device type
 *          userp      - user provided pointer (unused by this module)
 *
 * Return:  None
 *
 * Note:    This is a registered callback.  It is not called from the 
 *          DR task context.  It will construct a event message
 *          to convey to the DR task.
 *
 **************/
local void drdevchgstatecb(enum hpmdmstate modemstate, 
                            swi_bool apprunning,
                            enum SWI_TYPE_Device devtype,
                            void *userp )
{
    /* Allocate event block to send event to DR task */
    struct drevtblock  *eventbkp;    /* event block for DR task */

    /* Get pointer to the task control block */
    struct drtaskcb *drcbp = drgetcbp();

    /* Get an event block pointer */
    eventbkp = (struct drevtblock *) mmbufget( &drcbp->drevtpool);
    
    /* Log that this callback has been called -indicate the new device state */
    dlLog ( &drcbp->drdlcb, DRLOG_CLASSA,  /* Class A is for verbose logging */
            "Modem state change callback - modemOn[%d]\n",
            (swi_uint32)modemstate,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);            
           
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        /* Fill the event with Loopback response message */
        eventbkp->drevent         = DRMODEMSTATECB;
        eventbkp->drmemfreep      = NULL;  
        
        /* If  application is not running and modem state is online
         * report that modem is offline to the Application.
         */
        if ((!apprunning) && (modemstate == HPMDMONLINE)) 
        {
            modemstate = HPMDMOFFLINE;
        }
        eventbkp->drevtmember.drdevchg.drmdmstate = modemstate;
        eventbkp->drevtmember.drdevchg.drdevtype  = devtype;
        
        /* Send the new event to the DR task  for processing */
        icsmt_send( &drcbp->dricmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("DR: Out of memory", 0 );
    }
} 

/*************
 * Name: drmdmchngmsg
 *
 * Purpose: This function acts on a DR task received DRMODEMSTATECB 
 *          event.  This event conveys modem state change information
 *          to the Data Relay module.
 * 
 *
 * Parms:   eventbkp - Pointer to DR task received event

 * Return:  None
 *
 * Note:    This function is called from the DR task context.
 *          If the modem state is online but the system is in boot-hold
 *          the modem state is reported as offline.
 *          This function build Swi Demux Response AM packet and sends it over
 *          DS send interface function.
 */
package void drmdmchngmsg(struct drevtblock  *eventbkp)
{
    swi_uint16 chanid;   /* channel id as assigned by DS */
    enum drdevservicetype deviceservice = DRDIAG; /* device service as set 
                                                   * by DR
                                                   */
    swi_uint32 packetlen;     /* length of AM packet */

    
    struct drtaskcb   *drcbp;
    swi_uint8 *inbufp = NULL; /*  buffer pointer to build AM message */
    swi_uint8  opcode;        /* AM Demux Ctl Ind opertion code */
    swi_uint8 devicepresent;  /* indication of device on/offline */
    /* Input parameters for building AM packets */
    enum swi_amresults resultcode =  SWI_AMNODEVICE; /* default "no modem" */ 
    swi_uint8 errorstring[AMMAXERRORSTRINGSZ]; /* error string passed to AM to
                                                * build a packet
                                                */ 
    errorstring[0] = 0;
    
    /* Set operation code to Modem State */
    opcode = 0; /* better AMDEMUXCTLINDMDMSTATE when defined */
    
    /* Define and allocate the task control block */
    drcbp = drgetcbp();
    
    /* Proceed only if DR send interface has been enabled by Application */    
    if (drcbp->drsendenabled)
    {
        /* Get an input buffer from the pool */
        inbufp = (swi_uint8 *)mmbufget( &drcbp->drbufpool );
        /* Note: The beginning of the pool block is the beginning of the AM header 
         *       and a pointer to it is passed to AM build function. 
         *       The same pointer is the memory release pointer that is passed
         *       to dssend() function.
         */

        /* Construct modem change indication to DIAG channel */
        chanid =  cigetchannelnum(CIIPCDIAG);
        /* Convert to device service to be included in AM packet */
        deviceservice = DRDIAG;
        
        /* Set the modem status */
        if(eventbkp->drevtmember.drdevchg.drmdmstate == DRMDMONLINE)
        {
            /* make sure modem is not in boot and hold mode in which case 
             * must be reported to application as offline 
             */
            devicepresent = 1;
            /* set modem status in AM message to on-line */
            resultcode = SWI_AMSUCCESS;
            /* Note: If the system is in boot-hold mode and the modem is online 
             *       then a modem offline state is propagate to IPC channels
             *       This condition is handled  in drdevchgstatecb()
             */
        }
        else
        {
            devicepresent = 0;
            /* set modem status in AM message to off-line */
            resultcode =  SWI_AMNODEVICE;
        }
    
        /* Build AM packet starting from the header pointer */
        /* Increment era number */
        ++drcbp->dreranumber;
        packetlen = ambuilddemuxctlind( &inbufp, resultcode, &errorstring[0], 
            deviceservice, opcode, devicepresent, drcbp->dreranumber);  

        /* Call DS send inteface  for DIAG device service */
        dssend(inbufp, packetlen, chanid);
    
        /* Release memory */
        if ( inbufp != NULL )
        {
            mmbufrel( inbufp );
        }
    }
}

/*************
 * Name:    drapprequestmsg
 *
 * Purpose: This function acts on a DR task received DRAPPREQUEST
 *          event.  This event conveys interface and modem state information
 *          to the Data Relay module.
 * 
 *
 * Parms:   eventbkp - Pointer to DR task received event

 * Return:  None
 *
 * Note:    None
 */
local void drapprequestmsg(struct drevtblock  *eventbkp)
{
    struct drtaskcb   *drcbp;
    enum swi_amresults resultcode =  SWI_AMNODEVICE; /* default "no modem" */
    swi_bool airserverstatus = FALSE; /* air server presence flag */
    swi_uint32 packetlen;     /* length of AM packet */
    swi_uint16 chanid;        /* channel id as assigned by DS */
    swi_uint8 devicepresent;  /* indication of device on/offline */
    swi_uint8  opcode;        /* AM Demux Ctl Ind opertion code */
    swi_uint8 *inbufp = NULL; /* buffer pointer to build AM message */
    swi_uint8 errorstring[AMMAXERRORSTRINGSZ]; /* error string passed to AM to
                                                * build a packet
                                                */ 
    
    errorstring[0] = 0;
    
    /* On receiving request from the application send response - control 
     * indication with the current modem state 
     */
    
    /* Set operation code to Modem State */
    opcode = 0; 
    
    /* Get the task control block */
    drcbp = drgetcbp();
    
    switch (eventbkp->drevtmember.drapprequest.drdevservice)
    {
        case  DRDIAG:
            chanid =  cigetchannelnum(CIIPCDIAG);
            if (eventbkp->drevtmember.drapprequest.droperation)
            {
                /* Stop device service */
                drcbp->drsendenabled = FALSE; /* stop DR send interface */
            }
            else
            {
                /* Start device service */
                drcbp->drsendenabled = TRUE; /* enable DR send interface */
            }
            
            /* Send current modem state */
            /* Get an input buffer from the pool */
            inbufp = (swi_uint8 *)mmbufget( &drcbp->drbufpool );
            
            if ( !inbufp )
            {
                erAbort("drapprequestmsg: buffer not allocated", 0 );
            }
            
            /* Note: The beginning of the pool block is the beginning of  
             *       the AM header and a pointer to it is passed to AM 
             *       build function. The same pointer is the memory release 
             *       pointer that is passed to dssend() function.
             */

            /* Get current airserverstatus */
            airserverstatus = hpgetairserverstatus();
        
            /* Set the modem status */
            if( airserverstatus )
            {
                devicepresent = 1;
                /* Note: If we are here the application is running so
                 *       set modem status in AM message to on-line always
                 */
                resultcode = SWI_AMSUCCESS;
            }
            else
            {
                devicepresent = 0;
                /* set modem status in AM message to off-line */
                resultcode =  SWI_AMNODEVICE;
            }
            /* Build AM packet starting from the header pointer */
            /* Increment era number */
            ++drcbp->dreranumber;
            packetlen = ambuilddemuxctlind( &inbufp, resultcode, &errorstring[0], 
                eventbkp->drevtmember.drapprequest.drdevservice, opcode, 
                devicepresent, drcbp->dreranumber); 
                 
            /* Call DS send inteface  for DIAG device service */
            dssend(inbufp, packetlen, chanid);
    
            /* Release memory */
            if ( inbufp != NULL )
            {
                mmbufrel( inbufp );
            }
            break;
            
        case DRNMEA:
            /* Not implemented */
            break;
        
        default:
            /* Abort*/
            erAbort("drprocdemuxreq: unknown device service", 
                eventbkp->drevtmember.drapprequest.drdevservice);
            break;        
    }
}

/*************
 * 
 * Name:    drdiagmsg
 *
 * Purpose: Callback for receipt of a diagnostic message from the DM endpoint 
 *
 * Parms:   eventbkp -  Pointer to the structure that is holding the diagnostic
 *                      message information
 *
 * Return:  None
 *
 * Note:    This callback invokes dssend() interface to pass the message up to
 *          the application.
 *
 **************/
package void drdiagmsg(
    swi_uint8 *inbufp, 
    swi_uint32 resplength,
    swi_uint8 *memfreep, 
    swi_uint16 ipcchannel)
{
    swi_uint16  chanid ; /* initialize to 
                                   * DIAG device service 
                                   */
    enum drdevservicetype deviceservice = DRDIAG;
    swi_uint32 packetlen;     /* length of AM packet */
    struct drtaskcb   *drcbp;
    
    /* payload from modem */
    swi_uint8 *payloadp = inbufp; 
    /* length from payload */
    swi_uint16 paylength = resplength; 
    swi_uint8 *freememp = NULL;
    swi_uint8 *drpayloadp;
    
    /* Get pointer to control block */
    drcbp = drgetcbp();
    
    /* do nothing if DR send interface is not enabled */
    if ( drcbp->drsendenabled )
    {
        chanid =  cigetchannelnum(CIIPCDIAG);
        
        /* Get an input buffer from the pool */
        freememp = (swi_uint8 *)mmbufget( &drcbp->drbufpool );
        if ( freememp )
        {
            /* Convert to device service to be included in AM packet */
            deviceservice = DRDIAG;
        
            /* build am packet starting from this header pointer */
            packetlen = ambuilddemuxdataind( &freememp, (swi_uint16)deviceservice);  
            
            drpayloadp = (swi_uint8 *) ((swi_uint32)freememp + packetlen);
            slmemcpy( drpayloadp, payloadp, paylength);
            
            /* Note: packlen is the number of bytes added by AM routine;
             * for total length one must add paylength
             */
        
            /* Call DS send inteface  for DIAG device service */
            dssend(freememp, (packetlen+paylength), chanid);
        }
        else
        {
            erAbort("drdiagmsg: buffer not allocated", 0 );
        }
    }
    else
    {
        /* Log a message */
        dlLog ( &drcbp->drdlcb, DRLOG_CLASSB,  /* Class B is for Informational */
                "drdiagmsg: Packet dropped; send disabled\n",
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL,
                (swi_uint32)NULL);            
    }
    /* Release memory */
    if (freememp != NULL)
    {
        mmbufrel( freememp );
    }
} 

/*************
 *
 * Name:    drstartipcs
 *
 * Purpose: Starts all instances of DS shells for DR services
 *             
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   None 
 *
 **************/
package void drstartipcs( )
{   
    swi_uint16 numshells;
    swi_uint16 i;
    /* Define and allocate the task control block */
    struct drtaskcb   *drcbp = drgetcbp();
    swi_uint16 channel;
    swi_uint16 usetype;
    
    /* Loop through the number of available CNRR/notification channel pairs */
    numshells = cigetnumdrservices();
    for (i = 0; i < numshells; i++)
    {
        /* Get the IPC Name and set callback function */
        drcbp->dripcsh[i].dsipcshellnamep = cigetnextdrnamep(i);
        channel = cigetchannelnum ((char *)drcbp->dripcsh[i].dsipcshellnamep);
        /* Callbacks are different for services */
        usetype = cigetifusetype (channel);
        if (usetype == USEP_DIAG)
            drcbp->dripcsh[i].dsipccbfcnp = (void *)drdiagmsg;
        else
           drcbp->dripcsh[i].dsipccbfcnp = (void *)dripcrcvhandler;

        /* Create a thread for that channel */
        swi_ossdkthreadcreate ( dslaunchshell, (void *)&drcbp->dripcsh[i] );
    }
}

/*************
 *
 * Name: drtask
 *
 * Purpose: The Data Relay task is used to coordinate Device Services
 *             
 * Parms:   paramp - pointer to data that may be passed in when this task
 *                   is started.
 *
 * Return:  never returns
 *
 * Abort:   none
 *
 * Notes:   The DR Task runs on SDK site only.
 *          The DR Task initiates the DS instances for the supported
 *          device services. 
 *          
 *
 **************/
local void drtask( void *paramp)
{
    swi_uint32         loop     = 0;     /* Helps force log messages to print */
    struct drtaskcb   *drcbp   = NULL;
    struct drevtblock *eventbkp = NULL; /* Ptr to incoming event blk */
    swi_uint32         rmsg     = 0;    /* Ptr to incoming msg from 
                                         * SMT msg que 
                                         */

    /* Define and allocate the task control block */
    drcbp = drgetcbp();
    
    /* Initialize the DR package's access to logging */
    dlregister( "DR", &drcbp->drdlcb, FALSE );
    
    /* Create task message pool used for transmit buffer allocation */
    mmpcreate( &drcbp->drbufpool, "drPool", DRPOOLSZ, DRBUFSZ );
    
    /* Create the Event Pool for events received by HP tx  task */
    mmpcreate( &drcbp->drevtpool,
               "DREVTPOOL", 
               DREVPOOLSZ, 
               sizeof(struct drevtblock) );

    /* Initialize the SMT Message queue structure  */
    icsmt_create( &drcbp->dricmsgque, "DRMESSAGEQ", DRMSGDEPTH );

    /* Activate a DS instances for DIAG, NMEA, DIAG endpoint services */ 
    drstartipcs();

    /* Register for incoming DIAG message HPHSTACKIND with HP RX Task */
    /* Note: The callback passes the message to dssend() interface */
    hpregisterhip( HPHSTACKIND, (void *)drdscallback ); 
    dlLog ( &drcbp->drdlcb, DRLOG_CLASSC, /* Class C for occasional msgs */
            "Registering callback for msgid %d\n", 
            (swi_uint32)HPHSTACKIND,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);            
           
    /* Register for modem status change */
    /* Note: The modem status change is passed by the callback to the 
     * DR task queue as an event. DR task processes this event by
     * forming a notification message which is send over dssend()
     * interface to the device service IPC. This message is send 
     * to all device service IPC channels 
     */
    hpmdmregister( drdevchgstatecb, &(drcbp->drdevchgreqblk), 
        "DRTASK", NULL);
    dlLog ( &drcbp->drdlcb, DRLOG_CLASSC, /* Class C for occasional msgs */
            "Registering callback for mdm state \n",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);            

    /* Tell the world we've started up */
    dlLog(  &drcbp->drdlcb, DRLOG_CLASSC,  /* Class C for occasional msgs */
            "Data Relay Task started\n",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);            

    /* Tell the SU package it's OK to let the next task go */
    sutaskinitcb( SDK_DRPKG );

    /* Main line of processing begins next */
    for(loop=0;;loop++)
    {
        /* Wait to receive  a control message from  the Application
         * via DS and DR IPC receive handler
         */
        icsmt_rcv( &drcbp->dricmsgque, DRSMTRCVTOUT, &rmsg );

        if (rmsg)
        {
            /* event received and will be processed */
            eventbkp = (struct drevtblock *) rmsg;
            
            /* Dispatch control based on the incoming event */
            switch (eventbkp->drevent)
            {
                /* Incoming response from the modem */
                case DRMODEMSTATECB:
                    drmdmchngmsg(eventbkp);
                    break;
                /* Request from the Application */
                case DRAPPREQUEST:
                    drapprequestmsg(eventbkp);
                    break;
                /* Incoming diagnostic response from the modem using DIAG EP 
                 * NOTE: THIS IS NOT HANDLED HERE DUE TO PERFORMANCE ISSUES.
                 *       WE SEND THE DIAG MSG DIRECTLY INSTEAD OF QUEUING IT.
                 */
                default:
                    /* ignore unknown events */
                    dlLog ( &drcbp->drdlcb, 
                            DRLOG_CLASSC, /* Class C for occasional messages */
                            "Ignoring unknown event %x\n", 
                            (swi_uint32)eventbkp->drevent,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL);            
                    break;
            }
            
            if( eventbkp )
            {
                /* Release the event block */
                drreleventbk( eventbkp );  
            }
        }
    }
}

/*************
 *
 * Name: drinit
 *
 * Purpose: Initialize internal variables within the DR package
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
global void drinit( void )
{ 
    struct drtaskcb   *drcbp; 
    
    /* Define and allocate the task control block */
    drcbp = drgetcbp(); 
    
    /* Set era number to 0 */
    drcbp->dreranumber = 0;
    
    /* Disable send initially - this is enabled by  application */
    drcbp->drsendenabled = FALSE;
}

/*************
 *
 * Name: drtaskinit
 *
 * Purpose: Creates the task by making it known to the os package.
 *             
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Must be called *after* drinit() has been called
 *
 **************/
global void drtaskinit( void )
{
    /* Create the Data Relay  task */
    swi_ossdkthreadcreate( drtask, NULL );
    /* Note: After the drtask is created it creates the ds shell instances 
     * for DIAG and NMEA device services 
     */
}

/*
 * $Log: drtask_sdk.c,v $
 */

