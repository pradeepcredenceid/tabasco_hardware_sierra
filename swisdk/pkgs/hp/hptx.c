/*************
 *
 * $Id: hptx.c,v 1.19 2010/06/09 23:46:50 epasheva Exp $
 *
 * Filename: hptx.c
 *
 * Purpose:  This file contains procedures required to build and send HIP
 *           packets to a modem
 *
 * Copyright:  2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "hp/hpidefs.h"


/*************
 *
 * Name: hpgethdrsize 
 *
 * Purpose: Return the size of a HIP header given the short message 
 *          ID value
 *             
 * Parms:   rvsmsgdesignator - A outgoing HIP message designator
 *
 * Return:  Size, in bytes, of the HIP header for the corresponding 
 *          message ID
 *
 * Abort:   msgid is not within the reverse message ID range
 *
 * Notes:   None
 *
 **************/
package swi_int32 hpgethdrsize( swi_uint32 rvsmsgdesignator)
{
    swi_uint32 msgid;
    swi_uint32 extmsgid;
    swi_bool extheader;

    hpdesignator2msgid(rvsmsgdesignator, &msgid, &extheader, &extmsgid);
    
    /* If the message is outside the min/max range, abort */
    if( msgid > HPMAXRMSGID )
    {
        /* Caller has specific a message ID that is not in the 
         * valid range for HIP Reverse (i.e. host-to-modem) 
         */
        erAbort("hpgethdrsize: bad message id", msgid);
    }
    
 
    if( extheader)
    {
        if(extmsgid > HPMAXREXTMSGID)
        {
            erAbort("hpgethdrsize: bad extended message id", extmsgid);
        }
    
        return HPHDRLENGTHEXT;
    }
    else
    {
        return HPHDRLENGTH;
    }
}

/*************
 *
 * Name: hpgetpayp
 *
 * Purpose: To pass in a pointer to a buffer and have a pointer to the 
 *          HIP payload section returned. The returned pointer is 
 *          the original one, advanced by an amount equal to the HIP
 *          header. Note that the header size varies depending upon the
 *          HIP Message ID. 
 *             
 * Parms:   bufp              - pointer to a buffer which the caller desires to
 *                            skip past the HIP header field
 *          hpmsgdesignator   - Short message ID. This parameter is sufficient 
 *                            to indicate what size HIP header the buffer will 
 *                            need to accommodate. 
 *
 * Return:  Pointer to the HIP payload portion of the buffer
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
global swi_uint8 *hpgetpayp(swi_uint8 *buffp, swi_uint32 hipmsgdesignator)
{
    return (swi_uint8 *) ((swi_uint32) buffp+hpgethdrsize( hipmsgdesignator ));
}

/*************
 *
 * Name: hpgetbuf
 *
 * Purpose: To allocate a buffer payload and then pass it on to the HIP 
 *          package for addition of the HIP header. The caller is passed 
 *          pointer to the allocated buffer.
 *             
 * Parms:   none
 *
 * Return:  Pointer to allocated buffer
 *
 * Abort:   none
 *
 * Notes:   This is an obsolete function. It is recommended one uses 
 *          hpgetsizedbuf(swi_uint32 msglen) instead which allocates buffers
 *          based on message size for more efficient use of memory.
 *
 **************/
global swi_uint8 *hpgetbuf( void )
{
    struct hptxcb *hptxcbp = hpgettxcbp();
    swi_uint8 *bufp; 

    bufp = (swi_uint8 *) mmbufget( &hptxcbp->hpTxPool );

    /* Should never run out of memory */
    if( bufp == NULL )
    {
        erAbort("hpgetbuf: no memory", 0);
    }

    return bufp;
}

/*************
 *
 * Name: hpgetsizedbuf
 *
 * Purpose: To allocate a buffer payload based on the size of the message
 *          The caller is passed a pointer to the buffer
 *             
 * Parms:   msglen - length of the message in number of bytes
 *
 * Return:  Pointer to allocated buffer
 *
 * Abort:   none
 *
 * Notes:   There are 2 pools for data transmission - one with HDTXBUFSIZE and
 *          the other with HDTXSMLBUFSIZE
 *          The msglen parameter is used to determine what size of pool buffer
 *          to allocate. 
 *          If msglen is set to 0 by the caller buffer of size HDTXBUFSIZE is 
 *          allocated. 
 *          If msglen is non-zero then msglen is examined and the buffer of
 *          either HDTXSMLBUFSIZE or HDTXBUFSIZE is allocated.
 *          
 *
 **************/
global swi_uint8 *hpgetsizedbuf( swi_uint32 msglen )
{
    struct hptxcb *hptxcbp = hpgettxcbp();
    swi_uint8 *bufp; 
    
    /* Decide on which pool to use based on message length */
    if ((msglen != 0) &&
        (msglen < HDTXSMLBUFSIZE))
    {
        /* Get buffer from small pool */
        bufp = (swi_uint8 *) mmbufget( &hptxcbp->hpTxSmlPool );
    }
    else
    {
        /* Get buffer from the large pool */
        bufp = (swi_uint8 *) mmbufget( &hptxcbp->hpTxPool );
    }

    /* Should never run out of memory */
    if( bufp == NULL )
    {
        erAbort("hpgetsizedbuf: no memory", 0);
    }

    return bufp;
}



/*************
 *
 * Name: hpconstruct
 *
 * Purpose: To construct a HIP header and copy the HIP payload into
 *          a HIP packet buffer.
 *             
 * Parms:   packetp        - pointer to the HIP packet buffer
 *          msgdesignator  - message designator
 *          msgparm        - message specific parameter
 *          paylength      - length of the payload
 *          payloadp       - pointer to the payload of the message
 *
 * Return:  size of the HIP header just created. Either HPHDRLENGTH or
 *          HPHDRLENGTHEXT
 *
 * Abort:   none
 *
 * Notes:   Currently this function does not support the extended HIP
 *          header construction. 
 *
 **************/
package swi_int32 hpconstruct(
    swi_uint8 *packetp,
    swi_uint32 msgdesignator,
    swi_uint8 msgparm,
    swi_uint16 paylength,
    swi_uint8 *payloadp)
{
    swi_uint32 msgid;
    swi_uint32 extmsgid;
    swi_bool   extheader;
    swi_uint16 tomsk = 0;
    swi_uint16 frommsk = 0;

    /* 
     * Fetch actual message id and extended message id from the 
     * message designator 
     */
    hpdesignator2msgid(msgdesignator, &msgid, &extheader, &extmsgid);
    
    /* construct original hip header */
    piput16(tomsk|frommsk|paylength, &packetp);
    *packetp++ = msgid;
    *packetp++ = msgparm;
   
    /* if an extended header is required for this message */
    if(extheader)
    {
        piput16(extmsgid & HPEXTMSGIDMSK, &packetp);
    }
    
    /* return HIP header length based on the message */
    return(hpgethdrsize(msgdesignator)); 
}

/*************
 *
 * Name: hpTxPacket
 *
 * Purpose: Package entry point to send HIP messages to the modem
 *
 * Parms:   msgdesignator - message designator
 *          msgparm       - message specific parameter
 *          hippacketp    - pointer to a HIP packet header & filled-in payload
 *          paylength     - length of the message payload field
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   This is a non-blocking function
 *  
 *          The HP Transmit task is responsible for releasing the memory 
 *          containing the message once it has been transmitted.
 *          The caller must ensure the memory the message is stored in is
 *          allocated from the MM pool or a fatal error condition will result.
 *      
 *          On return from this function, it is not safe to release the 
 *          memory containing the HIP payload, if applicable.
 *          Buffers and event blocks will be released by HP TX task. 
 *
 **************/
global void hpTxPacket(
    swi_uint32 msgdesignator,
    swi_uint8 msgparm,
    swi_uint8 *hippacketp,
    swi_uint16 paylength)
{

    struct hptxevtblock  *eventbkp;    /* Incoming event block for HP TX task */

    /* Get pointer to the task control block */
    struct hptxcb *hptxcbp = hpgettxcbp();

    /* Get an event block pointer */
    eventbkp = (struct hptxevtblock *) mmbufget( &hptxcbp->hptxevtpool );
    
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        /* Fill the event with HIP message */
        eventbkp->hptxeventtype = HPTXHIP;
        eventbkp->hptxmemfreep  = hippacketp;
        eventbkp->msgdesignator = msgdesignator;
        eventbkp->msgparm       = msgparm;
        eventbkp->hippacketp    = hippacketp;
        eventbkp->paylength     = paylength;
    
        /* Send the new event to the HP TX task  */
        icsmt_send( &hptxcbp->hptxicmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("HPTXPKT: Out of memory", 0);
    }
}

/*************
 *
 * Name: hptxinit 
 *
 * Purpose: Initializes the Transmit path of the HP package. 
 *
 * Parms:   None
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Must be called once at initialization time before the HP
 *          Tx path can be used. 
 *
 **************/
package void hptxinit( void )
{
    struct hptxcb *hptxcbp = hpgettxcbp();
    
    /* create a memory pool for transmit buffer allocation */
    mmpcreate( &hptxcbp->hpTxPool, "hpTxPool", HPTXPOOLSZ, HDTXBUFSIZE );
    
    /* create a memory pool for transmit small buffer allocation */
    mmpcreate( &hptxcbp->hpTxSmlPool, "hpTxSmlPool", HPTXSMALLPLSZ, 
        HDTXSMLBUFSIZE );

    /* Create the Event Pool for events received by HP tx  task */
    mmpcreate( &hptxcbp->hptxevtpool,
               "HPTXEVTPOOL", 
               HPTXEVPOOLSZ, 
               sizeof(struct hptxevtblock) );
    
    /* Initialize the SMT Message queue structure  */
    icsmt_create( &hptxcbp->hptxicmsgque, "HPTXMESSAGEQ", HPTXMSGDEPTH );

    /* Register with the logger process, initially disabled */
    dlregister ("HPTX", &hptxcbp->hptxdluser, FALSE);
}

/*
 * $Log: hptx.c,v $
 */

