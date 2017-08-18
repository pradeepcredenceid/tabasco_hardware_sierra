/*************
 *
 * $Id: ds_app.c,v 1.5 2009/06/01 23:25:22 blee Exp $
 *
 * Filename: ds_app.c
 *
 * Purpose:  Contains DS package routines for use by applications 
 *           to exchange traffic with the DS counterpart within
 *           the SDK process
 *
 * Note:     This file is not intended to be linked into the SDK
 *           or API side images. It is intended for a separate,
 *           standalone application to enable that application
 *           to communicate with the SDK process.
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "ap/SwiDataTypes.h"
#include "ap/SwiRcodes.h"

#include "ds/dsidefs.h"

/* Local storage */

/* Application Side DS Control block */
struct dsappcb dsappcbk;
/* Note: channel is the index */

/*************
 *
 * Name: dsgetappipccbp
 *
 * Purpose: Return a pointer to the DS package IPC control block for a 
 *          specific channel
 *             
 * Parms:   channel  - channel id which is index to the array of IPC control
 *                     blocks
 *
 * Return:  pointer to the DS package control block structure
 *
 * Abort:   none
 *
 * Notes:   Used by Application side only
 *
 **************/
local struct dsappipccb *dsgetappipccbp(swi_uint32 channel)
{
    return &(dsappcbk.dsdxchancb[channel]);
}

/*************
 *
 * Name: dsopen
 *
 * Purpose: Open the specified IPC channel in preparation for 
 *          communication from an application to the SDK process
 *             
 * Parms:   callernamep - Pointer to a string containing the caller's
 *                        name for troubleshooting purposes
 *          channel     - One of the pre-defined IPC channel IDs in the 
 *                        icudefs.h file
 *
 * Return:  SWI_RCODE_OK           - Channel opened successfully
 *          SWI_RCODE_DEVICE_INUSE - Channel already in use
 *
 * Abort:   None
 *
 * Notes:   If you receive a SWI_RCODE_DEVICE_INUSE return code it 
 *          may be because another process has the channel open or
 *          you have previously opened it yourself and forgot. In
 *          order to distinguish between these two cases, additional
 *          checking would need to be made to the code. 
 *
 **************/
global SWI_RCODE dsopen( swi_uint8 *callernamep, swi_uint32 channel ) 
{
    swi_bool retval;
    struct dsappipccb *dsipccbp = dsgetappipccbp( channel );

    /* Initialize the IC handle for Request/Response channel */
    retval = ciinitapiipcchan(&(dsipccbp->dsipccbhdl), callernamep);
    
    /* Only update the control block entries if the IC call succeeded */
    if( retval )
    {
        /* Store the caller-supplied name string */
        dsipccbp->dsnamep = callernamep;

        /* Store the supplied channel number for easier identification
         * during troubleshooting, if need be
         */
        dsipccbp->dschannel = channel;

        return SWI_RCODE_OK;
    }

    return SWI_RCODE_DEVICE_INUSE;
}

/*************
 *
 * Name: dsappsend
 *
 * Purpose: Sends a packet from the application side through the 
 *          specified IPC channel to the SDK process
 *             
 * Parms:   channel     - One of the pre-defined IPC channel IDs in the 
 *                        icudefs.h file
 *          senddatap   - Pointer to a buffer of data to send over the
 *                        IPC channel
 *          length      - Number of bytes pointed to by senddatap
 *
 * Return:  SWI_RCODE_OK          - Packet sent successfully
 *          SWI_RCODE_INVALID_PAR - Channel number mismatch
 *
 * Abort:   None
 *
 * Notes:   The caller is responsible for managing their own buffer 
 *          which can be considered free for re-use upon return from
 *          this call
 *
 **************/
global SWI_RCODE dsappsend( swi_uint32 channel, 
                    swi_uint8 *senddatap, 
                    swi_uint32 length)
{
    struct dsappipccb *dsipccbp = dsgetappipccbp( channel ); 

    /* Make sure the caller isn't mixing up his channels */
    if( channel != dsipccbp->dschannel )
    {
        return SWI_RCODE_INVALID_PAR;
    }
    
    /* Send the packet on its way */
    icapi_send( &(dsipccbp->dsipccbhdl), senddatap, length );

    return SWI_RCODE_OK;
}

/*************
 *
 * Name: dsrcvapp
 *
 * Purpose: Blocks on the specified IPC channel waiting for a packet 
 *          from the SDK process. 
 *             
 * Parms:   channel         - One of the pre-defined IPC channel IDs in the 
 *                            icudefs.h file
 *          rcvdatap        - Pointer to a buffer into which the received
 *                            data will be placed
 *          lengthp[IN/OUT] - In, points to the amount of space (bytes)
 *                            available in the rcvdatap buffer. Out, 
 *                            contains the number of bytes received from 
 *                            the IPC channel
 *          timeout         - Length of time to wait for a packet before
 *                            giving up (0 means wait forever)
 *                              
 * Return:  SWI_RCODE_OK              - Successful call
 *          SWI_RCODE_REQUEST_TIMEOUT - Timed out waiting for read data
 *          SWI_RCODE_FAILED          - Request failed for some reason
 *          SWI_RCODE_INVALID_PAR     - Channel number mismatch
 *
 * Abort:   None
 *
 * Notes:   None 
 *
 **************/
global SWI_RCODE dsreceive( swi_uint32 channel, 
                      swi_uint8 *rcvdatap, 
                      swi_uint32 *lengthp,
                      swi_uint32 timeout )
{
    swi_uint32 rcvlength;
    swi_bool result;
    struct dsappipccb *dsipccbp = dsgetappipccbp( channel );
    
    if( channel != dsipccbp->dschannel )
    {
        return SWI_RCODE_INVALID_PAR;
    }
    
    /* Convert the caller's length into a local variable */
    rcvlength = *lengthp;

    /* Prepare to receive some incoming traffic */
    result = icapi_rcv( &(dsipccbp->dsipccbhdl),rcvdatap,&rcvlength,timeout );

    /* Check for a timeout */
    if( !rcvlength )
    {
        *lengthp = 0;
        return SWI_RCODE_REQUEST_TIMEOUT;
    }
    if( !result )
    {
        /* General purpose failure return code */
        return SWI_RCODE_FAILED;
    }

    /* Give the caller how many bytes were received */
    *lengthp = rcvlength;

    return SWI_RCODE_OK;
}

/*************
 *
 * Name: dsgetmaxnumchan
 *
 * Purpose: Return the maximum number of channels that can be supported by SDK
 *             
 * Parms:   numchanp - pointer to  parameters set to DSMAXSHELLS
 *
 * Return:  SWI_RCODE_OK  - always
 *
 * Abort:   none
 *
 * Notes:   This function provides an entry point for the Application to 
 *          retrieve the maximum number of channels that this version of 
 *          the SDK support. This number must  equal to the maximum number 
 *          of channels an Application can support.
 *
 **************/
global SWI_RCODE dsgetmaxnumchan( swi_uint16 *numchanp )
{
    /* Returns the maximum number of Demux IPC channels supported by SDK */
    *numchanp = CIMAXIPCCHAN;
    return SWI_RCODE_OK;
}

/*
 * $Log: ds_app.c,v $
 */

