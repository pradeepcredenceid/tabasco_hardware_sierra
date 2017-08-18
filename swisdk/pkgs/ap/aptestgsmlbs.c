/*
 *  $Id: aptestgsmlbs.c,v 1.4 2010/06/10 00:26:12 epasheva Exp $
 *
 *  Filename: aptestgsmlbs.c
 *
 *  Purpose:  Contains functions for testing GSM LBS APIs.
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

/*************
 *
 * Name:    apfixtypemap - LBS fix type Mapping Table
 *
 * Purpose: This table maps Fix Type enum to a string that can be printed
 *          to the user.
 *
 *************/
char apfixtypemap[][25] = 
  {{"UNKNOWN"}, 
   {"StandAlone" }, 
   {"MS-Based ONLY"}, 
   {"MS-Assisted ONLY"}, 
   {"Optimized for Speed"}, 
   {"Optimized for Accuracy"}, 
   {"Optimized for Data"}};

/*************
 *
 * Name:    apnireqmap - NI request set response error code Mapping Table
 *
 * Purpose: This table maps TYPE_LbsNiReqErrCode enum to a string that can be 
 *          printed to the user.
 *
 *************/
char apnireqmap[][25] = 
  {{"No Error"}, 
   {"Handle mismatch" }, 
   {"User rsp timed-out"}, 
   {"No User rsp required"}};

/* Functions */
/**************
 *
 * Name:     doSwiSetLbsPaKeepWarmStart
 *
 * Purpose:  Exercises the SwiSetLbsPaKeepWarmStart API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiSetLbsPaKeepWarmStart(unsigned int timeout)
{
    SWI_RCODE retcode;
    swi_char lasterror[80];
 
    retcode = SwiSetLbsPaKeepWarmStart(timeout );

    if( retcode == SWI_RCODE_OK )
    {
        printf ("\nLBS PA Keep Warm Start sent\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("LBS PA Keep Warm Start request timed out\n");
    else
    {
        printf ("LBS PA Keep Warm Start request is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}
/**************
 *
 * Name:     doSwiGetLbsPaWarmStatus
 *
 * Purpose:  Exercises the SwiSetLbsPaKeepWarmStart API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetLbsPaWarmStatus(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_LbsPaWarmStatus lbsPaWarmStatus;
    swi_char lasterror[80];
 
    lbsPaWarmStatus.sizeStruct = sizeof(struct SWI_STRUCT_LbsPaWarmStatus); 
    retcode = SwiGetLbsPaWarmStatus(&lbsPaWarmStatus, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        printf ("\nLBS PA Keep Warm Status:\n");
        printf ("    Keep Warm Processing Enabled: %s\n", 
                (lbsPaWarmStatus.bEnabled?"TRUE":"FALSE"));
        printf ("    Currently Warm:               %s\n", 
                (lbsPaWarmStatus.eStatus?"TRUE":"FALSE"));
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("LBS PA Keep Warm Status request timed out\n");
    else
    {
        printf ("LBS PA Keep Warm Status request is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}

/**************
 *
 * Name:     doSwiSetLbsPaKeepWarmStop
 *
 * Purpose:  Exercises the SwiSetLbsPaKeepWarmStop API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiSetLbsPaKeepWarmStop(unsigned int timeout)
{
    SWI_RCODE retcode;
    swi_char lasterror[80];
 
    retcode = SwiSetLbsPaKeepWarmStop(timeout );

    if( retcode == SWI_RCODE_OK )
    {
        printf ("\nLBS PA Keep Warm Stop sent\n");
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("LBS PA Keep Warm Stop request timed out\n");
    else
    {
        printf ("LBS PA Keep Warm Stop request is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}
/**************
 *
 * Name:     doSwiGetLbsFixSettings
 *
 * Purpose:  Exercises the SwiGetLbsFixSettings API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetLbsFixSettings(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_LbsFixSettings lbsFixSettings;
    swi_char lasterror[80];

    lbsFixSettings.sizeStruct = sizeof(struct SWI_STRUCT_LbsFixSettings); 

    retcode = SwiGetLbsFixSettings( &lbsFixSettings, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        printf ("\nLBS Fix Settings:\n");
        printf ("   Fix type setting use mask:\n");
        printf ("      readable: %s\n", 
                (lbsFixSettings.sFixTypeUserOp.bReadable?"TRUE":"FALSE"));
        printf ("      writable: %s\n", 
                (lbsFixSettings.sFixTypeUserOp.bWritable?"TRUE":"FALSE"));
        printf ("   Fix type:    %s\n", apfixtypemap[lbsFixSettings.eFixType]);
        printf ("   Performance setting use mask:\n");
        printf ("      readable: %s\n", 
                (lbsFixSettings.sPerformanceUserOp.bReadable?"TRUE":"FALSE"));
        printf ("      writable: %s\n", 
                (lbsFixSettings.sPerformanceUserOp.bWritable?"TRUE":"FALSE"));
        printf ("   Performance: %d seconds\n",
                (int)lbsFixSettings.nPerformance);
        printf ("   Accuracy setting use mask:\n");
        printf ("      readable: %s\n", 
                (lbsFixSettings.sAccuracyUserOp.bReadable?"TRUE":"FALSE"));
        printf ("      writable: %s\n", 
                (lbsFixSettings.sAccuracyUserOp.bWritable?"TRUE":"FALSE"));
        printf ("   Accuracy: %d metres\n", (int)lbsFixSettings.nAccuracy);
        printf ("   Fix Count setting use mask:\n");
        printf ("      readable: %s\n", 
                 (lbsFixSettings.sFixCountUserOp.bReadable?"TRUE":"FALSE"));
        printf ("      writable: %s\n", 
                 (lbsFixSettings.sFixCountUserOp.bWritable?"TRUE":"FALSE"));
        printf ("   Fix Count:   %d\n", (int)lbsFixSettings.nFixCount);
        printf ("   Fix Ratesetting use mask:\n");
        printf ("      readable: %s\n", 
                 (lbsFixSettings.sFixRateUserOp.bReadable?"TRUE":"FALSE"));
        printf ("      writable: %s\n", 
                 (lbsFixSettings.sFixRateUserOp.bWritable?"TRUE":"FALSE"));
        printf ("   Fix Rate:    %d seconds\n", (int)lbsFixSettings.nFixRate);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("LBS Fix Settings fetch request timed out\n");
    else
    {
        printf ("LBS Fix Settings fetch request is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}
/**************
 *
 * Name:     doSwiSetLbsFixSettings
 *
 * Purpose:  Exercises the SwiSetLbsFixSettings API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiSetLbsFixSettings(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_LbsFixSettings lbsFixSettings;
    swi_char lasterror[80];

    lbsFixSettings.sizeStruct = sizeof(struct SWI_STRUCT_LbsFixSettings); 
    lbsFixSettings.eFixType = SWI_LBSFIX_Autonomous;
    lbsFixSettings.nPerformance = 255;
    lbsFixSettings.nAccuracy = 50;
    lbsFixSettings.nFixCount = 1000;
    lbsFixSettings.nFixRate = 1;
 
    retcode = SwiSetLbsFixSettings( &lbsFixSettings, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        printf ("\nLBS FIX Settings set successful: status[%d]\n", 
                lbsFixSettings.eResult);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("LBS Fix Settings set request timed out\n");
    else
    {
        printf ("LBS Fix Settings set request is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}
/**************
 *
 * Name:     doSwiSetLbsNiReq
 *
 * Purpose:  Exercises the SwiSetLbsNiReq API - calls the API and 
 *           prints information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiSetLbsNiReq(unsigned int timeout)
{
    SWI_RCODE retcode;
    struct SWI_STRUCT_LbsNiReq lbsNiReq;
    swi_char lasterror[80];
 
    lbsNiReq.sizeStruct = sizeof(struct SWI_STRUCT_LbsFixSettings); 
    lbsNiReq.nNiId = 1;
    lbsNiReq.eReqResp = LBSNIREQRESP_Reject; 

    retcode = SwiSetLbsNiReq( &lbsNiReq, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        printf ("\nLBS NI Request result code:  %s\n", 
                 apnireqmap[lbsNiReq.eErrCode]);
    }
    else if (retcode == SWI_RCODE_REQUEST_TIMEOUT)
        printf ("LBS NI Request timed out\n");
    else
    {
        printf ("LBS NI Request is rejected: %d\n", retcode);
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}
/*
 * $Log: aptestgsmlbs.c,v $
 */
