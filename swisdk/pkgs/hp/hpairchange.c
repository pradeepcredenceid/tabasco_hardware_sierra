/*************
 *
 * $Id: hpairchange.c,v 1.12 2010/06/09 23:43:43 epasheva Exp $
 *
 * Filename: hpairchange.c
 *
 * Purpose:  This file contains functions in support of the Air
 *           Server Change notification message. The entry point
 *           is outside the HPPH task context while the handler 
 *           runs inside the task context
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "hp/hpidefs.h"
#include "am/amudefs.h"

/*************
 *
 * Name:    hpphgettechtype
 *
 * Purpose: Return Technology type obtained from modem when it was plugged in.
 *             
 * Parms:   none
 *
 * Return:  The technology type of the modem - GSM or CDMA
 *
 * Abort:   None
 *
 * Notes:   This function runs outside of the HPPH task's context
 *
 **************/
global swi_uint8 hpphgettechtype( void )
{
    /* Local pointer to AirServer  status structure */
    struct hpairserverstatus *hpairstatcbp;

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();   
    
    /* Initialize the AirServer Status server structure pointer */
    hpairstatcbp = &hpphcbp->hpphairstate;
    
    return (hpairstatcbp->hpairtechtype);
}

/*************
 *
 * Name:    hpphgetprodstr
 *
 * Purpose: Return Product string obtained from modem when it was plugged in.
 *             
 * Parms:   none
 *
 * Return:  Pointer to the product string
 *
 * Abort:   None
 *
 * Notes:   This function runs outside of the HPPH task's context
 *
 **************/
global swi_uint8 *hpphgetprodstr( void )
{
    /* Local pointer to airserver  status structure */
    struct hpairserverstatus *hpairstatcbp;

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();   
    
    /* Initialize the AirServer Status server structure pointer */
    hpairstatcbp = &hpphcbp->hpphairstate;
    
    return (&hpairstatcbp->hpairprodstr[0]);
}

/*************
 *
 * Name:    hpsetdirtimer
 *
 * Purpose: Start and stop the HIP Device Information Request timeout timer 
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
package void hpsetdirtimer( swi_uint32 timeout )
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
    hpphcbp->hpphdevinfo.hpphdevtimer = ticks;
}

/*************
 *
 * Name:    hpdirclearreq
 *
 * Purpose: Clear the settings for the current device information request
 *          stored in the device information control block
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
local void hpdirclearreq(void)
{
    swi_uint16 i;
    
    /* Pointer to device information control structure */
    struct hpdevinfo *hpdevinfocbp;

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* initialize the device information control structure pointer */
    hpdevinfocbp = &hpphcbp->hpphdevinfo;

    /* Clear out the entries for the current request */
    hpdevinfocbp->hpdevbusy     = FALSE;
    hpdevinfocbp->hpdevcbfp     = NULL;
    hpdevinfocbp->hppdevhuserp  = NULL;
    hpdevinfocbp->hpdevtechtype = 0;
    slmemset ((char *)&hpdevinfocbp->hpdevprodstr[0], 0, HPDIRPRODSTRSZ); 

    /* Invalidate Channels */
    for (i = 0; i < CIMAXCNRRPAIR; i++)
    {
        hpdevinfocbp->hpdevamchan[i] = CIINVIPCCHAN;
    }
    
    /* Stop the device information request timeout timer */
    hpsetdirtimer( 0 );
}

/*************
 *
 * Name:    hpsenddirresp
 *
 * Purpose: Send modem responses to Device Information Requests,
 *          ie, applications
 *             
 * Parms:   resultcode   - AM Result code to send back to application 
 *          amdevinforrp - pointer to AM's device info packet
 *          force_send   - flag to send regardless of whether the application
 *                         expected a response.
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   The force_flag is used to prevent a lock-up in AM task during error
 *          conditions. For example, the stop-and-wait protocol will not
 *          proceed until a 'sent response' event has occurred. 
 *
 **************/
local void hpsenddirresp (
    enum swi_amresults resultcode,
    struct amdevinfoparms  *amdevinforrp,
    swi_bool force_send)
{
    swi_uint16      i, channel;
    swi_uint16      outputsize;
    swi_uint8       outputbuf[AMDEVINFORESPTOTAL];
    swi_uint8       *outputbufp = &outputbuf[0];

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();   

    /* If IPC channel(s) requested this, return the data */
    for (i = 0; i < CIMAXCNRRPAIR; i++)
    {
        if (!force_send)
        {
            /* grab a requesting channels */
            channel = hpphcbp->hpphdevinfo.hpdevamchan[i];
        }
        else
        {
            /* grep an available RR channels */
            channel = cigetnextrrchannum (i);
        }
        /* Only for valid channels */
        if (channel != CIINVIPCCHAN)
        {
            /* build the AM message and send it back to the app */
            slmemset((char *)&outputbuf[0], 0, AMDEVINFORESPTOTAL);
            outputsize = ambuilddevinforesponse( &outputbufp, 
                                                 resultcode,
                                                 amdevinforrp);
            amsdk_send (outputbufp, outputsize, channel);
            dlLog(  &hpphcbp->hpphdluser, HPLOG_CLASSA, 
                    "Sent device info resp ch:%d",
                    (swi_uint32)channel, 
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL);
        }
    }
}

/*************
 *
 * Name:    hpdirtimer
 *
 * Purpose: Implements the logic required to service the device information
 *          request timer and to handle when the timer expires. 
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
package void hpdirtimer( void)
{
    /* Get pointer to the task control block */
    struct hpphcb           *hpphcbp = hpgetphcbp();
    struct amdevinfoparms   amdevinforr;

    /* Decrement the timer if it's active */
    if( hpphcbp->hpphdevinfo.hpphdevtimer )
    {
        if( --(hpphcbp->hpphdevinfo.hpphdevtimer) == 0 )
        {
            /* Handle a timeout condition */
            if( hpphcbp->hpphdevinfo.hpdevbusy )
            {
                /* If a callback function has been defined */
                if( hpphcbp->hpphdevinfo.hpdevcbfp )
                {
                    /* call it */
                    hpphcbp->hpphdevinfo.hpdevcbfp ( HPTIMEOUT, 
                                        hpphcbp->hpphdevinfo.hppdevhuserp );

                    dlLog(  &hpphcbp->hpphdluser, HPLOG_CLASSA, 
                            "DIR timed out awaiting response",
                            (swi_uint32)NULL,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL);
                }
                /* Unusual situation, log it */
                else
                {
                    dlLog(  &hpphcbp->hpphdluser, HPLOG_CLASSA, 
                            "DIR timeout with no callback",
                            (swi_uint32)NULL,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL,
                            (swi_uint32)NULL);
                }
                
            /* Timer expired but device information request not busy, do nothing */
            }
            else
            {
                dlLog(  &hpphcbp->hpphdluser, HPLOG_CLASSA, 
                        "DIR timeout but no longer busy",
                        (swi_uint32)NULL,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL);
            }
            /* In case there is a Stop-and-wait */
            slmemset ((char *)&amdevinforr, 0, sizeof (struct amdevinfoparms));
            amdevinforr.amdevreqstatus = HPDIRGENFAIL;
            hpsenddirresp ( SWI_AMTIMEOUT, &amdevinforr, TRUE );
            /* Clear the device information request parameters */
            hpdirclearreq();
        /* Timer active, but not yet zero, then do nothing */
        }
    /* No timer active, then do nothing */
    }
}

/*************
 *
 * Name:    hpairchangereq
 *
 * Purpose: Entry point to the HPPH task requesting that an AirServer Change
 *          notification be sent to the API side
 *             
 * Parms:   devicestate - TRUE the device has been detected by the US Scan
 *                        task. FALSE, the device has disappeared
 *          apprunning  - If devicestate is TRUE, then this variable 
 *                        indicates whether the modem is running in the 
 *                        boot loader (FALSE) or the application (TRUE)
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This function runs outside of the HPPH task's context
 *
 **************/
global void hpairchangereq( swi_bool devicestate, swi_bool apprunning  )
{
    struct hpphevtblock  *eventbkp;    /* Stores HPPH Task Request parms */
    struct hpphairchgevt *airchgevtp;  /* Air Change union member of event bk */

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Get an event block pointer */
    eventbkp = (struct hpphevtblock *) mmbufget( &hpphcbp->hpphevtpool);
    
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        airchgevtp = &eventbkp->hpevtmember.hpphairchg;

        /* Fill in the contents of the event block */
        eventbkp->hpphevent         = HPPHAIRCHG; 
        eventbkp->hpphmemfreep      = NULL; /* Nothing to release */
        airchgevtp->hpphairstatus   = devicestate;
        airchgevtp->hpphapprunning  = apprunning;
        
        /* Send the new event to the HP PH task  */
        icsmt_send( &hpphcbp->hpphicmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("hpairchg: Out of memory", 0);
    }
}

/*************
 *
 * Name:    hpprocairchg
 *
 * Purpose: This function processes an air server change notification
 *          request
 *             
 * Parms:   eventbkp - Pointer to the event block which contains the
 *                     request and associated parameters
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This function runs inside the HPPH task's context. It is
 *          the counterpart to hpairchangereq()
 *
 **************/
package void hpprocairchg( 
    struct hpphairchgevt *airchgevtp)
{
    swi_uint8       outbuf[AMNOTAIRCTOTAL];    /* Storage for the notification */
    swi_uint8       *outbufp;                  /* Required for call to AM */
    struct hpphcb   *hpphcbp = hpgetphcbp();

    /* Store the latest information locally */ 
    hpphcbp->hpphairstate.hpairserverpresent = airchgevtp->hpphairstatus;
    
    /* Increment era number to identify the change  */
    ++(hpphcbp->hpphairstate.hpairchgera);   

    /* Kick off the modem change notifications
     * to registered packages, if any
     */
    hpmdmkick( airchgevtp->hpphairstatus, 
               airchgevtp->hpphapprunning, 
               ddgetdevtype() );

    /* Create a pointer to the output buffer */
    outbufp = outbuf;

    /* Format the response */
    ambuildairchangenotify( &outbufp, ddgetdevtype(), 
                            airchgevtp->hpphairstatus, 
                            airchgevtp->hpphapprunning, 
                            hpphcbp->hpphairstate.hpairchgera,
                            &hpphcbp->hpphairstate.hpairprodstr[0],
                            hpphcbp->hpphairstate.hpairtechtype);

    /* 
     * Send the notification to the API Side.
     */
    amsdk_sendnot( outbuf, sizeof(outbuf), CICNNOTIFYALL );
}

/*************
 *
 * Name:    hpgetdevinfo
 *
 * Purpose: This function queues a get Device Info Request to the HP PH task.
 *             
 * Parms:   hpaircbkfp - pointer to caller's call back function
 *              resultcode - result code to be returned to call back 
 *              userp      - pointer to user originated data block.
 *          userp      - pointer to user parameter
 *          version    - version of message 
 *          reqtype    - request type 
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This function runs in the caller's task context.
 *
 **************/
global void hpgetdevinfo( 
    void (*hpdevcbfp)(  
         enum hpphstatus resultcode, 
         void *userp),
    void *userp,
    swi_uint8 version,
    swi_uint8 reqtype, 
    swi_uint16 ipcchannel)
{
    struct hpphevtblock   *eventbkp;   /* Stores HPPH Task Request parms */
    struct hpphdevinfoevt *devinfoevtp; /* Device info union member of event bk*/

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Get an event block pointer */
    eventbkp = (struct hpphevtblock *) mmbufget( &hpphcbp->hpphevtpool);
    
    /* Don't want NULL event block pointers */
    if( eventbkp )
    {
        devinfoevtp = &eventbkp->hpevtmember.hpdevinfo;

        /* Fill in the contents of the event block */
        eventbkp->hpphevent            = HPPHDIRREQ; 
        eventbkp->hpphmemfreep         = NULL; /* Nothing to release */
        devinfoevtp->hpdevcbfp         = (void *)hpdevcbfp;
        devinfoevtp->hpdevuserp        = userp;
        devinfoevtp->hpphdevinforeqver = version;
        devinfoevtp->hpphdevinforeqtyp = reqtype;
        devinfoevtp->hpphdevinfoipcch  = ipcchannel;
        
        /* Send the new event to the HP task  */
        icsmt_send( &hpphcbp->hpphicmsgque, (swi_uint32) eventbkp );
    }
    /* NULL Block */
    else
    {
        /* This should never happen */
        erAbort("hpgetdevinfo: Out of memory", 0);
    }
}

/*************
 *
 * Name:    hpprocdirreq
 *
 * Purpose: Package a Device Information Request HIP message and send it to the
 *          modem.
 *          
 * Parms:   devinfoevtp - Pointer to the Device Info union member of
 *                        the event block containing the caller's request type
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This function runs inside the HPPH task context
 *
 **************/
package void hpprocdirreq(struct hpphdevinfoevt *devinfoevtp)
{  
    swi_uint8               *bufp;
    swi_uint8               *payloadp; 
    swi_uint8               *payloadstartp; 
    swi_uint16              paysize;
    swi_uint16              i, local_chan;
    struct amdevinfoparms   amdevinforr;
    
    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();

    /* Only accept this request if there isn't one already being handled */
    if( !hpphcbp->hpphdevinfo.hpdevbusy 
        && !hpphcbp->hpphloopcb.hploopbusy )
    {
        /* Now flag that we are busy and start the timeout timer */
        hpphcbp->hpphdevinfo.hpdevbusy = TRUE;

        /* Standard timeout */
        hpsetdirtimer( HPDIRTIMEOUT );

        /* Register callback to call when response received */
        hpphcbp->hpphdevinfo.hpdevcbfp  = devinfoevtp->hpdevcbfp;
        hpphcbp->hpphdevinfo.hppdevhuserp = devinfoevtp->hpdevuserp;
        
        /* Save the channel number into the gcb for response use. */
        for (i = 0; i < CIMAXCNRRPAIR; i++)
        {
            if (devinfoevtp->hpphdevinfoipcch != CIINVIPCCHAN)
            {
                local_chan = devinfoevtp->hpphdevinfoipcch;
                if ( (hpphcbp->hpphdevinfo.hpdevamchan[i] != local_chan) &&
                     (hpphcbp->hpphdevinfo.hpdevamchan[i] == CIINVIPCCHAN) )
                {
                    hpphcbp->hpphdevinfo.hpdevamchan[i] = local_chan;
                    break;
                }
            }
        }
        
        /* This request is only valid for devices using unified PID */
        if( ddgetdevtype() != SWI_DEVICE_UNIFIEDPID )
        {
            /* In case there is a Stop-and-wait */
            slmemset ((char *)&amdevinforr, 0, sizeof (struct amdevinfoparms));
            amdevinforr.amdevreqstatus = HPDIRGENFAIL;
            hpsenddirresp ( SWI_AMDEVNOTSUP, &amdevinforr, TRUE);
            hpdirclearreq();
            return;
        }

        /* Get a HIP transmission buffer */
        bufp = hpgetbuf();
            
        /* 
         * Get a pointer to the payload portion
         * Fill in the payload portion with field message version and
         * request type
         */
        payloadp = hpgetpayp( bufp, HPMDEVREQ );
        payloadstartp = payloadp;
        piput16 ( devinfoevtp->hpphdevinforeqver, &payloadp); 
        *payloadp++ = devinfoevtp->hpphdevinforeqtyp;
        
        paysize = payloadp - payloadstartp;
        /* Send the request on its way */
        hpTxPacket( HPMDEVREQ, HPNULLPARMFIELD, bufp, paysize );

        dlLog ( &hpphcbp->hpphdluser, DLCLASSALL, 
                "Sent device info req %d to modem, paysz:%d",
                (swi_uint32)devinfoevtp->hpphdevinforeqtyp,
                (swi_uint32)paysize,
                (swi_uint32)NULL,
                (swi_uint32)NULL);
    }
    else
    {
        /* Already processing another request, tell the caller NO */
        if (devinfoevtp->hpdevcbfp)
            devinfoevtp->hpdevcbfp( HPDIRBUSY, devinfoevtp->hpdevuserp );
            
        slmemset ((char *)&amdevinforr, 0, sizeof (struct amdevinfoparms));
        amdevinforr.amdevreqstatus = HPDIRGENFAIL;
        hpsenddirresp ( SWI_AMTIMEOUT, &amdevinforr, TRUE);
        hpdirclearreq();
    }
} 

/*************
 *
 * Name:    hpprocdirresp
 *
 * Purpose: Process modem responses to Device Information Requests
 *             
 * Parms:   hipevtp  - Pointer to the HIP union member of the
 *                     incoming event block 
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This is the callback for the extended HIP message 
 *          Device Information Response, executed in the HPPH task's context. 
 *
 **************/
package void hpprocdirresp(struct hphipevt *hipevtp)
{
    enum hpphstatus         status = HPFAILED;
    swi_uint32              msgid;
    swi_uint32              extmsgid;
    swi_bool                extheader;
    swi_uint16              version;
    swi_uint8               reqstat;
    swi_uint8               *payloadp;
    struct hpphairtlv       *tlvp;
    struct amdevinfoparms   amdevinforr;
    enum swi_amresults      resultcode = SWI_AMFAILED;
    
    /* Local pointer to Air Server status and device info structures */
    struct hpairserverstatus *hpairstatcbp;
    struct hpdevinfo         *hpdevinfocbp;

    /* Get pointer to the task control block */
    struct hpphcb *hpphcbp = hpgetphcbp();   
    
    /* Initialize the Air Server Status and device info structure pointers */
    hpairstatcbp = &hpphcbp->hpphairstate;
    hpdevinfocbp = &hpphcbp->hpphdevinfo;
   
    /* If there was a HIP error , mark error */
    if (hipevtp->hppherrorflg) 
    {
        dlLog(  &hpphcbp->hpphdluser, HPLOG_CLASSA, 
              "Device Info Request error %d",
              (swi_uint32)hipevtp->hppherrorflg, 
              (swi_uint32)NULL,
              (swi_uint32)NULL,
              (swi_uint32)NULL);

        status     = HPFAILED;
        resultcode = SWI_AMFAILED;
        /* Send back to app, if any, via IPC channels */
        amdevinforr.amdevreqstatus = HPDIRGENFAIL;
        hpsenddirresp (resultcode, &amdevinforr, TRUE);
    }
    else /* HIP message OK, process message */
    {
        hpdesignator2msgid( hipevtp->hpphmsgdesignator, 
                            &msgid, &extheader, &extmsgid);

        /* The service is for an extended HIP message */
        if ( msgid == HPMAXFMSGID )
        {
            /* Get the payload and convert to TLV structure */
            payloadp = hipevtp->hpphdatap;
            version  = piget16 (&payloadp);
            reqstat  = *payloadp++;
            tlvp     = (struct hpphairtlv  *)payloadp;
            
            slmemset ((char *)&amdevinforr, 0, sizeof (struct amdevinfoparms));
            /* Set data that are always returned */
            amdevinforr.amdevversion   = version;
            amdevinforr.amdevreqstatus = reqstat;
            amdevinforr.amdevinfolen   = tlvp->hpphtlvlen;

            /* If the request was successful, we can extract data */
            if (reqstat == 0)
            {
                resultcode = SWI_AMSUCCESS;
                dlLog(  &hpphcbp->hpphdluser, HPLOG_CLASSA, 
                        "Device Info resp recv'd",
                        (swi_uint32)NULL, 
                        (swi_uint32)NULL,
                        (swi_uint32)NULL,
                        (swi_uint32)NULL);
                
                switch (tlvp->hpphtlvtype)
                {
                    case HPMDEVREQTECHTYP:
                        /* get the Technology type */
                        hpairstatcbp->hpairtechtype = 
                                                tlvp->hpphpayld.hpphtlvvalue;
                        amdevinforr.amdevtechology  = 
                                                tlvp->hpphpayld.hpphtlvvalue;
                        break;
                    case HPMDEVREQPRDSTR:
                        /* Clear any old data from our storage */
                        slmemset ( (char *)&hpairstatcbp->hpairprodstr[0],
                                    0, HPDIRPRODSTRSZ );
                        /* Copy the product string  */
                        slmemcpy (  &hpairstatcbp->hpairprodstr[0],
                                    &tlvp->hpphpayld.hpphtlvvalstr[0], 
                                    tlvp->hpphtlvlen );
                        slmemcpy (  & amdevinforr.amdevprodstr[0],
                                    &tlvp->hpphpayld.hpphtlvvalstr[0], 
                                    tlvp->hpphtlvlen );
                        dlLog(  &hpphcbp->hpphdluser, HPLOG_CLASSA, 
                                "Product string: %s",
                                (swi_uint32)&hpairstatcbp->hpairprodstr[0], 
                                (swi_uint32)NULL,
                                (swi_uint32)NULL,
                                (swi_uint32)NULL );
                        break;
                    case HPMDEVREQFWDNLD:
                        amdevinforr.amdevfwdnldproto = 
                                                tlvp->hpphpayld.hpphtlvvalue;
                        break;
                    case HPMDEVREQDEVENUM:
                        amdevinforr.amdevreenum = tlvp->hpphpayld.hpphtlvvalue;
                        break;
                    case HPMDEVREQCNS:
                        amdevinforr.amdevcnssupported = 
                                                tlvp->hpphpayld.hpphtlvvalue;
                        break;
                    default:
                        dlLog(  &hpphcbp->hpphdluser, HPLOG_CLASSA, 
                                "Device Info Resp %x not supported yet",
                                (swi_uint32)tlvp->hpphtlvtype, 
                                (swi_uint32)NULL,
                                (swi_uint32)NULL,
                                (swi_uint32)NULL );
                        resultcode = SWI_AMDEVNOTSUP;
                    break; 
                }
                
                status = HPOK;
             } /* successful request status, otherwise only status is returned */
             else
                resultcode = SWI_AMDEVNOTSUP;
                
        } /* Extended message ID */
        /* Send back to app, if any, via IPC channels */
        hpsenddirresp (resultcode, &amdevinforr, FALSE);
    }
    
    /* Call the caller-supplied callback routine if there is one */
    if( hpdevinfocbp->hpdevcbfp )
    {
        hpdevinfocbp->hpdevcbfp(status, hpdevinfocbp->hppdevhuserp);
    }
    
    /* Reset request fields */
    hpdirclearreq ();
}

/*************
 *
 * Name:    hpdirinit
 *
 * Purpose: Initialize the Device Information request subsystem in the HPPH task
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
package void hpdirinit( void )
{
    /* This one call does it all for us */
    hpdirclearreq();
}

/*
 * $Log: hpairchange.c,v $
 */

