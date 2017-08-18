/*************
 *
 * $Id: dstask_sdk.c,v 1.4 2010/06/09 23:39:20 epasheva Exp $
 *
 * Filename: dsipctask_sdk.c
 *
 * Purpose:  DS package IPC task associated functions
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "ds/dsidefs.h"

/* Note: There is no explicit dsipctaskinit() for creating DS tasks 
 * because the instances of DS 'shells' are called by other package and run
 * in that package's context.
 */

/*************
 *
 * Name: dssend
 *
 * Purpose: DS package send interface over a specified channel
 *             
 * Parms:   packetptr - pointer to AM packet
 *          packetlen - length of AP packet
 *          chanid    - channel id
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   On return it is safe for the caller to release the buffer
 *          memory if applicable.
 *          This is thread safe function.
 *
 **************/
global void dssend( 
    swi_uint8 *packetptr, 
    swi_uint32 packetlen, 
    swi_uint16 chanid)
{
    struct icipccb *ipccbp; 

    switch(cigetifacetype(cigetchannelname(chanid)))
    {
        case CIUIF:
            /* Send the message over the endpoint  */
            dsep_send( cigetifusetype(chanid), packetptr, packetlen);
            break;
        case CIIPC:
            /* Obtain pointer to the IPC channel structure */
            ipccbp = cigetipcp(chanid);
            
            /* Send the message over the IPC channel  */
            icsdk_send( ipccbp, packetptr, packetlen );
            break;
        default:
            erAbort("dssend: unknown channel type", 0 );
            break;
        }
}

/*************
 *
 * Name: dsclose
 *
 * Purpose: Closes specific IPC channel
 *             
 * Parms:   chanid - channel id
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   This function clears channels structure and un-installs the 
 *          receive callback function 
 *
 **************/
global void dsclose(swi_uint16 chanid)
{
    /* Not implemented at this time */
}

/*
 * $Log: dstask_sdk.c,v $
 */

