/*
 * $Id: ci_api.c,v 1.1 2009/06/01 23:13:46 blee Exp $
 *
 * Filename: ci_api.c
 *
 * Purpose:  Routines callable by other packages to obtain information on
 *           IPC channels used by the API.
 *
 * NOTES:   
 *
 * Copyright: © 2009 Sierra Wireless Inc., all rights reserved
 */


/* include files */

#include "ciidefs.h"

/* Global Control Block */

/* local variables */
/* Functions */

/*
 * Name:    ciinitapiipcchan
 *
 * Purpose: Opens the IPC channel
 *
 * Params:  ipccbp       - pointer to user provided space for IC IPC structure.
 *          channelnamep - pointer to IPC channel name.
 *
 * Return:  TRUE  - OK
 *          FALSE - failed
 *
 * Note:    None.
 *
 * Abort:   Invalid service name.
 *
 */
global swi_bool ciinitapiipcchan( 
    struct icipccb *ipccbp,
    swi_uint8 *channelnamep)
{
    swi_uint16 channel;
    swi_uint16 totalnumchannels;     
    struct cigblctrlblk *cigcbp = cigetgcbp();
    
    totalnumchannels = cigetnumchannels();
    channel = cigetindex (channelnamep);

    if (channel >= totalnumchannels)
        /* Invalid service name */
        /* log a fatal error */
        erAbort("CI - Invalid service name(a)", (swi_uint32) channelnamep);

    /* init the IPC channel */
    if (cigcbp->ciipcchanlist[channel].cichanfixedinfop->ciiftype ==
        CIIPC)
    {
        return (icinitapihndl(  ipccbp, 
                                cigcbp->ciipcchanlist[channel].cichannel,
                                channelnamep));
    }
    return FALSE;
}

/*
 * $Log: ci_api.c,v $
 */
