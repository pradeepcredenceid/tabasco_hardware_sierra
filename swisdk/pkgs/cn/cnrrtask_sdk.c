/*************
 *
 * $Id: cnrrtask_sdk.c,v 1.22 2010/06/10 00:03:53 epasheva Exp $
 *
 * Filename: cnrrtask_sdk.c
 *
 * Purpose:  CnS Request Response task and associated functions
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "cn/cnidefs.h"

/* Defines */

/* This next structure is a dummy event block which will
 * be sent to this CN RR task at a fixed interval by the
 * IC package timer task service. 
 */
struct cnsevtblock cntimerevt = { CNRR_TMRTICK, NULL, NULL };

/* Local storage */
struct cnrrtcb cnsrrtaskblk;

/*************
 *
 * Name: cngetrrcbp
 *
 * Purpose: Function to get a pointer to the CnS Request/Response 
 *          thread's control block structure.
 *             
 * Parms:   none
 *
 * Return:  pointer to struct cnsrrtcb
 *
 * Abort:   none
 *
 * Notes:   None
 *
 **************/
package struct cnrrtcb *cngetrrcbp( void )
{
    return &cnsrrtaskblk;
}

/*************
 *
 * Name: cnRelEventbk
 *
 * Purpose: To release the resources assocated with the CnS event block,
 *          passed to this task from the HIP Receive task
 *             
 * Parms:   eventbkp - Pointer to the CnS Event Block structure, received
 *                     from the HIP Receive task
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   This function releases the resources associated with the 
 *          CnS event block as follows: first the memory buffer containing
 *          the newly received CnS packet is released. Next the event buffer
 *          itself is released. Both are allocated from pools created at
 *          system startup time by this CnS RR task. 
 *
 **************/
local void cnRelEventbk( struct cnsevtblock *eventbkp )
{
    /* If the caller's memory can be freed... */
    if( eventbkp->cnevtmemfreep != NULL )
    {
        /* Free the CnS message buffer first */
        mmbufrel( eventbkp->cnevtmemfreep );
    }

    /* Finally, free the event block itself */
    mmbufrel( (swi_uint8 *)eventbkp );
}

/*************
 *
 * Name: cnsClearRespInfo
 *
 * Purpose: To clear the information about the current outbound request.
 *             
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Calling this function causes the cnmdmwaittmr to be stopped
 *          if it isn't already
 *
 **************/
local void cnsClearRespInfo( void )
{
    struct cnswdata *swinfop;

    /* Get the CnS RR control block pointer */
    struct cnrrtcb *cnrrcbp = cngetrrcbp();

    swinfop = &cnrrcbp->cnswdata;

    /* Clear out the fields */
    swinfop->cnsbusy = FALSE;
    swinfop->cnsobjid = 0;
    swinfop->cnsoperation = 0;
    swinfop->cnsipcchannel = 0xFF;

    /* Stop the timer */
    cnrrcbp->cnmdmwaittmr = 0;
}

/*************
 *
 * Name: cnsSaveRespInfo
 *
 * Purpose: To store the information about the current outbound request
 *          so that when the response comes in we can validate that its
 *          the correct one for the stop-and-wait protocol. This function
 *          converts the outgoing operation type into the expected
 *          incoming operation response
 *             
 * Parms:   cnsobjid      - The expected object ID is the same as what was sent
 *          cnsoperation  - The operation sent is used to derive the expected
 *                          contents of the response packet's field
 *          timeout       - The time limit the user requested in the API call
 *                          for the modem to respond, in milliseconds
 *          cnsparameter  - This is used to store the IPC channel that the
 *                          CN request came in from. In the event of a timeout
 *                          the response is sent to this channel.
 *
 * Return:  none
 *
 * Abort:   Unexpected cnsoperation 
 *          Stop and wait violation
 *
 * Notes:   Assumes all the entries in the case statement below are meant
 *          to be synchronous operations
 *
 *          This function starts the internal modem response timer using
 *          the timeout value specified by the caller in the API 
 *
 **************/
local void cnsSaveRespInfo( 
    swi_uint16 cnsobjid, 
    swi_uint8 cnsoperation, 
    swi_uint32 timeout,
    swi_uint32 cnsparameter)
{
    struct cnswdata *swinfop;
    enum cnsoptypes expectedop;
    swi_uint32 ticks;

    /* Get the CnS RR control block pointer */
    struct cnrrtcb *cnrrcbp = cngetrrcbp();

    swinfop = &cnrrcbp->cnswdata;

    /* Check for an existing stop and wait operation */
    if( swinfop->cnsbusy )
    {
        /* Stop and wait already busy - Fatal Error */
        erAbort("cnsSaveRespInfo: Stop-and-wait violation", cnsobjid );
 
        return; /* Not needed, but for completeness */
    }

    /* First, figure out what the expected operation is, 
     * based on the outgoing one
     */
    switch( cnsoperation )
    {
        case CNS_GETREQ:
            expectedop = CNS_GETRESP;
            break;

        case CNS_SETREQ:
            expectedop = CNS_SETRESP;
            break;

        case CNS_NOTREQ:
            expectedop = CNS_NOTRESP;
            break;

        case CNS_STOPREQ:
            expectedop = CNS_STOPRESP;
            break;

        case CNS_CHNGREQ:
            expectedop = CNS_CHNGRESP;
            break;

        default:
            erAbort("cnsSaveRespInfo: Invalid operation", cnsoperation );
            break;
    }

    /* Store the information */
    swinfop->cnsbusy = TRUE;
    swinfop->cnsobjid = cnsobjid;
    swinfop->cnsoperation = expectedop;
    swinfop->cnsipcchannel = cnsparameter;

    /* Calculate the number of ticks to use, for the given timeout */
    ticks = (timeout/ICTTTICK) + 1;        /* 1 tick minimum */

    /* Start the timer */
    cnrrcbp->cnmdmwaittmr = ticks;
}

/*************
 *
 * Name: cnShortResp
 *
 * Purpose: Build and send a short response to the API side. This
 *          function is used to send a reply to a request received
 *          from the API side when the request cannot be complied
 *          with in the normal manner
 *             
 * Parms:   resultcode - An AM packet result code to be stuffed into
 *                       the returned packet
 *          ipcchannel - IPC channel to send Short response to.
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local void cnShortResp( enum swi_amresults resultcode, swi_uint32 ipcchannel )
{
    struct amcnsrrparms cnsargs; /* CnS Arguments from the CNSREQUEST packet */
    swi_uint16          outipcsize; /* Length (bytes) of the outgoing IPC msg */
    swi_uint8           outbuf[AMCNSRESPTOTAL]; /* temp storage for response */
    swi_uint8           *outbufp;   /* working pointer to outbuf */
    struct cnrrtcb *cnrrcbp = cngetrrcbp();

    outbufp = outbuf;

    /* Fill the cns argument structure with NULLS for 
     * completeness, then set the result code to indicate the 
     * timeout occurred. The reader needs to realize that none
     * of the fields are valid if it receives a timeout 
     * response
     */
    slmemset( (char *)&cnsargs, 0x0, sizeof( struct amcnsrrparms ) );

    /* Now set the result code to indicate a timeout occurred */
    cnsargs.amresultcode = resultcode;

    /* Build the response packet, empty except for the timeout
     * indication. This call modifies the address pointed to by
     * outbufp
     */
    outipcsize = ambuildcnsresponse( &outbufp, &cnsargs );

    /* Log this particular short response transaction */
    dlLog(  &cnrrcbp->cndlcb, CNLOG_CLASSC, "cnShortResp( %d, ch: %d)", 
            (swi_uint32)resultcode, 
            (swi_uint32)ipcchannel,
            (swi_uint32)NULL,
            (swi_uint32)NULL);

    /* Send this packet to the host. On return the input data buffer
     * is done with and can be released. Don't forget to use the 
     * original pointer - outbuf - instead of outbufp here
     */
    amsdk_send( outbuf, outipcsize, (swi_uint8) ipcchannel);
}

/*************
 *
 * Name: cnsValidateResponse
 *
 * Purpose: To compare the new modem CnS response against the expected
 *          values. 
 *             
 * Parms:   cnsobjid     - The expected object ID from the modem packet
 *          cnsoperation - The operation type from the new packet
 *
 * Return:  TRUE  - The new modem CnS packet matches the expected one
 *          FALSE - The new CnS packet does not match
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local swi_bool cnsValidateResponse( swi_uint16 cnsobjid, 
                                    enum cnsoptypes cnsoperation )
{
    struct cnswdata *swinfop;
    swi_bool validated = FALSE;    /* Assume the packets don't match */

    /* Get the CnS RR control block pointer */
    struct cnrrtcb *cnrrcbp = cngetrrcbp();

    /* Get a pointer to the stored expected values */
    swinfop = &cnrrcbp->cnswdata;

    /* If busy not set then the rest of the fields are 
     * meaningless. Likely cause is a timeout occurred 
     * and this incoming response arrived after
     */
    if( swinfop->cnsbusy )
    {
        /* Compare the incoming values with the stored ones */
        if( (swinfop->cnsobjid == cnsobjid ) &&
            (swinfop->cnsoperation == cnsoperation) )
        {
            /* The incoming packet is the one we were waiting for */
            validated = TRUE;

            /* No longer busy so clear the transaction */
            cnsClearRespInfo();
        }
        /* We got a response, but not the one we were expecting */
        else
        {
            /* TBD what to do here */
        }
    }
    /* Unexpected response. Probably caused 
     * by a timeout while waiting for it
     */
    else
    {
        /* Log that a stray packet was received */
    }
    return validated;
}

/* 
 * Name: cnsGetparamp
 *
 * Purpose: Get a pointer to the parameter field of a CnS packet given 
 *          a pointer to the header.
 *
 * Parms:   cnshdrp - ptr to beginning of CnS packet header
 *
 * Return:  pointer to the start of the parameter field
 *
 * Abort:   none
 *
 * Notes:   The header fields are assumed to be in Network Byte Order
 *
 */
local swi_uint8 *cnsGetParamp( swi_uint8 *cnshdrp )
{
    /* The CnS header is a fixed size */
    return cnshdrp + CNSHEADERSZ;
}


/* 
 * Name: cnsParseHeader
 *
 * Purpose: Parse a CnS header, placing its fields into the calling
 *          arguments provided
 *
 * Parms:   cnshdrpp       - ptr to ptr to beginning of CnS packet header
 *          cnsobjidp      - ptr to storage for the Cns object ID
 *          cnsoperationp  - ptr to storage for the incoming operation
 *          cnsparameterp  - ptr to storage for the Cns parameter
 *          cnslengthp     - ptr to storage for the CnS length
 *          cnserrflagp    - ptr to a boolean which will be TRUE if
 *                           there was an error flag and FALSE otherwise
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Called from within the CnS Request Response task
 *
 *          The header fields are assumed to be in Network Byte Order
 *
 *          This function parses a CnS header as defined in section
 *          section 3.2 "CnS message packet, R1" of Sierra Wireless 
 *          document "2110636 - CWE Control and Status Protocol"
 */
local void cnsParseHeader( swi_uint8 **cnshdrpp, 
                                 swi_uint16 *cnsobjidp, 
                                 swi_uint8 *cnsoperationp, 
                                 swi_uint32 *cnsparameterp, 
                                 swi_uint16 *cnslengthp,
                                 swi_bool   *cnserrflagp )
{
    swi_uint8 *localhdrp;  /* Local copy of the header storage pointer */
    swi_uint8 operation;   /* local storage for the Op'n and error flag field */
    swi_uint8 dummybyte;   /* For reading the spare fields */
    swi_uint32 bytesread;  /* For verification purposes */

    /* Make a local copy of the incoming pointer */
    localhdrp = *cnshdrpp;

    /* Validate the parameters */

    /* First field contains the object ID */
    *cnsobjidp = piget16( &localhdrp );

    /* The operation field is next */
    operation = *localhdrp++;

    /* Separate the error flag and the operation */
    *cnsoperationp = operation & CNSOPERATIONMASK;
    *cnserrflagp = (operation & CNSERRORFLAGMASK)?TRUE:FALSE;

    /* Reserved Byte A is always zero-filled */
    dummybyte = *localhdrp++;

    /* Application Parameter field is next */
    *cnsparameterp = piget32( &localhdrp );

    /* The length field comes last */
    *cnslengthp = piget16( &localhdrp ); 

    /* Calculate the number of bytes we've read */
    bytesread = localhdrp - *cnshdrpp;

    /* The following check is there to make sure the defined
     * constant matches the header size. The constant is used
     * elsewhere and it must be exactly the same as the header
     * size, so this is a convenient place to do the check
     */
    if( bytesread != CNSHEADERSZ )
    {
        erAbort("cnsParseHeader: constant out of date", CNSHEADERSZ );
    }

    /* Advance the caller's pointer */
    *cnshdrpp += (localhdrp - *cnshdrpp);
}

/* 
 * Name: cnsBuildHeader
 *
 * Purpose: Build a CnS header into the supplied buffer space using 
 *          the supplied information
 *
 * Parms:   cnshdrp      - Pointer to storage for the CnS header
 *          cnsobjid     - The object ID to put into the header
 *          cnsoperation - The operation, Get, Set, etc. 
 *          cnsparameter - The value to fill into the parameter field
 *          cnslength    - Length of the parameter section
 *
 * Return:  size of the header just created, in bytes, not including
 *          the CnS Parameter field length
 *
 * Abort:   none
 *
 * Notes:   Called from within the CnS Request Response task
 *
 *          The header fields are built in network byte order
 *
 *          This function builds a CnS header as defined in section
 *          section 3.2 "CnS message packet, R1" of Sierra Wireless 
 *          document "2110636 - CWE Control and Status Protocol"
 */
local swi_uint32 cnsBuildHeader( swi_uint8 *cnshdrp, 
                                 swi_uint16 cnsobjid, 
                                 swi_uint8 cnsoperation, 
                                 swi_uint32 cnsparameter, 
                                 swi_uint16 cnslength )
{
    swi_uint8 *localhdrp;  /* Local copy of the header storage pointer */

    localhdrp = cnshdrp;

    /* Validate the parameters */

    /* First field contains the object ID */
    piput16( cnsobjid, &localhdrp );

    /* The operation field is next. For host-to-modem 
     * packets, the error bit is always zero 
     */
    *localhdrp++ = cnsoperation;

    /* Reserved Byte A is always zero-filled */
    *localhdrp++ = 0;

    /* Application Parameter field is next */
    piput32( cnsparameter, &localhdrp );

    /* Reserved Byte B is zero-filled */
    *localhdrp++ = 0;

    /* The length field comes last */
    *localhdrp++ = (swi_uint8) cnslength;

    /* Done, return the length of the CnS header just built */
    return (swi_uint8) (localhdrp - cnshdrp);
}

/*************
 *
 * Name: cnsreadoptype
 *
 * Purpose: Read the CnS Operation type from its field in the CnS
 *          packet header. A pointer to this header is passed in 
 *          as the calling argument.
 *             
 * Parms:   cnshdrp    Pointer to the CnS message header whose 
 *                     operation type needs to be read
 *
 * Return:  One of enum cnsoptypes
 *
 * Abort:   Unrecognized CnS operation type
 *
 * Notes:   Runs in the context of the HIP Receive task as part of 
 *          handling the incoming message delivery 
 *
 *          Leaves the CnS message intact. The function only reads
 *          the contents of the CnS operation field from the header
 *
 **************/
local enum cnsoptypes cnsreadoptype( swi_uint8 *cnshdrp )
{
    swi_uint8  optype;

    optype = *(cnshdrp + CNSOFFSET_OPTYPE) & CNSOPERATIONMASK;

    if( optype >= CNS_MINOPERATION &&
        optype < CNS_MAXOPERATION )
        return optype;

    erAbort("CNSREADOT: Invalid CnS op'n", optype );
    return 0;
}

/*************
 *
 * Name: cnHandleNotify
 *
 * Purpose: Function called by cnRxPacket() when a notification is 
 *          received from the modem. It is this function's job to 
 *          accept the Cns notification packet, build an AM Notification
 *          packet and forward it to the API side via the Notification
 *          IPC channel
 *             
 * Parms:   msgdesignator is the HIP message ID we originally registered for,
 *                     must be HPHCNSIND always.
 *          memrelp    Pointer to use when releasing the packet containing
 *                     the CnS message
 *          cnsheaderp Pointer to the beginning of the CnS header
 *          cnslength  is the length in bytes of the data pointed-to by 
 *                     payloadp including the CnS parameter field
 *          errorflag  TRUE  - the message is incomplete
 *                     FALSE - The message has no errors
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Runs in the context of the HIP Receive task.
 *          message delivery registered function. 
 *
 **************/
local void cnHandleNotify( swi_uint32 msgdesignator, swi_uint8 *memrelp, 
                           swi_uint8 *cnshdrp, swi_uint16 cnslength, 
                           swi_bool errorflag )
{
    swi_uint8 *cnsresp;    /* Local copy of incoming message */
    swi_uint8 *outsendp;   /* Ptr to AM message packet bound for API side */
    swi_uint8 *outbufp;    /* Ptr to working copy of outsendp */
    swi_uint16 outipcsize; /* Length (bytes) of the outgoing IPC msg */
    struct amcnsrrparms cnsargs; /* CnS Arguments from the Notif'n packet */

    /* Get the CnS control block pointer */
    struct cnrrtcb *cnrrcbp = cngetrrcbp();

    /* Initialize the local copy pointer to the incoming message */
    cnsresp = cnshdrp;

    /* Parse the incoming CnS header, advancing the 
     * cnsresp pointer to the start of the parameter
     * area
     */
    cnsParseHeader( &cnsresp, 
                    &cnsargs.amcnsobjid, 
                    &cnsargs.amcnsoperation, 
                    &cnsargs.amcnsparameter, 
                    &cnsargs.amcnslength, 
                    &errorflag );

    /* Set the result code with the good news */
    cnsargs.amresultcode = SWI_AMSUCCESS;

    /* Compute a pointer to the start of the AM packet header
     * using the pointer to the CnS Parameter section as the
     * start point. We want to subtract the AMCNSNOTIFICATION
     * packet header size from the CnS Parameter pointer. 
     */
    outbufp = amgethdrptr( cnsresp, AMCNSNOTIFICATION, memrelp );
 
    /* Make a copy of this for when we send it to the app'n */
    outsendp = outbufp;

    /* Build the AM response packet. Note, the outbufp
     * pointer is advanced to the start of the CnS parameter
     * field on return so there is nothing additional to do
     * after this AM packet is built.
     */
    outipcsize = ambuildcnsnotify( &outbufp, &cnsargs );

    /* Include the length of the CnS Parameter field */
    outipcsize += cnsargs.amcnslength;

    /* Log this event */
    dlLog(  &cnrrcbp->cndlcb, CNLOG_CLASSA, 
            "SDK<-Mdm: Objid/Op'n: %04x/%d", 
            (swi_uint32)cnsargs.amcnsobjid,
            (swi_uint32)cnsargs.amcnsoperation,
            (swi_uint32)NULL,
            (swi_uint32)NULL );

    /* Send this packet to the host, use the pointer to the 
     * very start of the buffer
     */
     amsdk_sendcnsnot( outsendp, outipcsize, cnsargs.amcnsobjid);

    /* All done, release the memory holding the incoming message */
    mmbufrel( memrelp );
}

/*************
 *
 * Name: cnRxPacket
 *
 * Purpose: Entry point in CnS package for receiving a CnS packet
 *          from the modem. The packet needs to be further narrowed
 *          down in terms of its operation. Notifications take a 
 *          different route compared to Get/Set responses
 *             
 * Parms:   msgdesignator is the HIP message ID we originally registered for,
 *                     must be HPHCNSIND always.
 *          msgparm    is the byte contained in the HIP message parameter
 *                     field. Unused in this case.
 *          memrelp    Pointer to use when releasing the packet containing
 *                     the CnS message
 *          cnsheaderp Pointer to the beginning of the CnS header
 *          cnslength  is the length in bytes of the data pointed-to by 
 *                     payloadp including the CnS parameter field
 *          errorflag  TRUE  - the message is incomplete
 *                     FALSE - The message has no errors
 *
 * Return:  none
 *
 * Abort:   Out of event blocks 
 *
 * Notes:   Runs in the context of the HIP Receive task as the incoming
 *          message delivery registered function. 
 *
 *          This function receives new CNS messages from the modem
 *          via the next lower layer of the protocol stack (HIP, in 
 *          this case)
 *
 **************/
local void cnRxPacket( swi_uint32 msgdesignator, swi_uint8 msgparm, 
                           swi_uint8 *memrelp, swi_uint8 *cnshdrp, 
                           swi_uint16 cnslength, swi_bool errorflag )
{
    struct cnsevtblock *eventbkp;

    /* Get the CnS RR control block pointer */
    struct cnrrtcb *cnrrcbp = cngetrrcbp();

    /* Determine what type of packet this is and 
     * send it to the appropriate destination
     */

    /* If it's a notification (unsolicited message) */
    if( cnsreadoptype( cnshdrp ) == CNS_NOTIFY )
    {
        /* Log that we got a notification */

        /* Build an IPC packet and send 
         * this notification to the application
         * via the Notification IPC Channel
         */
        cnHandleNotify( msgdesignator, memrelp, cnshdrp, cnslength, errorflag );
    }

    /* It's a stop-and-wait response of some kind */
    else
    {
        /* Log that we got a Response packet */

        /* Get an event block pointer */
        eventbkp = (struct cnsevtblock *) mmbufget( &cnrrcbp->cnevtpool );
    
        /* Don't want NULL event block pointers */
        if( eventbkp )
        {
            eventbkp->cneventtype = CNRR_HPMSG;
            eventbkp->cnevtdatap = cnshdrp;
            eventbkp->cnevtmemfreep = memrelp;
    
            /* Send the new packet to the CnS Request/Response task */
            icsmt_send( &cnrrcbp->cnicmsgque, (swi_uint32) eventbkp );
        }
        /* NULL Block */
        else
        {
            /* This should never happen */
            erAbort("CNSRCVPKT: Out of memory", 0);
        }
    }
}

/*************
 *
 * Name: cncnsreq
 *
 * Purpose: Entry point in CnS package for sending a packet to the 
 *          modem. First checks to ensure Stop-and-Wait protocol is
 *          being obeyed. Saves some information and then sends the
 *          packet on its way
 *             
 * Parms:   packetp      - Pointer to a byte-field containing the 
 *                         CnS message parameter specific to this request
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   On return from this function, the memory containing the 
 *          parameter section can be deleted, if applicable
 *
 *          Runs in the context of the CnS Request/Response thread
 *
 **************/
local void cncnsreq( swi_uint8 *packetp )
{
    swi_uint32 packetsize;     /* Size of the packet to be sent to the modem */
    struct amcnsrrparms cnsparm; /* CnS Arguments from the CNRR_IPC packet */
    swi_uint8  *inbufp;        /* Local copy of inputbuf and response buf */
    swi_uint8 *sendbufp;       /* Pointer to storage for outgoing buffer */
    swi_uint8 *cnsheaderp;     /* Pointer to CnS Header area of sendbufp */
    swi_uint8 *cnsparmfieldp;  /* Pointer to CnS parameter area of sendbufp */

    /* Get the CnS RR control block pointer */
    struct cnrrtcb *cnrrcbp = cngetrrcbp();

    /* Make a local copy of the incoming packet pointer */
    inbufp = packetp;

    /* Parse the contents of the CNS request packet, 
     * advancing the packet pointer in the process
     */
    amparsecnsrequest( &inbufp, &cnsparm );

    /* Log this new request now that we know some details */
    dlLog( &cnrrcbp->cndlcb, CNLOG_CLASSA, 
           "SDK->Mdm: ch/Objid/Op'n: %d/%04x/%d",
           (swi_uint32)cnsparm.amcnsparameter,  
           (swi_uint32)cnsparm.amcnsobjid, 
           (swi_uint32)cnsparm.amcnsoperation,        
           (swi_uint32)NULL);

    /* Ask the HIP layer for a buffer to build the message into a buffer */
    sendbufp = (swi_uint8 *)hpgetsizedbuf((cnsparm.amcnslength+CNMAXCNSHDRSZ));

    /* Skip past the HIP header section */
    cnsheaderp = hpgetpayp( sendbufp, HPMCNSIND );

    /* Build the CnS header */
    packetsize = cnsBuildHeader( cnsheaderp, 
                                 cnsparm.amcnsobjid, 
                                 cnsparm.amcnsoperation, 
                                 cnsparm.amcnsparameter, 
                                 cnsparm.amcnslength );

    /* Store key information used to validate responses, 
     * NOTE, this will abort if there's a stop and wait
     * violation. Note, calling this function also starts
     * the internal response timer
     */
    cnsSaveRespInfo( cnsparm.amcnsobjid, cnsparm.amcnsoperation, 
        cnsparm.amcnstimeout, cnsparm.amcnsparameter );

    /* Only copy the parameter field if its length is non-zero */
    if( cnsparm.amcnslength )
    {
        /* Get the offset to the Parameter section */
        cnsparmfieldp = cnsGetParamp( cnsheaderp );

        /* Copy in the parameter section */
        slmemcpy( cnsparmfieldp, inbufp, cnsparm.amcnslength );
    }

    /* The packet is ready for transmission - 
     * don't forget to use the sendbufp pointer. 
     * Note that it's the HP package's job to 
     * release the send buffer when it's finished
     * transmitting to the modem
     */
    hpTxPacket( HPMCNSIND, 
                HPNULLPARMFIELD, 
                sendbufp, 
                packetsize + cnsparm.amcnslength );
    
    /* All done here */
}

/*************
 *
 * Name: cnsendrr
 *
 * Purpose: Send a new IPC message to the CnS RR task 
 *             
 * Parms:   inipcmsgp - Pointer to the Message Data portion of the
 *                      inbound (i.e. from the application) AM message
 *          memfreep  - Pointer to the memory release address for the
 *                      inbound message. CN RR task will manage the
 *                      release of this packet when done processing it
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Allocate an event block, then store the calling arguments
 *          into the appropriate fields and send the packet to the 
 *          CnS task. Buffer releasing is the responsibility of the 
 *          CnS RR task, callers must not do this
 *
 *          This message sends a packet to the CNS RR task. The 
 *          counterpart handler function within that task is "cncnsreq()"
 *
 **************/
global void cnsendrr( swi_uint8 *inipcmsgp, swi_uint8 *memfreep )
{
    struct cnsevtblock *eventbkp;
    struct cnrrtcb *cnrrcbp = cngetrrcbp();

    /* Allocate an event block */
    eventbkp = (struct cnsevtblock *) mmbufget( &cnrrcbp->cnevtpool );

    if( eventbkp )
    {
        /* Stuff the fields with the required information */
        eventbkp->cneventtype = CNRR_IPCMSG;

        /* The CnS information comes next */
        eventbkp->cnevtdatap = inipcmsgp;

        /* Save the memory release pointer */
        eventbkp->cnevtmemfreep = memfreep;

        /* Send the message on its way */
        icsmt_send( &cnrrcbp->cnicmsgque, (swi_uint32) eventbkp );
    }
    else
    {
        erAbort("cnsendrr: no event blocks", 0 );
    }
}

/*************
 *
 * Name: cncnship
 *
 * Purpose: Handle incoming CnS responses from the modem via the 
 *          HIP receive task. 
 *             
 * Parms:   cnspacketp  - Pointer to the CnS header portion of the newly
 *                        received packet
 *          pktstartp   - Pointer to the start of the buffer containing
 *                        the newly-received packet
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Called when the CnS RR task receives a CNRR_HPMSG 
 *          intertask message
 *
 *          On return from this call, it is guaranteed safe to release the
 *          resources associated with the event block
 *          
 **************/
local void cncnship( swi_uint8 *cnspacketp, swi_uint8 *pktfreep )
{
    swi_uint8           *outbufp;  /* ptr to response buf to send back to app */
    swi_uint8           *outsendp; /* ptr to response buf to send back to app */
    swi_uint8           *cnsresp;  /* Working copy of rmsgp */
    swi_uint16          outipcsize; /* Length (bytes) of the outgoing IPC msg */
    swi_bool            errorflag = 0; /* Error flag from the incoming packet */
    swi_bool            validated; /* If the resp pkt is the expected one */
    struct amcnsrrparms cnsargs; /* CnS Arguments from the CNSREQUEST packet */
    struct cnrrtcb *cnrrcbp = cngetrrcbp();

    /* Got a response from the modem, make sure 
     * it's the one we are waiting for and then 
     * build the response to the application. 
     * Note, the memory pointed to by rmsg needs to be 
     * released when we're done processing the incoming 
     * packet
     */

    /* Get a pointer to the new CNS Packet header */
    cnsresp = cnspacketp;

    /* Parse the incoming CnS header, advancing the 
     * cnsresp pointer to the start of the parameter
     * area
     */
    cnsParseHeader( &cnsresp, 
                    &cnsargs.amcnsobjid, 
                    &cnsargs.amcnsoperation, 
                    &cnsargs.amcnsparameter, 
                    &cnsargs.amcnslength, 
                    &errorflag );

    /* Perform the check to make sure this incoming 
     * packet is the one we are expecting. If validation
     * fails, we'll log a stray packet and silently drop it
     */
    validated = cnsValidateResponse( cnsargs.amcnsobjid, 
                                     cnsargs.amcnsoperation );

    /* If validated, it means this is the expected modem response */
    if( validated )
    {
        /* Set the result code with the good news */
        cnsargs.amresultcode = SWI_AMSUCCESS;

        /* Need to check the error flag returned from 
         * the header parsing routine 
         */
        if( errorflag )
        {
            /* This changes the return code the Application gets */
            cnsargs.amresultcode = SWI_AMCNSERROR;
            /* Ensure there is a NULL termination on this string */
            cnsresp[ cnsargs.amcnslength ] = 0;
            /* Increment the length to account for the NULL */
            cnsargs.amcnslength += 1;
            dlLog( &cnrrcbp->cndlcb, CNLOG_CLASSC, 
                   "Mdm reports CnS Error: \"%s\"\n", 
                   (swi_uint32)cnsresp,
                   (swi_uint32)NULL,
                   (swi_uint32)NULL,        
                   (swi_uint32)NULL);
        }

        /* Compute a pointer to the start of the AM packet header
         * using the pointer to the CnS Parameter section as the
         * start point. We want to subtract the AMCNSRESPONSE
         * packet header size from the CnS Parameter pointer. 
         *
         * We do this to save a copy operation which we would have 
         * to do if we built the AM Response packet in a different 
         * buffer. In that case, it would be necessary to copy
         * the CnS parameter field from the incoming CnS response
         * into the different buffer
         */
        outbufp = amgethdrptr( cnsresp, 
                                   AMCNSRESPONSE, 
                                   pktfreep );
 
        /* Remember the address of the start of this message 
         * so we know where to start sending from later on
         */
        outsendp = outbufp;
    
        /* Build the AM response packet */
        outipcsize = ambuildcnsresponse( &outbufp, &cnsargs );

        /* Include the length of the CnS Parameter field */
        outipcsize += (cnsargs.amcnslength);
 
        dlLog( &cnrrcbp->cndlcb, CNLOG_CLASSA, 
               "SDK<-Mdm: ch/Objid/Op'n: %d/%04x/%d", 
               (swi_uint32)cnsargs.amcnsparameter, 
               (swi_uint32)cnsargs.amcnsobjid,
               (swi_uint32)cnsargs.amcnsoperation,        
               (swi_uint32)NULL);               

        /* Send this packet to the host, use the pointer to the 
         * very start of the buffer
         */
        amsdk_send( outsendp, outipcsize, cnsargs.amcnsparameter );
    }
    /* Didn't validate, probably because this packet arrived after
     * the timeout expired. Log the stray and release the resources
     * without bothering the application
     */
    else
    {
        /* Log a stray packet */
        dlLog( &cnrrcbp->cndlcb, CNLOG_CLASSC, 
               "Rcv'd stale modem response: ch/Objid/Op'n: %04x/%d",
               (swi_uint32)cnsargs.amcnsparameter, 
               (swi_uint32)cnsargs.amcnsobjid,
               (swi_uint32)cnsargs.amcnsoperation,
               (swi_uint32)NULL );
    }
}

/*************
 *
 * Name: cnmdmtimeout
 *
 * Purpose: Called if the cnmdmtmrwait timer expires, meaning that the
 *          modem has not responded to an outstanding CnS request 
 *          in the time specified by the caller of the API. Send back
 *          a timeout indication to the API side
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
local void cnmdmtimeout( swi_uint32 ipcchannel )
{
    
    /* Clear the expected response information. Even though
     * we are processing a timeout, there is nothing to stop
     * the expected response from arriving late. In that case
     * we would need to know whether to forward it or drop it.
     * We drop it if the contents of the expected response 
     * structure have been cleared
     */
    cnsClearRespInfo();

    /* Send the timeout reply to the api side */
    cnShortResp( SWI_AMTIMEOUT, ipcchannel);
}

/*************
 *
 * Name: cntick
 *
 * Purpose: Called once each time a timer tick message is received
 *          by the RR task. This function takes care of managing 
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
local void cntick( void )
{
    struct cnrrtcb *cnrrcbp = cngetrrcbp();

    /* Decrement the timer and if it reaches zero as a result, 
     * invoke the timeout function
     */
    if( cnrrcbp->cnmdmwaittmr )
    {
        if( --(cnrrcbp->cnmdmwaittmr) ==  0 )
        {
            /* Invoke the timer expiry function */
            cnmdmtimeout(cnrrcbp->cnswdata.cnsipcchannel);
        }
    }
}

/*************
 *
 * Name: cnrrtask
 *
 * Purpose: Code implementing the CnS Request/Response task which
 *          coordinates receiving requests from the application
 *          and eventually sending them to the modem. Since the CnS 
 *          protocol is stop-and-wait, the task must await a response
 *          from the modem (or a timeout) 
 *             
 * Parms:   paramp - pointer to data that may be passed in when this task
 *                   is started.
 *
 * Return:  never returns
 *
 * Abort:   none
 *
 * Notes:   This task blocks in two places, first where it waits for 
 *          an IPC message from the application. It accepts this message
 *          and formats it into a call to the modem. After sending it, 
 *          the task blocks in its second location, awaiting a response
 *          from the modem via the HIP Receive task. A timeout ensures
 *          this task doesn't wait for the modem's response forever.
 *
 **************/
local void cnrrtask( void *paramp)
{
    swi_bool releaseevt;    /* Some event blocks should not be released */
    swi_uint32 rmsg;        /* ptr to incoming CnS response from modem */
    struct cnsevtblock *eventbkp;

    /* Define and allocate the task control block */
    struct cnrrtcb *cnrrcbp = cngetrrcbp();
 
    /* Register with the HIP Receive task for CNS traffic */
    hpregisterhip( HPHCNSIND, cnRxPacket );

    /* Prepare for logging */
    dlregister( "CN", &cnrrcbp->cndlcb, FALSE );

    /* Initialize the SMT Message queue structure */
    icsmt_create( &cnrrcbp->cnicmsgque, "CNRRMESSAGEQ", CNRRMSGDEPTH );

    /* Create the Event Pool for exchanging messages from the HIP Rx task */
    mmpcreate( &cnrrcbp->cnevtpool, 
               "CNEVTPOOL", 
               CNRREVTPOOLSZ, 
               sizeof(struct cnsevtblock) );

    dlLog(  &cnrrcbp->cndlcb, 
            CNLOG_CLASSC, 
            "RR Task started\n",
           (swi_uint32)NULL,
           (swi_uint32)NULL,
           (swi_uint32)NULL,
           (swi_uint32)NULL );

    /* Register with the IC package to receive periodic timer ticks */
    ictttickreg( &cnrrcbp->cnicmsgque, (swi_uint32) &cntimerevt, &cnrrcbp->cnictthndl );

    /* Tell SU it's ok for the next task in line to go */
    sutaskinitcb( SDK_CNPKG );

    /* Main line of processing begins next */
    for(;;)
    {
        /* Reset the release event block flag */
        releaseevt = TRUE;

        /* Wait for an incoming IPC message then dispatch it.
         * Incoming messages from the HIP receive task are 
         * stored in MM pool buffers. We need to release this
         * storage when we've done with it. Timeout value is 
         * NULL to wait forever. We get our ticks from the 
         * IC timer task
         */
        icsmt_rcv( &cnrrcbp->cnicmsgque, CNNOTIMEOUT, &rmsg );

        if( rmsg )
        {
            /* Cast the data to an event block pointer */
            eventbkp = (struct cnsevtblock *) rmsg;

            /* Dispatch to the handler for this type of message */
            switch( eventbkp->cneventtype )
            {
                /* API-side to SDK-side CnS Request */
                case CNRR_IPCMSG:
                    cncnsreq( eventbkp->cnevtdatap );
                    break;

                /* Modem's response to CnS Request */
                case CNRR_HPMSG:
                    cncnship( eventbkp->cnevtdatap, eventbkp->cnevtmemfreep );
                    break;

                /* Periodic timer tick message */
                case CNRR_TMRTICK:
                    cntick();
                    releaseevt = FALSE;
                    break;

                default:
                    erAbort("cnrrtask: Invalid Event message",
                            rmsg);
                    break;
            }

            /* Message has been handled, release the associated resources */
            if( releaseevt )
                cnRelEventbk( eventbkp );
        }
        /* Do nothing if the message queue is empty or on timeout */
    }
}

/*************
 *
 * Name: cninit
 *
 * Purpose: Package initialization routine
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
global void cninit( void )
{
    /* Define and allocate the task control block */
    struct cnrrtcb *cnrrcbp = cngetrrcbp();

    /* Ensure the modem wait timer is disabled initially */
    cnrrcbp->cnmdmwaittmr = 0;
}

/*************
 *
 * Name: cnrrtaskinit
 *
 * Purpose: Creates the task by making it known to the os
 *          package.
 *             
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Must be called *after* cninit() has been called
 *
 **************/
global void cnrrtaskinit( void )
{
    /* Create the CNRR task */
    swi_ossdkthreadcreate( cnrrtask, NULL );

}

/*
 * $Log: cnrrtask_sdk.c,v $
 */

