/*************
 *
 * $Id: drshell.c,v 1.11 2010/06/09 23:38:22 epasheva Exp $
 *
 * Filename: drshell.c
 *
 * Purpose:  DR package shell functions
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "dr/dridefs.h"

/* Define */


/*************
 *
 * Name:  drprocdemuxreq
 *
 * Purpose: This function sends the event DRAPPREQUEST to DR task
 *             
 * Parms:   deviceservice - device service indicator 
 *          operation     - Demux request operation
 *
 * Return:  none
 *
 * Abort:   When DR is out of event blocks
 *
 * Notes:   none
 * 
 * 
 **************/
local void  drprocdemuxreq(swi_uint16 deviceservice, swi_uint16 operation)
{  

    /* Allocate event block to send event to DR task */
    struct drevtblock  *eventbkp;    /* event block for DR task */

    /* Get pointer to the task control block */
    struct drtaskcb *drcbp = drgetcbp();

    /* Get an event block pointer */
    eventbkp = (struct drevtblock *) mmbufget( &drcbp->drevtpool);
           
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        /* Fill the event with Loopback response message */
        eventbkp->drevent         = DRAPPREQUEST;
        eventbkp->drmemfreep      = NULL;  
        /* Fill the rest of the event block */
        eventbkp->drevtmember.drapprequest.drdevservice = deviceservice;
        eventbkp->drevtmember.drapprequest.droperation = operation;
        
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
 *
 * Name:  drprocdemuxdata
 *
 * Purpose: Process Demux Data Indication 
 *             
 * Parms:   msgp          - pointer to data message payload
 *          msglen        - length of payload
 *          deviceservice - device service indicator 
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   Uses existing HP mechanism to send data to the device if DM EP is 
 *          not available. Using the existing HP mechanism, this function 
 *          builds a HIP message based on deviceservice and sends it using the 
 *          existing HIP interface.
 * 
 **************/
local void drprocdemuxdata( swi_uint8 *msgp, swi_uint32 msglen, 
    swi_uint16 deviceservice )
{
    swi_uint8 *sendbufp;       /* Pointer to storage for outgoing buffer */
    swi_uint8 *payloadp;       /* Pointer to payload area of sendbufp */
    struct usbep dmep = {0, 0}; /* US endpoint */
    swi_uint16 channel;
    
    switch (deviceservice)
    {
        case  DRDIAG:
            if (usgetep(USEP_DIAG, &dmep))
            {
                /* make sure not sending empty buffer */
                if (msgp && (msglen > 0))
                {
                    channel = cigetchannelnum("UDIAG");
                    /* Pass diagnostic message to device */
                    dssend(msgp, msglen, channel);
                }
            }
            else
            {
                /* Build HPMSTACKIND packet and copy the payload in it */
                /* Ask the HIP layer for a buffer to build the message into */
                sendbufp = (swi_uint8 *) hpgetsizedbuf(msglen);
                /* Note: hpgetbuf() returns always a buffer; it aborts on 
                 *       no memory otherwise
                 */
                /* Skip past the HIP header section of HPMSTACKIND message */
                payloadp = hpgetpayp( sendbufp, HPMSTACKIND );
                /* Verify parameters */
                if (payloadp && (msglen > 0))
                {
                    slmemcpy( payloadp, msgp, msglen);
                    /* Note: The packet is ready for transmission -don't forget 
                     * to use the sendbufp pointer because it's the HP package's 
                     * job to release the send buffer when it's finished 
                     * transmitting to the modem
                     */
                    /* Pass HP message to device */
                    hpTxPacket( HPMSTACKIND, HPNULLPARMFIELD, sendbufp, msglen);
                }
                else
                {
                    /* release sendbufp */ 
                    mmbufrel( sendbufp ); 
                }
            }
            break;
            
        case DRNMEA:
            /* Do nothing - Not implemented */
            break;
        
        default:
            /* Abort*/
            erAbort("drprocdemuxdata: unknown device service", 
                deviceservice);
            break;        
    }
}

/*************
 *
 * Name: dripcrcvhandler
 *
 * Purpose: Generic IPC Receive Handler which is invoked for incomming  data 
 *          from Device Service IPC channel. It sends messages to the endpoint
 *          using the existing HP send mechanism.
 *             
 * Parms:   inipcmsgp    - Pointer to received AM packet payload.
 *          inipcmsglen  - length of received message in bytes
 *          memfreep     - Pointer to be used when freeing memory 
 *                         for this request
 *          ipcchannel   - IPC channel over which this msg came in - unused here
 * 
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   The device service must be initialized and the corresponding IPC 
 *          channel open before the handler is invoked.
 *          The supported protocol is described in IDD_DemuxMessaging.doc
 *          This function must release memfreep after sending data to
 *          the endpoint.
 *          We want DR to be in charge of the buffer so that it can be released
 *          here or somewhere else under the control of DR. This could be used 
 *          for future improvements, e.g. avoiding copying the message.
 * 
 * 
 **************/
package void dripcrcvhandler( 
    swi_uint8 *inipcmsgp, 
    swi_uint32 inipcmsglen,
    swi_uint8 *memfreep,
    swi_uint16 ipcchannel)
{
    swi_uint32 msglen;         /* length of the message to be send to device */
    swi_uint32 eranumber;      /* to uniquely id the request */
    swi_uint32 payldoffset;  /* offset of payload within the packet*/ 
    swi_uint16 operation;      /* Operation code for AMDEMUXREQUEST */
    swi_uint16 deviceservice;  /* Local var for device service storage */
    swi_uint16 amtype;
    swi_uint16 amheaderversion;
    swi_uint16 ampacketlength;
    swi_uint8  amresultcode;
    swi_uint8 opcode;          /* op code from Control Ind */
    swi_uint8 devicepresent;   /* flag for device presence from Control Ind */
    swi_uint8  errorstring[AMMAXERRORSTRINGSZ]; /* error string storage */
    swi_uint8 *msgp;           /* Local copy of input IPC message pointer to be
                                * used in parsing 
                                */

    msgp = inipcmsgp;  
    
    /* Parse the received message */
    /* First parse the header */
    amparseamheader( &amtype, &amheaderversion, &amresultcode, &ampacketlength,
        &msgp);
    /* Note: header parameter check not implemented */
    switch (amtype)
    {
        case AMDEMUXREQUEST:
            /* Parse Demux Request */
            amparsedemuxrequest( &msgp, &deviceservice, &operation ); 
            /* Process request */
            drprocdemuxreq( deviceservice, operation );
            break;
                       
        case AMDEMUXCTLIND:
            /* Parse Demux Control Indication */
            amparsedemuxctlind( &msgp, &deviceservice, &opcode, &devicepresent,
                &eranumber, &errorstring[0] );
            /* Process Demux Control Indication - Not implemented */
            break;
            
        case AMDEMUXDATAIND:
            /* Parse Demux Data Indication */
            amparsedemuxdataind( &msgp, &deviceservice );
            if (msgp)
            {
                /* calculate payload length */
                payldoffset =  msgp - inipcmsgp; 
                msglen = inipcmsglen - payldoffset;
            }
            /* Process data */
            drprocdemuxdata( msgp, msglen, deviceservice );
            break;
            
        default:
            /* Do nothing */          
            break;
    }
    
    /* Free message buffer here since we either copied the message or will
     * ignore the message if unknown
     */

    /* Release the buffer */
    if (memfreep != NULL)
    {
        mmbufrel( memfreep );
    }
 }

/*
 * $Log: drshell.c,v $
 */
  
