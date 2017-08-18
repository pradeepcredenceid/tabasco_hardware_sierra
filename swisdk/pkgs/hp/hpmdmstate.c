/*************
 *
 * $Id: hpmdmstate.c,v 1.16 2010/06/09 23:45:06 epasheva Exp $
 *
 * Filename: hpmdmstate.c
 *
 * Purpose:  Contains code that handles all aspects of the modem
 *           state change facility offered by the HPPH task. This
 *           facility allows external packages to register for 
 *           notifications (via callback) whenever the modem 
 *           appears (enumerates) or disappears (de-enumerates)
 *           from the system
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "hp/hpidefs.h"
#include "us/usudefs.h"

/*************
 * Name:    hpapprunning
 *
 * Purpose: This function indicates the state of the device: Application
 *          or Bootloader.
 *
 * Parms:   None
 *
 * Return:  TRUE  - if the modem is in APPLICATION mode
 *          FALSE - otherwise
 *
 * Note:    None
 *
 */
package swi_bool hpapprunning(void)
{
    struct hpmdmstreg   *regcbp;

    /* Pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Initialize the pointer to the registration control block */
    regcbp = &hpphcbp->hpphmdmregcb;

    /* Return the current value of apprunning */
    return(regcbp->hpmdmapprunning);   
}
/*************
 * Name:    hpamportreq
 *
 * Purpose: This function is called when a HPPHPORTREQ event is received
 *          by the HPPH task.  It parses and processes the received AM Port Name
 *          request.
 *
 * Parms:   packetp    - pointer to received AM message
 *          ipcchannel - IPC channel that request came in from
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
package void hpamportreq(swi_uint8 *packetp, swi_uint8 ipcchannel)
{
    struct amusbportnameparms portnameparm;
    swi_uint16 outputsize;
    swi_uint8  outputbuf[AMPORTNAMERESPTOTAL];
    swi_uint8  *outputbufp = &outputbuf[0];
    enum swi_amresults amresultcode = SWI_AMFAILED;
    struct usbep amep;
    /* Pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();
    
    /* clear local port name structure */
    slmemset((char *)&portnameparm, 0, sizeof(struct amusbportnameparms));
    slmemset((char *)&outputbuf[0], 0, AMPORTNAMERESPTOTAL);
    
    /* Log the new port name request */
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "Port Name request received",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
    
    /* Get Port Name */
    usgetep (USEP_HIP, &amep);
    if (usgetepname( amep.usreadep, &(portnameparm.amhipport[0]),
                  sizeof (portnameparm.amhipport), USREAD))
        amresultcode = SWI_AMSUCCESS;

    usgetep (USEP_DIAG, &amep);
    if (usgetepname( amep.usreadep, &(portnameparm.amdiagport[0]),
                  sizeof (portnameparm.amdiagport), USREAD))
        amresultcode = SWI_AMSUCCESS;

    usgetep (USEP_NMEA, &amep);
    if (usgetepname( amep.usreadep, &(portnameparm.amnmeaport[0]),
                  sizeof (portnameparm.amnmeaport), USREAD))
        amresultcode = SWI_AMSUCCESS;

    usgetep (USEP_ATCMD, &amep);
    if (usgetepname( amep.usreadep, &(portnameparm.amatcmdport[0]),
                  sizeof (portnameparm.amatcmdport), USREAD))
        amresultcode = SWI_AMSUCCESS;

    usgetep (USEP_DATA1, &amep);
    if (usgetepname( amep.usreadep, &(portnameparm.amdata1port[0]),
                  sizeof (portnameparm.amdata1port), USREAD))
        amresultcode = SWI_AMSUCCESS;

    usgetep (USEP_DATA2, &amep);
    if (usgetepname( amep.usreadep, &(portnameparm.amdata2port[0]),
                  sizeof (portnameparm.amdata2port), USREAD))
        amresultcode = SWI_AMSUCCESS;

    usgetep (USEP_DATA3, &amep);
    if (usgetepname( amep.usreadep, &(portnameparm.amdata3port[0]),
                  sizeof (portnameparm.amdata3port), USREAD))
        amresultcode = SWI_AMSUCCESS;

    usgetep (USEP_MASS_STORAGE, &amep);
    if (usgetepname( amep.usreadep, &(portnameparm.ammstorport[0]),
                  sizeof (portnameparm.ammstorport), USREAD))
        amresultcode = SWI_AMSUCCESS;
        
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "Constructing Port Name response %d", 
            (swi_uint32)amresultcode,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
        
    /* prepare outgoing AM portname response packet */
    outputsize =  ambuildusbportnameresponse( &outputbufp, 
                      amresultcode,
                      &portnameparm);
    
    /* send response to the host */
    amsdk_send( &outputbuf[0], outputsize, ipcchannel );
}

/*************
 *
 * Name:    hpsendportnamerr
 *
 * Purpose: Send a new portname request IPC message to the HP task 
 *             
 * Parms:   inipcmsgp - Pointer to the Message Data portion of the
 *                      inbound (i.e. from the application) AM message
 *          memfreep  - Pointer to the memory release address for the
 *                      inbound message. HP task will manage the
 *                      release of this packet when done processing it
 *          ipcchannel - IPC channel that request came in from
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Allocate an event block, then store the calling arguments
 *          into the appropriate fields and send the packet to the 
 *          HP task. Buffer releasing is the responsibility of the 
 *          HP task, callers must not do this
 *
 **************/
global void hpsendportnamerr( swi_uint8 *inipcmsgp, swi_uint8 *memfreep, 
swi_uint8 ipcchannel)
{
    struct hpphevtblock  *eventbkp;    /* event block for HPPH task */

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Get an event block pointer */
    eventbkp = (struct hpphevtblock *) mmbufget( &hpphcbp->hpphevtpool);
    
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        eventbkp->hpphevent     = HPPHPORTREQ;
        eventbkp->hpphmemfreep  = memfreep;
        eventbkp->hpphipcchannel = ipcchannel;
        eventbkp->hpevtmember.hppham.hpmsgdatap = inipcmsgp;
        
        /* Send the event to the HPPH task  for processing */
        icsmt_send( &hpphcbp->hpphicmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("HPPH: Out of memory", 0);
    }
}

/*************
 * Name:    hpamairreq
 *
 * Purpose: This function is called when a HPPHAIRREQ event is received
 *          by the HPPH task.  It parses and processes the received AM Air 
 *          Server request.
 *
 * Parms:   packetp    - pointer to received AM message
 *          ipcchannel - IPC channel that request came in from
 *
 * Return:  None
 *
 * Note:    This function is called from the HPPH task context.
 *
 */
package void hpamairreq(swi_uint8 *packetp, swi_uint8 ipcchannel)
{
    struct amairserverparms airparm;
    swi_uint16 outputsize;
    swi_uint8  outputbuf[AMAIRSRVRESPTOTAL];
    swi_uint8  *outputbufp = &outputbuf[0];
    enum swi_amresults amresultcode = SWI_AMFAILED;
    /* Pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();
    
    /* clear local Air Server structure */
    slmemset((char *)&airparm, 0, sizeof(struct amairserverparms));
        
    /* Parse the contents of the Air Server request packet */
    amparseairserverrequest( &packetp, &airparm );

    /* Log the new firmware download request */
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "Air Server request received",
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
    
    /* if this is a Get Avail Air Servers Request */
    if(airparm.amairoperation == AMAIRSERVERAVAIL)
    {
        airparm.amairdevicestat = hpphcbp->hpphairstate.hpairserverpresent;
        airparm.amairchgera = hpphcbp->hpphairstate.hpairchgera;
        airparm.amairdevicetype = ddgetdevtype();
        slmemset((char *)&airparm.amairclimac[0], 0, AMMACSIZE);
        slmemset((char *)&airparm.amairservmac[0], 0, AMMACSIZE);
        airparm.amairapprunning = hpphcbp->hpphmdmregcb.hpmdmapprunning;
        airparm.amairtechtype = hpphcbp->hpphairstate.hpairtechtype;
        slmemset((char *)&airparm.amairprodstr[0], 0, AMPRODSTRSIZE);
        slmemcpy(&airparm.amairprodstr[0], 
                 &hpphcbp->hpphairstate.hpairprodstr[0], 
                 AMPRODSTRSIZE);
        
        if(hpphcbp->hpphairstate.hpairserverpresent)
        {
            amresultcode = SWI_AMSUCCESS;
        }
    }
    
    dlLog(  &hpphcbp->hpphdluser, DLCLASSALL, 
            "Constructing Air Server response %d", 
            (swi_uint32)amresultcode,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL);
    
    /* prepare outgoing AM Air Server response packet */
    outputsize =  ambuildairserverresponse( &outputbufp, 
                      amresultcode,
                      &airparm);
    
    /* send response to the host */
    amsdk_send( &outputbuf[0], outputsize, ipcchannel );
    
}
/*************
 *
 * Name:    hpsendairserverrr
 *
 * Purpose: Send a new Air Server request IPC message to 
 *          the HP task 
 *             
 * Parms:   inipcmsgp - Pointer to the Message Data portion of the
 *                      inbound (i.e. from the application) AM message
 *          memfreep  - Pointer to the memory release address for the
 *                      inbound message. HP task will manage the
 *                      release of this packet when done processing it.
 *          ipcchannel- IPC channel over which this request came from.
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Allocate an event block, then store the calling arguments
 *          into the appropriate fields and send the packet to the 
 *          HP task. Buffer releasing is the responsibility of the 
 *          HP task, callers must not do this
 *
 **************/
global void hpsendairserverrr( 
    swi_uint8 *inipcmsgp,
    swi_uint8 *memfreep,
    swi_uint8 ipcchannel )
{
    struct hpphevtblock  *eventbkp;    /* event block for HPPH task */

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Get an event block pointer */
    eventbkp = (struct hpphevtblock *) mmbufget( &hpphcbp->hpphevtpool);
    
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        eventbkp->hpphevent     = HPPHAIRREQ;
        eventbkp->hpphmemfreep  = memfreep;
        eventbkp->hpphipcchannel  = ipcchannel;
        eventbkp->hpevtmember.hppham.hpmsgdatap = inipcmsgp;
        
        /* Send the event to the HPPH task  for processing */
        icsmt_send( &hpphcbp->hpphicmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("HPPH: Out of memory", 0);
    }
}
/*************
 *
 * Name:    hpmdmregister
 *
 * Purpose: Entry point allowing external packages to register for modem
 *          state changes
 *             
 * Parms:   cbfp     - Pointer to a function to call whenever the modem
 *                     state changes. The prototype is documented below
 *          reghndlp - Caller-supplied handle structure pointer used by
 *                     mdm state change to keep track of registrants
 *          namep    - Arbitrary string passed by caller to identify who
 *                     owns this registration block. Used for logging and
 *                     troubleshooting. E.g. "XXTask" 
 *          userp    - Arbitrary user-specified parameter passed to the 
 *                     callback function whenever that function is called
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This function runs outside of the HPPH task's context
 *
 *              void      cbfp( enum hpmdmstate modemstate, 
 *                              enum SWI_TYPE_Device devtype,
 *                              void *userp );
 *
 *              Where:
 *              =====
 *              mdmstate   is one of the enumerations of enum hpmdmstate,
 *              apprunning TRUE, the app'n is running, FALSE, the boot loader is
 *              devtype    is the type of device connected using the AM package
 *                         enumerations. Invalid if there is no modem enumerated
 *              userp      is the arg passed in the request for registration
 *                         for modem state change events
 *          
 *          Once the registration has been accepted, the caller's callback
 *          is called immediately to allow the caller to synchronize with the
 *          current state of the modem. From then on, the callback is guaranteed
 *          to be called only once per modem state change
 *
 **************/
global void hpmdmregister( void(*cbfp)(enum hpmdmstate modemstate, 
                                       swi_bool apprunning,
                                       enum SWI_TYPE_Device devtype,
                                       void *userp ),
                           struct hpmdmreghndl *reghndlp,
                           char *namep,
                           void *userp )
{
    struct hpphevtblock  *eventbkp; /* Stores HPPH Task Request parms */
    struct hpmdmregevt   *regevtp;  /* Modem state chg union member */

    /* Pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Get an event block pointer */
    eventbkp = (struct hpphevtblock *) mmbufget( &hpphcbp->hpphevtpool);
    
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        regevtp = &eventbkp->hpevtmember.hpmdmreg;

        /* Fill in the contents of the event block */
        eventbkp->hpphevent         = HPMDMCHGREQ; 
        eventbkp->hpphmemfreep      = NULL; /* Nothing to release */
        regevtp->hpmdmreghndlp      = reghndlp;
        regevtp->hpmdmcbfp          = cbfp;
        regevtp->hpmdmuserp         = userp;
        regevtp->hpmdmnamep         = (swi_uint8 *)namep;
        
        /* Send the new event to the HP LB task  */
        icsmt_send( &hpphcbp->hpphicmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("hpmdmreg: Out of memory", 0);
    }
}

/*************
 *
 * Name:    hpprocmdmreg
 *
 * Purpose: This function processes incoming requests by external 
 *          packages to register to receive callbacks when the 
 *          modem's enumeration status changes
 *             
 * Parms:   mdmregreq  Pointer to the hpmdmchgevt part of the 
 *                     event block containing the caller's request info
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This function runs inside the HPPH task's context. It is
 *          the counterpart to hpmdmregister
 *
 **************/
package void hpprocmdmreg( struct hpmdmregevt *mdmregreq )
{
    struct hpmdmreghndl *hndlp;
    struct hpmdmstreg   *regcbp;
    struct qllink       *qlp;
    enum   SWI_TYPE_Device devicetype;
    enum   hpmdmstate mdmstate;

    /* Pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Initialize the pointer to the registration control block */
    regcbp = &hpphcbp->hpphmdmregcb;

    /* Get a pointer to the caller-supplied handle */
    hndlp = mdmregreq->hpmdmreghndlp;

    /* Initialize the caller's registration block queue link */
    qlp = &hndlp->hpmdmregql;
    qlinitlnk( qlp, (void *) hndlp );
    
    /* Store the remaining information in the handle structure */

    /* Validate the caller's handle. This field should
     * never change. Since the caller provides this 
     * memory we don't trust them to keep it safe so 
     * we put our own token in there
     */
    hndlp->hpmdmvalid = HPMDMVALIDATER;

    /* First the caller-supplied name string */
    hndlp->hpmdmregname = mdmregreq->hpmdmnamep;

    /* Now save the callback function and user parameter */
    hndlp->hpmdmcbfp = mdmregreq->hpmdmcbfp;
    hndlp->hpmdmuser = mdmregreq->hpmdmuserp;
    
    /* Link it into the queue */
    qladdprev( &regcbp->hpmdmreghead, qlp );

    /* Learn the current state of the modem 
     * by trying to get the device type 
     */
    devicetype = ddgetdevtype();

    if( devicetype == SWI_DEVICE_None )
    {
        /* No device at the moment */
        mdmstate = HPMDMOFFLINE;
    }
    else
    {
        mdmstate = HPMDMONLINE;
    }
    
    /* Immediate callback to synchronize the registrant */
    hndlp->hpmdmcbfp( mdmstate, regcbp->hpmdmapprunning, 
                      devicetype, hndlp->hpmdmuser );
}

/*************
 *
 * Name:    hpmdmkick
 *
 * Purpose: Called whenever a modem status change is reported to us
 *          so that we can run down the chain of registered packages
 *          to inform them of the new modem state. 
 *             
 * Parms:   mdmstate   - HPMDMOFFLINE or HPMDMONLINE
 *          apprunning - if mdmstate is HPMDMONLINE, this boolean 
 *                       indicates whether the modem is executing 
 *                       its boot image (FALSE) or application (TRUE)
 *          devicetype - One of the device types defined in the 
 *                       AM package, amudefs.h. Specifically IDs
 *                       the type of modem that is connected or
 *                       AM_DEVICE_None if there is no connected device
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This function runs inside the HPPH task's context
 *
 **************/
package void hpmdmkick( enum hpmdmstate mdmstate, 
                        swi_bool apprunning,
                        enum SWI_TYPE_Device devicetype )
{
    swi_uint32        queuepos;
    struct hpmdmstreg   *regcbp;
    struct hpmdmreghndl *hndlp;
    struct qllink    *curlinkp;

    /* Pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Initialize the pointer to the registration control block */
    regcbp = &hpphcbp->hpphmdmregcb;

    /* Store the current value of apprunning */
    regcbp->hpmdmapprunning = apprunning;

    /* Go through the list of packages who registered
     * for modem update notifications
     */
    queuepos = 0;

    /* Set the initial link to point to the head of the queue */
    curlinkp = &regcbp->hpmdmreghead;

    while( (hndlp = qlgetnext( curlinkp )) != NULL )
    {
        /* Update curlinkp for the next time through */
        curlinkp = &hndlp->hpmdmregql;

        /* For debugging purposes, keep track of the entry we are kicking */
        queuepos += 1;

        /* Validate the caller's handle before use */
        if( hndlp->hpmdmvalid == HPMDMVALIDATER )
        {
            /* Call this block's registered callback function */
            hndlp->hpmdmcbfp( mdmstate, apprunning, 
                              devicetype, hndlp->hpmdmuser );
            
            if( mdmstate )
            {
                /* Log that we've done this */
                dlLog(  &hpphcbp->hpphdluser, HPLOG_CLASSA, 
                        "Called %s: Device: %d online, executing %d",
                        (swi_uint32)hndlp->hpmdmregname, 
                        (swi_uint32)devicetype, 
                        (swi_uint32)apprunning,
                        (swi_uint32)NULL);
            }
            else
            {
                /* Log that we've done this */
                dlLog(  &hpphcbp->hpphdluser, HPLOG_CLASSA, 
                        "Called %s: modem unavailable", 
                        (swi_uint32)hndlp->hpmdmregname,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL);
            }
        }
        else
        {
            /* Should never happen */
            erAbort("HPMDMKICK: Corrupt handle", queuepos);
        }
    }
}

/*************
 *
 * Name:    hpmdminit
 *
 * Purpose: Initialize the mdm subsection of the HPPH task. 
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
package void hpmdminit( void )
{
    struct hpmdmstreg *mdmregcbp;

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Initialize the local pointer to the modem reg ctl block */
    mdmregcbp = &hpphcbp->hpphmdmregcb;

    /* Initially, we don't know the modem's apprunning state, 
     * it could be running in the boot loader or in the 
     * application, but for now, we'll initialize this field
     * to FALSE and leave it to the US Detection mechanism to 
     * update us when it knows
     */
    mdmregcbp->hpmdmapprunning = FALSE;

    /* Initialize the head of the registration queue */
    qlinitlnk( &mdmregcbp->hpmdmreghead, NULL );
}

/*************
 *
 * Name:    hpgetairserverstatus
 *
 * Purpose: Reports the current air server status 
 *             
 * Parms:   None
 *
 * Return:  TRUE - if air server present
 *          FALSE - otherwise
 *
 * Abort:   None
 *
 * Notes:   None
 *
 **************/
global swi_bool hpgetairserverstatus(void)
{
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Return the current air server status */ 
    return (hpphcbp->hpphairstate.hpairserverpresent);
} 

/*
 * $Log: hpmdmstate.c,v $
 */

