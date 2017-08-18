/*************
 *
 * $Id: hpreset.c,v 1.2 2009/04/30 23:23:57 blee Exp $
 *
 * Filename: hpreset.c
 *
 * Purpose:  Contains code that handles requests to reset the 
 *           modem
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "aa/aaglobal.h"
#include "am/amudefs.h"
#include "hp/hpidefs.h"

/*************
 *
 * Name:    hpresetmdm
 *
 * Purpose: Entry point, called whenever there is an incoming request to
 *          reset the modem from the API side.
 *             
 * Parms:   inipcmsgp   - Pointer to the payload portion of an
 *                        AMMDMRESETREQ packet.
 *          memfreep    - Pointer to the handle associated with the incoming
 *                        message that can be released or NULL if non-releasable
 *          ipcchannel  - IPC channel that request came in from
 *
 * Return:  None
 *
 * Abort:   None
 *
 * Notes:   This function runs outside of the HPPH task's context
 *
 **************/
global void hpresetmdm( 
    swi_uint8 *inipcmsgp, 
    swi_uint8 *memfreep, 
    swi_uint8 ipcchannel )
{
    struct ammdmresetparms reset;
    swi_uint16             packetsize;   /* Response packet size in bytes */
    swi_uint8              *messagep;    /* Local copy of inipcmsgp */
    swi_uint8              *bufp = NULL; 
    swi_uint8              *resp;        /* Pointer to reply buffer */
    swi_uint8              replybuf[ AMRESETMDMRESPTOTAL ];

    /* Make a local copy of the incoming message */
    messagep = inipcmsgp;
    
    /* Parse the message and proceed with the request */
    amparsemdmresetrequest( &messagep, &reset );

    /* We now have the information needed to initiate a reset */

    /* Get a HIP transmission buffer */
    bufp = hpgetbuf();
    
    /* Send reset request to the device */
    hpTxPacket( HPRESETREQ, reset.amresettype, bufp, 0 );

    /* Get pointer to reply buffer */
    resp = &replybuf[0];

    /* Build and send a response to the caller on the API side */
    packetsize = ambuildmdmresetresp( &resp, &reset );

    /* Send the reply to the caller */
    amsdk_send( &replybuf[0], packetsize, ipcchannel );

    /* All done, release the memory block */
    if( memfreep != NULL )
    {
        mmbufrel( memfreep );
    }
}

/*
 * $Log: hpreset.c,v $
 */

