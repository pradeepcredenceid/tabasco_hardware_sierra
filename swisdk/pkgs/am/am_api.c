/*************
 *
 * $Id: am_api.c,v 1.11 2010/06/09 23:23:57 epasheva Exp $
 *
 * Filename: amapi.c
 *
 * Purpose:  Contains routines related to handling packets
 *           defined by the AM package on the API side.
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "am/amidefs.h"
#include "am/amudefs.h"
#include "ap/apudefs.h"

/* Local storage */

/* AM API control block structure */
struct amapicb amapicblk;

/* The following arrays are used for sending and receiving responses
 * between the API and the SDK side. They can be global because the
 * protocol is stop and wait for AM request/response operations. The 
 * notifications use a different structure and they arrive
 * asynchronously
 */
swi_uint8 amrequestblock[AMMAXREQBKLEN];
swi_uint8 amresponseblock[AMMAXRESPBKLEN];

/*************
 *
 * Name: amgetapicbp
 *
 * Purpose: Return a pointer to the AM package control block for the
 *          API-side
 *             
 * Parms:   none
 *
 * Return:  pointer to the AM package control block structure
 *
 * Abort:   none
 *
 * Notes:   None
 *
 **************/
local struct amapicb *amgetapicbp( void )
{
    return &amapicblk;
}

/* 
 ******************* Request/Response Buffer routines ********************
 */
/*************
 *
 * Name: amgetparamp
 *
 * Purpose: Return a pointer to a CnS parameter field, given the AM CNS 
 *          packet type of interest and a pointer to the beginning of
 *          the AM packet
 *             
 * Parms:   amcnstype     - Which of the CNS-specific messages we want
 *                          the pointer to the parameter field of
 *          bufstartp     - Pointer to the start of the buffer that will/
 *                          already does contain the CNS message
 *
 * Return:  Number of bytes to the start of the parameter field
 *
 * Abort:   none
 *
 * Notes:   This is a general-purpose utility function for use on the API
 *          side. All Swi APIs that do Get or Set calls will need a pointer
 *          to the parameter field for the CnS object they are servicing
 *
 **************/
global swi_uint8 *amgetparamp( swi_uint16 amcnstype, swi_uint8 *bufstartp )
{
    swi_uint32 offset;

    /* The offset to the parameter field depends on the message type */
    switch( amcnstype )
    {
        case AMCNSREQUEST:
            offset = AMCNSREQTOTAL;
            break;

        case AMCNSRESPONSE:
            offset = AMCNSRESPTOTAL;
            break;

        default:
            return NULL;
    }

    /* If we get here, the offset must be valid */
    return offset + bufstartp;
}
/*************
 *
 * Name: amgetreqbufp
 *
 * Purpose: Return a pointer to sufficient storage for holding a 
 *          AM request for transmission to the SDK side. 
 *             
 * Parms:   none
 *
 * Return:  Pointer to an array of swi_uint8 large enough to contain
 *          the biggest possible AM request
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
global swi_uint8 *amgetreqbufp( void )
{
    return(&amrequestblock[0]); 
}
/*************
 *
 * Name: amgetrespbufp
 *
 * Purpose: Return a pointer to sufficient storage for holding a 
 *          AM response received from the SDK side. 
 *             
 * Parms:   none
 *
 * Return:  Pointer to an array of swi_uint8 large enough to contain
 *          the biggest possible AM response
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local swi_uint8 *amgetrespbufp( void )
{
    return(&amresponseblock[0]); 
}
/*************
 *
 * Name: amgetrespbufsz
 *
 * Purpose: Return the size of the response block, in bytes
 *             
 * Parms:   none
 *
 * Return:  size of the amresponseblock array, in bytes
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local swi_uint32 amgetrespbufsz( void )
{
    return sizeof( amresponseblock );
}

/* 
 ******************* Utility Routines ********************
 */

/*************
 *
 * Name: amconvert2rcode
 *
 * Purpose: Maps a AM packet result code to a SWI_RCODE
 *          that can be returned to the API called functions.
 *             
 * Parms:   amresultcode - received AM result code
 *
 * Return:  equivalent SWI_RCODE
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
local SWI_RCODE amconvert2rcode(enum swi_amresults amresultcode)
{
    SWI_RCODE rcodevalue;
    
    switch(amresultcode)
    { 
        case SWI_AMSUCCESS:
            rcodevalue = SWI_RCODE_OK;
            break;
            
        case SWI_AMTIMEOUT:
            rcodevalue = SWI_RCODE_REQUEST_TIMEOUT;
            break;
    
        case SWI_AMNODEVICE:
            rcodevalue = SWI_RCODE_DEVICE_UNAVAIL;
            break;
        
        case SWI_AMCNSERROR:
        case SWI_AMFWDWNABORTED:
            rcodevalue = SWI_RCODE_REQUEST_REJECTED;
            break;
        
        case SWI_AMAIFMISMATCH:
            rcodevalue = SWI_RCODE_NOT_SUP;
            break;
            
        case SWI_AMFWDWNBUSY:
            rcodevalue = SWI_RCODE_BUSY;
            break;
            
        case SWI_AMFAILED:
            rcodevalue = SWI_RCODE_FAILED;
            break;
            
        case SWI_AMDEVNOTSUP:
            rcodevalue = SWI_RCODE_DEVICE_NOT_SUP;
            break;
            
        case SWI_AMVERSERROR:
        default:
            rcodevalue = SWI_RCODE_NOT_COMPATIBLE;
            break;
    }
    
    return(rcodevalue);
}

/* 
 ******************* Packet Building Routines ********************
 */

/*************
 *
 * Name: ambuildrequest
 *
 * Purpose: Build a AM request packet in the provided storage area
 *          given the information contained in the parameter structure
 *             
 * Parms:   amsgp          - Pointer to storage into which the message
 *                           will be written
 *          amrrparmp      - Pointer to a structure containing the request
 *                           parameters
 *          amipcindex     - ipc channel over which this request will go.
 *
 * Return:  Number of bytes occupied by the AM packet.  For CNSREQUEST
 *          packets this value will include the CnS parameter field length.
 *
 * Abort:   none
 *
 * Notes:   AM request packets travel from the API-side to the SDK
 *          side
 *
 **************/
local swi_uint16 ambuildrequest( 
    swi_uint8 **amsgpp, 
    struct amrrparms *amrrparmp, 
    swi_uint8 amipcindex)
{
    swi_uint16 amsize;

    switch(amrrparmp->amparmtype)
    {
        case SWI_PARM_CNS:
            amrrparmp->amparm.amcns.amcnsparameter = amipcindex;
            amsize = ambuildcnsrequest( amsgpp, &amrrparmp->amparm.amcns );
            amsize += amrrparmp->amparm.amcns.amcnslength;
            break;

        case SWI_PARM_FWDWN:
            amsize = ambuildfwdwnrequest( amsgpp, &amrrparmp->amparm.amfwdwn );
            break;

        case SWI_PARM_AIRSRV:
            amsize = ambuildairserverrequest( amsgpp, 
                &amrrparmp->amparm.amairsrv);
            break;
            
       case SWI_PARM_PORTNAME:
            amsize = ambuildusbportnamerequest( amsgpp, 
                &amrrparmp->amparm.amusbportname);
            break;
            
       case SWI_PARM_RESET:
            amsize = ambuildmdmresetrequest( amsgpp, 
                &amrrparmp->amparm.amreset);
            break;
       case SWI_PARM_DEV_INFO:
            amsize = ambuilddevinforequest( amsgpp, 
                &amrrparmp->amparm.amdevinfo);
            break;
        default:
            break;
    }
    return(amsize);
}

/* 
 ******************* Parsing Routines ********************
 */

/*************
 *
 * Name: amparseresponse
 *
 * Purpose: This routine knows how to unpack the contents of AM
 *          response messages
 *             
 * Parms:   ampacketpp  - pointer to a pointer to an AM response
 *                        packet.
 *          amrrparmp   - Pointer to a caller-allocated structure
 *                        defined by AM package for exchanging 
 *                        Request/Response parameters. Saves
 *                        long lists of calling arguments
 *
 * Return:  TRUE  - the response has been parsed
 *          FALSE - the AM response type is unknown
 *
 * Abort:   none
 *
 * Notes:   Parses the fields from an AM response packet and returns
 *          the values to the caller in the structure provided
 *
 *          On return the packet pointer pointer is advanced to point
 *          to the CnS Parameter field for CnS Responses.
 *
 **************/
local swi_bool amparseresponse( 
    swi_uint8 **ampacketpp, 
    enum amrrparmtype amtype,
    struct amrrparms *amrrparmp)
{
    swi_bool retval = FALSE;

    switch(amtype)
    {
        case AMCNSRESPONSE:
            /* if the response type matches the request type */
            if(amrrparmp->amparmtype == SWI_PARM_CNS)
            {
                /* parse CnS specfic AM packet payload */
                amparsecnsresponse( ampacketpp,  &amrrparmp->amparm.amcns);
                retval = TRUE;
            }
            break;

        case AMFWDWNRESPONSE: 
            if(amrrparmp->amparmtype == SWI_PARM_FWDWN)
            {
                /* parse Firmware Download specific AM packet payload */
                amparsefwdwnresponse( ampacketpp,  &amrrparmp->amparm.amfwdwn);
                retval = TRUE;
            }
            break;

        case AMAIRSRVRESP:
            if(amrrparmp->amparmtype == SWI_PARM_AIRSRV)
            {
                /* parse Firmware Download specific AM packet payload */
                amparseairserverresponse( ampacketpp,
                    &amrrparmp->amparm.amairsrv);
                retval = TRUE;
            }
            break;
            
        case AMPORTNAMERESP:
            if(amrrparmp->amparmtype == SWI_PARM_PORTNAME)
            {
                /* parse Port Name specific AM packet payload */
                amparseportnameresponse( ampacketpp,
                    &amrrparmp->amparm.amusbportname);
                retval = TRUE;
            }
            break;

        case AMMDMRESETRESP:
            if( amrrparmp->amparmtype == SWI_PARM_RESET )
            {
                amparsemdmresetrequest( ampacketpp,
                    &amrrparmp->amparm.amreset);
                retval = TRUE;
            }
            break;
            
        case AMDEVINFORESP:
            if( amrrparmp->amparmtype == SWI_PARM_DEV_INFO )
            {
                amparsedevinforesp( ampacketpp,
                                    &amrrparmp->amparm.amdevinfo);
                retval = TRUE;
            }
            break;
            
        default:
            /* the AM response type is unsupported by the API side */
            retval = FALSE;
            break;
    }
    
    return(retval);
}

/*************
 *
 * Name: amparsenotify
 *
 * Purpose: This routine knows how to unpack the contents of AM
 *          response messages
 *             
 * Parms:   apipcmsgp  - pointer to a pointer to an AM response
 *                        packet.
 *          amtype     - type of AM packet to be parsed
 *          amrrparmp  - Pointer to a caller-allocated structure
 *                       defined by AM package for collecting notification
 *                       information.
 *
 * Return:  TRUE  - the notification has been parsed
 *          FALSE - the AM notification type is unknown
 *
 * Abort:   none
 *
 * Notes:   Parses the fields from an AM response packet and returns
 *          the values to the caller in the structure provided
 *
 *          On return the packet pointer pointer is advanced to point
 *          to the CnS Parameter field for CnS Notifications.
 *
 **************/
local swi_bool amparsenotify(
    swi_uint8 **apipcmsgp, 
    enum amrrparmtype amtype,
    struct amrrparms *parmsp)
{
    swi_bool retval = TRUE;
    
    /* parse data based on AM packet type */
    switch(amtype)
    {
        case AMCNSNOTIFICATION:
            parmsp->amparmtype = SWI_PARM_CNS;
            amparsecnsnotify(apipcmsgp, &parmsp->amparm.amcns);
            break;

        case AMFWDWNNOTIFICATION: 
            parmsp->amparmtype = SWI_PARM_FWDWN;
            amparsefwdwnnotify(apipcmsgp, &parmsp->amparm.amfwdwn);
            break;

        case AMAIRSRVCHGNOTIFY:
            parmsp->amparmtype = SWI_PARM_AIRSRV;
            amparseairchgnotify( apipcmsgp, &parmsp->amparm.amairsrv );
            break;

        default:
            retval = FALSE;
    }
    return(retval);
}

/* 
 ******************* Send Routine *********************
 */

/*************
 *
 * Name: amsendnwait
 *
 * Purpose: Format an API ongoing AM packet using the calling arguments
 *          send it on its way and block until a response is received.
 *          Pass the response back to the caller
 *             
 * Parms:   amhdrp     - Pointer to the beginning of the packet that
 *                       will contain an AM Message
 *          amparamsp  - [IN] Pointer to a structure containing valid AM
 *                       parameters sufficient to build an outgoing AM
 *                       request packet
 *                       [OUT] Contains the parsed fields from an incoming AM
 *                       response packet
 *          responsepp - Pointer to a pointer which will be loaded with
 *                       the address of the CnS Parameter field received
 *                       from the modem
 *
 * Return:  Value of SWI_RCODE
 *
 * Abort:   none
 *
 * Notes:   This is a blocking call which returns only after the modem
 *          has sent its response or the timeout (stored in amparamsp)
 *          has expired, whichever is sooner
 *
 *          This function returns the caller a pointer to the received
 *          buffer data in responsepp. The actual storage holding this
 *          data is global to this package. It cannot be overwritten 
 *          until the next icapi_rcv() call is posted which can only
 *          happen when the application sends the next message. There
 *          are NO RESOURCES TO RELEASE on the return path
 *
 **************/
global SWI_RCODE amsendnwait( 
    swi_uint8 *amhdrp, 
    struct amrrparms *amparamsp, 
    swi_uint8 **responsepp )
{
    SWI_RCODE retval;            /* To derive return to caller value */
    swi_bool result;                 /* Result of call to IPC read routine */
    swi_uint16 amsize;           /* Returned by pkt building routine */
    swi_uint16 amtype, amlength; /* Returned by amchecktype() call */
    swi_uint16 amhdrversion;     /* Returned by amchecktype() call */
    swi_uint32 buflength;        /* Used in call to icapi_rcv() */
    struct amapicb *amapicbp;    /* Pointer to CNS Pkg Control block */
    swi_uint8 *hdrp;             /* Local copy of calling argument */
    swi_uint8 *inbufp;           /* Local pointer to buffer from IPC RCV */
    swi_uint8 amresultcode;

    /* Assume a succesful return */
    retval = SWI_RCODE_OK;

    /* Initialize the control block pointer */
    amapicbp = amgetapicbp();

    /* initialize the local copy */
    hdrp = amhdrp;

    /* Build the AM packet first */
    amsize = ambuildrequest( &hdrp, 
                        amparamsp,
                        amapicbp->amapiipcrr.icipcindex);

    /* Send the packet to the SDK side */
    icapi_send(&amapicbp->amapiipcrr, amhdrp, amsize);

    /* Make a local copy of the buffer for storing the received IPC msg */
    inbufp = amgetrespbufp();

    /* 
     * Determine the maximum size of response that can be 
     * accommodated in the local buffer 
     */
    buflength = amgetrespbufsz();
    
    /* if a non-zero timeout is specified */
    if(amparamsp->amtimeout)
    {
        /* 
         * adjust the timeout value so to ensure that the API side
         * times out before the SDK side.
         */
        amparamsp->amtimeout = (AMTIMEGUARD + amparamsp->amtimeout) << 2;
    }

    /* Now, block waiting for a response from the modem or a timeout */
    result = icapi_rcv( &amapicbp->amapiipcrr, 
                        inbufp,
                        &buflength,
                        amparamsp->amtimeout);

    /* Determine if there was a timeout. Note, it's possible this 
     * is a real timeout or that the SDK task has disappeared
     */
    if( !buflength )
    {
        /* Timeout */
        return(SWI_RCODE_GET_TIMEOUT);
    }
  
    /* Parse the header of the received AM packet */
    amparseamheader(
        &amtype, 
        &amhdrversion, 
        &amresultcode, 
        &amlength, 
        &inbufp);
    
    /* if the SDK header version is different from what the API supports */
    if((amhdrversion != AMHDRVERSION ) ||  
       (amresultcode == SWI_AMVERSERROR))
    {
        return(SWI_RCODE_NOT_COMPATIBLE);
    }
    
    /* if this is not a short response from the SDK */
    if(amlength)
    {
        /* Parse AM response packet payload */
        if(!amparseresponse(&inbufp, amtype, amparamsp))
        {
            /* if response type is unknown */
            return(SWI_RCODE_UNEXPECTED_RESP);
        }
        
        /* check if an CnS error message needs to be saved */
        if((amtype == AMCNSRESPONSE) && (amresultcode == SWI_AMCNSERROR))
        {
            /* The error message should be right at the inbufp
             * after parsing out the contents of the API header.
             * Note the +1 below is to ensure the NULL termination
             * is included in the copy. The CnS parameter length
             * for error strings doesn't include the NULL term. 
             */
            SwiApSaveError( (swi_charp)inbufp,
                            amparamsp->amparm.amcns.amcnslength+1 );
        }   
    
        /* check if a Firmware Download error message needs to be saved */
        if((amtype == AMFWDWNRESPONSE) && (amresultcode == SWI_AMFWDWNABORTED))
        {
            /* The error message should be right at the inbufp
             * after parsing out the contents of the API header.
             * Note the +1 below is to ensure the NULL termination
             * is included in the copy. 
             */
            SwiApSaveError(amparamsp->amparm.amfwdwn.amerrorstr, 
                           amparamsp->amparm.amfwdwn.amerrorstrlen);
        }
    }
    
    /* return pointer to the end of the parsed fields */
    *responsepp = inbufp;
            
    /* convert the AM result code to a SWI_RCODE and return */
    return(amconvert2rcode(amresultcode));

}
/*************
 *
 * Name:    amapiwaitnotif
 *
 * Purpose: Waits for notification messages on IPC notification channel.
 *          Passes these back to caller  
 *             
 * Parms:   apipcmsgp       - pointer to input buffer where data is received
 *          apipcbuflngp    - pointer to length of input buffer
 *          parmsp          - pointer user supplied parameter structure to 
 *                            stored receive notification info in
 *          payloadpp       - memory location to place pointer to CnS payload
 *                            pointer.
 *          timeout         - response timeout value in milliseconds 
 *                            0 = wait forever
 *
 * Return:  TRUE            - notification message received successfully
 *          FALSE           - error in receiving notification message
 *
 * Abort:   if received AM packet header version is not supported
 *          if received AM packet type is not supported
 *
 * Notes:  When timeout occurs this function returns TRUE and sets the buffer 
 *         length return parameter to zero.
 *          
 *
 **************/
global swi_bool amapiwaitnotif(
    swi_uint8  *apipcmsgp, 
    swi_uint32 *apipcbuflngp, 
    struct amrrparms *parmsp,
    swi_uint8  **payloadpp,
    swi_uint32 timeout )
{
    swi_bool result; /* result of waiting for notification message on IPC channel */
    swi_uint16 amtype, amlength; 
    swi_uint16 amhdrver;         
    swi_uint8  amresultcode;
    swi_uint8  *packetp;
    
    /* initialize temporary packet pointer to the beginning of input buffer */
    packetp = apipcmsgp;

    /* Get Notification Channel Handle */
    struct amapicb *amapnotcbp  = amgetapicbp();

    /* Wait for IPC message on notification channel */
    /* Note: timeout specified as 0 means waits forever */
    result = icapi_rcv( &amapnotcbp->amapiipcnot, 
                       packetp, 
                       apipcbuflngp, 
                       timeout);

    /* When notification is received successfully */
    if (result && (*apipcbuflngp != 0))
    {
        /* Parse the header of the received AM packet */
        amparseamheader(&amtype, 
                        &amhdrver, 
                        &amresultcode, 
                        &amlength, 
                        &packetp);

        /* First ensure the AM packet header matches */
        if( amhdrver != AMHDRVERSION )
        {
            /* Put both values into our 32 bit error data */
            amhdrver= amhdrver<< 16 | AMHDRVERSION;
            erAbort("API/SDK IPC hdr mismatch\n", amhdrver );
        }
        
        /* Parse known AM packet type */
        if(!amparsenotify(&packetp, amtype, parmsp))
        {
            erAbort("API/SDK IPC unknown packet\n", amtype );
        }
    }
    /* 
     * Return advanced packet pointer to caller
     * In the case of a CnS notification this will point at the 
     * CnS payload
     */
    *payloadpp = packetp;

    return result;
}
/*************
 *
 * Name: aminitapi
 *
 * Purpose: Initialize the AM package for the API side
 *             
 * Parms:   none
 *
 * Return:  TRUE  - Initialized successfully
 *          FALSE - Unable to open an IPC channel for use. All ICCNMCMAX 
 *                  channels are in use.
 *
 * Abort:   none
 *
 * Notes:   Don't confuse this function with the SDK-side function
 *          of exactly the same name.  There should never be a linker 
 *          conflict because this file's object is not part of the
 *          SDK link and vice-versa
 *
 **************/
global swi_bool aminitapi( void )
{
    swi_bool retval;
    struct amapicb *amapicbp = amgetapicbp();
    int i;
    swi_uint16  numappchan = 0;
    swi_uint8   *chnamep;
    swi_uint8   *pchnamep;
    swi_uint16  channel, pchannel;
    
    /* Initialize the Channel Information package */
    ciinit();
    
    /* 
     * Loop through all the channels to get the first available 
     * CNRR/Notification pair.
     */
    numappchan = cigetnumRRchan( );
    /* find the first CNRR/Notification IPC pair available */
    for (i=0; i < numappchan; i++)
    {
        /* Initialize the IC handle for Request/Response channel */
        channel = cigetnextrrchannum(i);
        chnamep = cigetchannelname(channel);
        retval = ciinitapiipcchan(&amapicbp->amapiipcrr, chnamep);
        if ( retval )
        {
            /* get the peer Notification channel as well */
            pchannel = cigetpeerchan( channel );
            pchnamep = cigetchannelname(pchannel);
            /* Initialize the IC handle for Notification channel */
            retval = ciinitapiipcchan(&amapicbp->amapiipcnot, pchnamep);
            /* if both CNRR/Notification channels are OK. we can proceed */
            if ( retval )
                break;
        }
    }
    return retval;
}

/*
 * $Log: am_api.c,v $
 */

