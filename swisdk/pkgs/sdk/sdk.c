/*************
 *
 * $Id: sdk.c,v 1.6 2009/05/08 22:08:44 blee Exp $
 *
 * Filename: swi_sdk.c
 *
 * Purpose:  Mainline code for the SDK user-mode task. This file
 *           provides the overall context for the SDK task 
 *           (process). This file contains the "main" routine for
 *           the SDK.
 *           
 *
 * Copyright: © 2008 Sierra Wireless Inc., all rights reserved
 *
 **************/

/* include files */
#include "../aa/aaglobal.h"    /* system wide typedefs */
#include "sdk/sdkidefs.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/* Local storage for the SDK control block */
local struct swi_sdkcb sdkcbk;

/*************
 *
 * Name: sdkgetcbkp
 *
 * Purpose: Package entry point to obtain a pointer to the SDK task's 
 *          control block
 *           
 * Params:  No values
 *
 * Return:  Pointer to the SDK task's control block structure
 *
 * Abort:   none
 *
 * Notes:   This function can be local for now
 *
 **************/
local struct swi_sdkcb *swi_sdkgetcbkp( void )
{    
    /* Return a pointer to the control block */
    return &sdkcbk;
}


/*************
 *
 * Name: main
 *
 * Purpose: Main entry point for SDK daemon.
 *           
 * Params:  No values
 *
 * Return:  No value
 *
 * Abort:   none
 *
 * Notes:   none
 *
 **************/
int main(int argc, const char **argv)
{
    
    struct swi_sdkcb *sdkcbkp;


    /* Perform necessary initialization for 
     * the underlying process. 
     *
     * This MUST be done before anything else
     */
    swi_ossdkprocessinit();    

    /* don't try to write to stdout or stderr from anywhere inside
     * the SDK task -- they've been closed and this process is a 
     * detached daemon. Use swi_ossdklog instead, here, and look 
     * in /var/log/<logfile> where <logfile> is distro-dependent
     */

    sdkcbkp = swi_sdkgetcbkp();
 
       
    /* Call the system startup package. This will initialize
     * all internal packages and spawn all the tasks needed
     * by the SDK process
    */
    suinit();

    /* Register with DL so we can log, initially enabled */
    dlregister ("SDK", &sdkcbkp->sdkdlcb, TRUE);

    /* Log the startup with version, ie, <Tag Name> for released version
     * or "(NO TAG)" for development version 
     */
    dlLog(  &sdkcbkp->sdkdlcb, SDK_CLASSA, 
            "Process Started, version: %s\n", 
            (swi_uint32)amgettagname(), 
            (swi_uint32)NULL, 
            (swi_uint32)NULL,
            (swi_uint32)NULL);

    /* This daemon never ends it just keeps on going */
    swi_ossdkwaiterinit( &sdkcbkp->sdkwaiter );
    while(1)
    {
        /* Just so it doesn't consume any cycles, 
         * we'll put this thread permanently to sleep 
         */
        swi_ossdkwaiterwait( &sdkcbkp->sdkwaiter, 0 );
    }
}

/* 
 * $Log: sdk.c,v $
 */
