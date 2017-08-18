/*
 *  $Id: aptestcmupgrade.c,v 1.9 2009/02/02 20:27:01 bsiu Exp $
 *
 *  Filename: aptestcmupgrade.c
 *
 *  Purpose:  Contains functions for testing Firmware Download APIs.
 *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <stdio.h>

/* include files */
#include "SwiDataTypes.h"
#include "sl/sludefs.h"
#include "apudefs.h"
#include "aptestdefs.h"


/* Functions */
/**************
 *
 * Name:     doSwiNvStore
 *
 * Purpose:  Exercises the SwiNvStore API - calls the API and prints
 *           information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiNvStore(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_NvStore nvStore;
    swi_char lasterror[80];

    nvStore.sizeStruct = sizeof(struct SWI_STRUCT_NvStore);
    nvStore.eOperation = SWI_NVOPER_Backup;
    nvStore.eCategory  = SWI_NVCAT_User;
 
    /* Fill up our data  */
    retcode = SwiNvStore( nvStore, timeout );

    printf ("\n");

    if( retcode == SWI_RCODE_OK )
        printf ("NV Store command is being processed\n");
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("NV Store command timed out\n");
    else
    {
        printf ("NV Store command is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }

}

/**************
 *
 * Name:     doSwiFirmwareDownload
 *
 * Purpose:  Exercises the SwiFirmwareDownload API - calls the API and prints
 *           information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiFirmwareDownload(unsigned int timeout)
{
    printf ("\nPlease use the FirmwareDownload sample application to perform this action.\n");
    
    doDeprecated(timeout);
}
/*
 * $log: $
 */
