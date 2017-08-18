/*
 *  $Id: aptestroutines.c,v 1.42 2010/06/10 00:27:06 epasheva Exp $
 *
 *  Filename: aptestroutines.c
 *
 *  Purpose:  Contains functions implementing specific tests called 
 *            by the main() routine in aptest.c
 *
 * Copyright: © 2008 Sierra Wireless, Inc., all rights reserved
 *
 */

/* Linux definitions */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* include files */
#include "SwiDataTypes.h"
#include "sl/sludefs.h"

#include "apudefs.h"
#include "aptestdefs.h"

#include "SwiApiCdmaNetwork.h"


/* Defines */

/* global data */
swi_char fwver[256];
swi_char imei[32];

/* Data structure for testing SwiPstSetHomeSidNid(). The numbers in this
 * array are dummies, for testing purposes only. Modify this structure
 * as required for configuring an actual modem
 */
SWI_STRUCT_HomeSidNid testsidnid [] =
{
       {{{ 65535, 65535 }}},
       {{{ 16384, 65535 }}},
       {{{ 4139, 65535 }}},
       {{{ 1000, 65535 }}},
       {{{ 1001, 65535 }}},
       {{{ 1002, 65535 }}},
       {{{ 1003, 65535 }}},
       {{{ 1004, 65535 }}},
       {{{ 1005, 65535 }}},
       {{{ 1006, 65535 }}},
       {{{ 1007, 65535 }}},
       {{{ 1008, 65535 }}},
       {{{ 1009, 65535 }}},
       {{{ 1010, 65535 }}},
       {{{ 1020, 65535 }}},
       {{{ 1030, 65535 }}},
       {{{ 1040, 65535 }}},
       {{{ 1050, 65535 }}},
       {{{ 1060, 65535 }}}
};


/* Notification Handler Testing */
extern swi_uint8  inbuf[]; /* receive buffer for notification handler 
                            * function 
                            */
extern struct SwiNotifyVariant sNotifyVariant; /* holds data for notification
                                                * type 
                                                */
/* Our table of functions */
struct aptesttab ttabs[] = 
{
    {(void*)&doSwiGetFirmwareVersion,    "t1", "SwiGetFirmwareVersion"},
    {(void*)&doSwiGetImei,               "t2", "SwiGetImei"},
    {(void*)&doSwiGetBandInfo,           "t3", "SwiGetBandInfo"},
    {(void*)&doSwiGetAvailableFeatures,  "t4", "SwiGetAvailableFeatures"},
    {(void*)&doSwiSetMEPUnlock,          "t5", "SwiSetMEPUnlock"},
    {(void*)&doSwiGetRssiBer,            "t6", "SwiGetRssiBer"},
    {(void*)&doSwiGetMEIDESN,            "t7", "SwiGetMEIDESN"},
    {(void*)&doSwiSetTempModePref,       "t8", "SwiSetTempModePref"},
    {(void*)&doSwiPstSetHomeSidNid,      "t9", "SwiPstSetHomeSidNid"},
    {(void*)&doSwiGetTrafficStatistics,  "t10", "SwiGetTrafficStatistics"},
    {(void*)&doSwiResetTrafficStatistics,"t11", "SwiResetTrafficStatistics"},
    {(void*)&doSwiRegisterCallback,      "t12", "SwiRegisterCallback"},
    {(void*)&doSwiActivateProfile,       "t13", "SwiActivateProfile(Start)"},
    {(void*)&doSwiDeActivateProfile,     "t14", "SwiActivateProfile(End)"},
    {(void*)&doSwiSetSimVerify,          "t15", "SwiSetSimVerify"},
    {(void*)&doSwiSetSimLock,            "t16", "SwiSetSimLock"},
    {(void*)&doSwiGetSimLock,            "t17", "SwiGetSimLock"},
    {(void*)&doSwiSetSimPassword,        "t18", "SwiSetSimPassword"},
    {(void*)&doSwiChv2StatusKick,        "t19", "SwiChv2StatusKick"},
    {(void*)&doSwiGetGsmProfileSummary,  "t20", "SwiGetGsmProfileSummary"},
    {(void*)&doSwiGetGsmProfileBasic,    "t21", "SwiGetGsmProfileBasic"},
    {(void*)&doSwiGetGsmProfileQos,      "t22", "SwiGetGsmProfileQos"},
    {(void*)&doSwiGetGsmProfileDns,      "t23", "SwiGetGsmProfileDns"},
    {(void*)&doSwiGetGsmProfileTft,      "t24", "SwiGetGsmProfileTft"},
    {(void*)&doSwiSetDefaultProfile,     "t25", "SwiSetDefaultProfile"},
    {(void*)&doSwiSetGsmProfileBasic,    "t26", "SwiSetGsmProfileBasic"},
    {(void*)&doSwiSetGsmProfileQos,      "t27", "SwiSetGsmProfileQos"},
    {(void*)&doSwiSetGsmProfileDns,      "t28", "SwiSetGsmProfileDns"},
    {(void*)&doSwiSetGsmProfileTft,      "t29", "SwiSetGsmProfileTft"},
    {(void*)&doSwiEraseProfile,          "t30", "SwiEraseProfile"},
    {(void*)&doSwiGetSerialNumber,       "t31", "SwiGetSerialNumber"},
    {(void*)&doSwiGetTestHistoryByte,    "t32", "SwiGetTestHistoryByte"},
    {(void*)&doSwiGetSetModemDisable,    "t33", "SwiGetSetModemDisable"},
    {(void*)&doSwiGetFeatureCustomizations,"t34", "SwiGetFeatureCustomizations"},
    {(void*)&doSwiGetNetworkProviderID,  "t35", "SwiGetNetworkProviderID"},
    {(void*)&doSwiNvStore,               "t36", "SwiNvStore"},
    {(void*)&doSwiGetFirmwareBuildDate,  "t37", "SwiGetFirmwareBuildDate"},
    {(void*)&doSwiGetHardwareVersion,    "t38", "SwiGetHardwareVersion"},
    {(void*)&doSwiGetBootVersion,        "t39", "SwiGetBootVersion"},
    {(void*)&doSwiGetBootloaderBuildDate,"t40", "SwiGetBootloaderBuildDate"},
    {(void*)&doSwiGetRadioPower,         "t41", "SwiGetRadioPower"},
    {(void*)&doSwiSetRadioPower,         "t42", "SwiSetRadioPower"},
    {(void*)&doSwiGetRadioTempState,     "t43", "SwiGetRadioTempState"},
    {(void*)&doSwiGetRadioBattery,       "t44", "SwiGetRadioBattery"},
    {(void*)&doSwiGetCurrentRadioBand,   "t45", "SwiGetCurrentRadioBand"},
    {(void*)&doSwiGetRadioBandCfg,       "t46", "SwiGetRadioBandCfg"},
    {(void*)&doSwiSetRadioBandCfg,       "t47", "SwiSetRadioBandCfg"},
    {(void*)&doSwiGetServiceStatusEx,    "t48", "SwiGetServiceStatusEx"},
    {(void*)&doSwiGetPktSessionStatusEx, "t49", "SwiGetPktSessionStatusEx"},
    {(void*)&doDeprecated,               "t50", "Test Deprecated"},
    {(void*)&doSwiGsmStatus,             "t51", "SwiGetGsmSystemStatus"},
    {(void*)&doSwiGetBootloaderBuildDate,"t52", "SwiGetBootloaderBuildDate"},
    {(void*)&doSwiGetUsbdInfo,           "t53", "SwiGetUsbdInfo"},
    {(void*)&doSwiGetPriInfo,            "t54", "SwiGetPriInfo"},
    {(void*)&doSwiGetModemType,          "t55", "SwiGetModemType"},
    {(void*)&doSwiGetDeviceTime,         "t56", "SwiGetDeviceTime"},
    {(void*)&doSwiGetDeviceID,           "t57", "SwiGetDeviceID"},
    {(void*)&doSwiGetFlashImageInfo,     "t58", "SwiGetFlashImgInfo"},
    {(void*)&doSwiGetObjRngRev,          "t59", "SwiGetObjRngRev"},
    {(void*)&doSwiSetSimAuthentication,  "t60", "SwiSetSimAuthentication"},
    {(void*)&doSwiSetLbsPaKeepWarmStart, "t61", "SwiSetLbsPaKeepWarmStart"},
    {(void*)&doSwiGetLbsPaWarmStatus,    "t62", "SwiGetLbsPaWarmStatus"},
    {(void*)&doSwiSetLbsPaKeepWarmStop,  "t63", "SwiSetLbsPaKeepWarmStop"},
    {(void*)&doSwiGetLbsFixSettings,     "t64", "SwiGetLbsFixSettings"},
    {(void*)&doSwiSetLbsFixSettings,     "t65", "SwiSetLbsFixSettings"},
    {(void*)&doSwiSetLbsNiReq,           "t66", "SwiSetLbsNiReq"},
    {(void*)&doSwiFirmwareDownload,      "t67", "SwiFirmwareDownload"},
    {(void*)&doSwiGetGsmIMSI,            "t68", "SwiGetGsmIMSI"},
    {(void*)&doSwiSetPLMNMode,           "t69", "SwiSetPLMNMode"},
    {(void*)&doSwiGetPLMNMode,           "t70", "SwiGetPLMNMode"},
    {(void*)&doSwiStartPLMNSearch,       "t71", "SwiStartPLMNSearch"},
    {(void*)&doSwiGetPLMNSelection,      "t72", "SwiGetPLMNSelection"},
    {(void*)&doSwiGetUsbPortName,        "t73", "SwiGetUsbPortName"},
    {(void*)&doSwiSoftModemReset,        "t74", "SwiSoftModemReset"},
    {(void*)&doSwiResetToBootHold,       "t75", "SwiResetToBootHold"},
    {(void*)&doSwiGetIccId,              "t76", "SwiGetIccId"}, 
    {(void*)&doSwiStopAllNotif,          "t77", "SwiStopAllNotif"},
    {(void*)&doSwiGetPhonebookAvailable, "t78", "SwiGetPhonebookAvailable"},
    {(void*)&doSwiGetPhonebookSize,      "t79", "SwiGetPhonebookSize"},
    {(void*)&doSwiGetFdnMode,            "t80", "SwiGetFdnMode"},
    {(void*)&doSwiSetFdnModeEnable,      "t81", "SwiSetFdnMode-Enable"},    
    {(void*)&doSwiSetFdnModeDisable,     "t82", "SwiSetFdnMode-Disable"},
    {(void*)&doSwiGetEmergencyEntry,     "t83", "SwiGetEmergencyEntry"},
    {(void*)&doSwiAddPhonebookEntryExp,  "t84", "SwiAddPhonebookEntryExp"},
    {(void*)&doSwiGetPhonebookEntryExp,  "t85", "SwiGetPhonebookEntryExp"},
    {(void*)&doSwiDeletePhonebookEntry,  "t86", "SwiDeletePhonebookEntry"},
    {(void*)&doSwiDeletePhonebookLastNum,"t87", "SwiDeletePhonebookLastNum"},
    {(void*)&doSwiEditPhonebookEntryExp, "t88", "SwiEditPhonebookEntryExp"},
    {(void*)&doSwiGetSpeakerVolume,      "t89", "SwiGetSpeakerVolume"},
    {(void*)&doSwiSetSpeakerVolume,      "t90", "SwiSetSpeakerVolume"},
    {(void*)&doSwiGetMicMute,            "t91", "SwiGetMicMute"},
    {(void*)&doSwiSetMicMute,            "t92", "SwiSetMicMute"},
    {(void*)&doSwiGetSpeakerMute,        "t93", "SwiGetSpeakerMute"},
    {(void*)&doSwiSetSpeakerMute,        "t94", "SwiSetSpeakerMute"},
    {(void*)&doSwiGetNVToneStore,        "t95", "SwiGetNVToneStore"},
    {(void*)&doSwiSetNVToneStore,        "t96", "SwiSetNVToneStore"},
    {(void*)&doSwiGetAudioProfile,       "t97", "SwiGetAudioProfile"},
    {(void*)&doSwiSetAudioProfile,       "t98", "SwiSetAudioProfile"},
    {(void*)&doSwiGetCLIRSetting,        "t99", "SwiGetCLIRSetting"},
    {(void*)&doSwiSetCLIRSetting,        "t100", "SwiSetCLIRSetting"},
    {(void*)&doSwiGetVoiceMessage,       "t101", "SwiGetVoiceMessage"},
    {(void*)&doSwiAcknowledgeMissedCall, "t102", "SwiAcknowledgeMissedCall"},
    {(void*)&doSwiSetCallOriginate,      "t103", "SwiSetCallOriginate"},
    {(void*)&doSwiSetDTMFSend,           "t104", "SwiSetDTMFSend"},
    {(void*)&doSwiSetCallControlUMTS,    "t105", "SwiSetCallControlUMTS"},
    {(void*)&doSwiSetContDtmf,           "t106", "SwiSetContDtmf"},
    {(void*)&doSwiSetUserdefinedUserBusy,"t107", "SwiSetUserdefinedUserBusy"},
    {(void*)&doSwiSetSSRequest,          "t108", "SwiSetSSRequest"},
    {(void*)&doSwiAbortRequest,          "t109", "SwiAbortRequest"},
    {(void*)&doSwiSetSSRequestErase,     "t110", "SwiSetSSRequestErase"},
    {(void*)&doSwiSetSSRequestActivate,  "t111", "SwiSetSSRequestActivate"},
    {(void*)&doSwiSetSSRequestDeactivate, "t112", "SwiSetSSRequestDeactivate"},
    {(void*)&doSwiSetSSRequestInterrogate, "t113", "SwiSetSSRequestInterrogate"},
    {(void*)&doSwiSetSSRequestRegister,  "t114", "SwiSetSSRequestRegister"},
    {(void*)&doSwiSetSSPassword,         "t115", "SwiSetSSPassword"},
    {(void*)&doSwiGetSIMToolKitCommand,  "t116",  "SwiGetSIMToolKitCommand"},
    {(void*)&doSwiGetDeviceInformation,  "t117",  "SwiGetDeviceInformation"},
    {(void*)&doSwiGetRat,                "t118",  "SwiGetRat"},
    {(void*)&doSwiSetRat,                "t119",  "SwiSetRat"},
    {(void*)&doSwiGetPrlRegion,          "t120",  "SwiGetPrlRegion"},
    {(void*)&doSwiSetPrlRegion,          "t121",  "SwiSetPrlRegion"},
    {(void*)&doSwiGetSwocCfg,            "t122",  "SwiGetSwocCfg"}


};

#define TMAX (sizeof(ttabs)/sizeof(ttabs[0]))

/**************
 *
 * Name:     doDeprecated
 *
 * Purpose:  This function is used when tests need to be deprecated. It 
 *           informs the user via printf() that the test no longer exists
 *           and returns. Any test that needs to be obsoleted can use this
 *           function
 *           
 * Parms:    timeout - ignored
 *
 * Return:   none
 *
 * Notes:    None
 *
 **************/
package void doDeprecated( swi_uint32 timeout )
{
    printf("\nTest deprecated\n" );
}

/**************
 *
 * Name:     doSwiGetMEIDESN
 *
 * Purpose:  Perform the test that calls the SwiGetMEIDESN() API
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response
 *
 * Return:   none
 *
 * Notes:    The numbers for filling in the calling arguments were 
 *           taken from "memo017(CnS GeneralObjects).doc, for object
 *           ID 0x1078, CNS_PH_SS_PREF.
 *
 **************/
package void doSwiGetMEIDESN( swi_uint32 timeout )
{
    SWI_RCODE retcode;
    static struct SWI_STRUCT_MeidEsn meidesn; /* Local scope only */

    /* Invoke the API */
    retcode = SwiGetMEIDESN( &meidesn, timeout );

    /* Pull out the contents of the structure for display */
    if( retcode == SWI_RCODE_OK )
    {
        printf("\nModem returned preferences:\n" );
        printf("    Structure size:    %d\n", (int)meidesn.sizeStruct );
        printf("    Meid Enabled:      %d\n", (int)meidesn.nMeidEnabled );
        printf("    MeidPresent:       %d\n", (int)meidesn.nMeidPresent );
        printf("    Meid high quad:    %d\n", (int)meidesn.nMeidHigh );
        printf("    Meid low quad:     %d\n", (int)meidesn.nMeidLow );
        printf("    pEsn value:        %d\n", (int)meidesn.nPEsn );
        printf("    tESN Present:      %d\n", (int)meidesn.nTEsnPresent );
        printf("    ESN:               %d\n", (int)meidesn.nEsn );
    }
    else if ( retcode == SWI_RCODE_DEVICE_NOT_SUP )
    {
        printf("\nThis test is for CDMA modems only\n");            
    }
    else
    {
        printf("\nError setting temporary mode prefs: %d\n", retcode );
    }
}
/**************
 *
 * Name:     doSwiSetTempModePref
 *
 * Purpose:  Perform the test that calls the SwiSetTempModePref() API
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response
 *
 * Return:   none
 *
 * Notes:    The numbers for filling in the calling arguments were 
 *           taken from "memo017(CnS GeneralObjects).doc, for object
 *           ID 0x1070, CNS_PH_SS_PREF.
 *
 **************/
package void doSwiSetTempModePref( swi_uint32 timeout )
{
    SWI_RCODE retcode;

    /* Invoke the API */
    retcode = SwiSetTempModePref(
                  0x0009,   /* CDMA Only */
                  0x0001,   /* Until Power Cycle */
              0x00000060,   /* Mode duration, sec. */
              0x0000FFFF,   /* Band preference, any band */
                  0x0002,   /* Roam pref, only systems with 
                             * Roam Indicator On
                             */
                 timeout        );
              

    if( retcode == SWI_RCODE_OK )
    {
        printf("\nTemporary mode preferences set successfully\n" );
    }
    else if( retcode == SWI_RCODE_DEVICE_NOT_SUP)
    {
        printf("\nThis test is for CDMA modems only\n");
    }
    else
    {
        printf("\nError setting temporary mode prefs: %d\n", (int)retcode );
    }
}
/**************
 *
 * Name:     doSwiPstSetHomeSidNid
 *
 * Purpose:  Perform the test that calls the SwiPstSetHomeSidNid() API
 *           
 * Parms:    timeout - the timeout value to use when awaiting a modem response
 *
 * Return:   none1
 *
 * Notes:    none
 *
 **************/
package void doSwiPstSetHomeSidNid( swi_uint32 timeout )
{
    swi_char lasterror[80];
    SWI_RCODE retcode;

   /* Invoke the API for Setting the Sid Nid */
    retcode = SwiPstSetHomeSidNid( 100, testsidnid, timeout );

    if( retcode == SWI_RCODE_OK )
    {
        printf("\nSid/Nid set successfully\n" );
    }
    else if( retcode == SWI_RCODE_DEVICE_NOT_SUP)
    {
        printf("\nThis test is for CDMA modems only\n");
    }
    else
    {
        printf("\nError setting Sid/Nid table: %d\n", (int)retcode );

        /* In addition, print the error reported by the modem */
        if( retcode == SWI_RCODE_REQUEST_REJECTED )
        {
            SwiGetLastError( lasterror, sizeof(lasterror) );
            printf("  Mdm reports CnS Error: %s\n", lasterror );
        }
    }
}
/**************
 *
 * Name:     doSwiGetTrafficStatistics
 *
 * Purpose:  Runs the test to get the tx/rx byte counters from a GSM
 *           modem 
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiGetTrafficStatistics(unsigned int timeout)
{
    SWI_RCODE               retcode;
    swi_uint32              sentbytes;
    swi_uint32              receivedbytes;

    /* Get the byte coutners */
    retcode = SwiGetTrafficStatistics( &sentbytes, &receivedbytes, timeout );
   
    printf("\n");
 
    if( retcode == SWI_RCODE_OK )
    {
        printf("Byte Counters:\n");
        printf("  Modem tx bytes: %d\n",  (int)sentbytes); 
        printf("  Modem rx bytes: %d\n", (int)receivedbytes);
    }
    else
        printf("SwiGetTrafficStatistics returned error: %d\n", (int)retcode );
}

/**************
 *
 * Name:     doSwiResetTrafficStatistics
 *
 * Purpose:  Exercises the SwiResetTrafficStatistics API - calls the API and prints
 *           information related to the API
 *           
 * Parms:    timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    Simply prints data if successful or error message on failure
 *
 **************/
package void doSwiResetTrafficStatistics(unsigned int timeout)
{
    SWI_RCODE retcode;
 
    /* Fill up our data  */

    retcode = SwiResetTrafficStatistics( timeout );

    printf ("\n");

    if( retcode == SWI_RCODE_OK )
    {
        printf ("Byte Counters cleared\n");
    }
    else
        printf ("Byte Counters failed to clear\n");
}

/*************
 * Functions below here are control routines for calling the functions above.
 ************/
/**************
 *
 * Name:     apDisplayTestXref
 *
 * Purpose:  Displays the list of available test functions and the 
 *           corresponding tag name 
 *           
 * Parms:    none
 * 
 * Return:   none
 *
 * Notes:    none
 *
 **************/
package void apDisplayTestXref()
{
    unsigned int i;
    printf("Test id API name\n");
    printf("======= ========\n");    
    for (i = 0; i < TMAX; i++)
    {
        printf ("%4s\t%s \n", ttabs[i].estrp, ttabs[i].fstrp);
    }
}

/**************
 *
 * Name:     apRunSelectedTest
 *
 * Purpose:  Runs a selected test, given the test tag
 *           
 * Parms:    testtagp  - pointer to test tag
 *           timeout   - timeout value for stop and wait functions as 
 *                       passed in on the command line or defaulted.
 * Return:   none
 *
 * Notes:    
 *
 **************/
package void apRunSelectedTest(char * testtagp, unsigned int timeout)
{
    unsigned int i;
    int slen, tslen;
    
    /* Loop through our table and call the API function that matches */
    /* the test tag                                                  */
    for (i = 0; i < TMAX; i++)
    {
        tslen = slstrlen (testtagp);
        slen = slstrncmp(ttabs[i].estrp, testtagp, tslen );
        if (slen == 0)
        {
            /* Invoke the selected API function */
            printf ("Invoking test: %s, %s\n", testtagp, ttabs[i].fstrp);
            (*(ttabs[i].testroutinep))(timeout);
            break;
        }
    } 
}

/**************
 *
 * Name:     apProcessTestList
 *
 * Purpose:  Runs a selected test
 *           
 * Parms:    testsp  - test or list of to execute
 *           timeout - timeout for calls to API as passed in or defaulted
 *
 * Return:   none
 *
 * Abort:    none
 *
 * Notes:    testsp example: t1 or t1,t3,t6 or x or X
 *
 **************/
package void apProcessTestList(char * testsp, unsigned int timeout)
{
    char * ltestsp = testsp;
    struct slstrtokblk tokblk;
    char *tokresultp;
    char delimiter[2];
    
    delimiter[0] = ',';
    delimiter[1] = '\0';
    
    /* Get the first item in the list */
    tokresultp = slstrtok (ltestsp, &delimiter[0], &tokblk);
   
    /* Show X-ref list if asked */
    if (tokresultp != NULL && sltoupper(*tokresultp) == 'X')
    {
        apDisplayTestXref ();
        return;
    }
    
    /* Proceed to running the sample API calls */
    /* Loop until no more */
    while (tokresultp != NULL)
    {
        apRunSelectedTest(tokresultp, timeout);
        
        /* get the next available test */
        tokresultp = slstrtok (NULL, &delimiter[0], &tokblk);
    }
}

/**************
 *
 * Name:     apProcessTestFile
 *
 * Purpose:  Reads test tags from a file and run them 
 *           
 * Parms:    testfilenamep - file containing test or list of to execute
 *           timeout       - timeout for calls to API as passed in or defaulted
 *
 * Return:   none
 *
 * Abort:    exits if file cannot be opened
 * 
 * Notes:    Format same as test list 
 *
 **************/
package void apProcessTestFile(char * testfilenamep, unsigned int timeout)
{
    int i;
    char buf[128];
    FILE * fp;
    char * readp;
      
    /* Open file first */
    fp = fopen (testfilenamep, "r");
    
    /* Make sure file open is OK */
    if (fp == NULL)
    {
        printf ("Error: opening %s for reading\n", testfilenamep);
        exit(1);
    }
    
    /* Proceed to running the sample API calls */
    /* Loop until no more */
    slmemset (buf, 0, sizeof (buf));
    while ( (readp = fgets (buf, sizeof(buf), fp)) != NULL)
    {
        /* hack - slstrtok doesn't like '\n' in the line read from the */
        /* file, it looks for '\0'                                     */
        for (i=0; i< strlen (buf); i++)
        {
            if (buf[i] == '\n')
                buf[i] = '\0';
        }
        
        /* call the test number/test list function - does the same thing */
        apProcessTestList(buf, timeout);
        slmemset (buf, 0, sizeof (buf));
    } 
}

/**************
 *
 * Name:     apProcessTests
 *
 * Purpose:  Calls the Test list processor depending on what is available,
 *           test list or filename.
 *           
 * Parms:    testsp           - test or list of to execute
 *           testsfilenamep   - filename of test list
 * *
 * Return:   none
 *
 * Notes:    By the time this function is called, only one of the first 
 *           two parameters should be active, since they cannot co-exist
 *
 **************/
package void apProcessTests(char * testsp, char * testsfilenamep, unsigned int timeout)
{
    /* Do test tags as given from the command line */
    if (testsp != NULL)
    {
        apProcessTestList(testsp, timeout);
    }
    
    /* Filename supplied */
    if (testsfilenamep != NULL)
    {
         apProcessTestFile(testsfilenamep, timeout); 
    }
}


/*
 * $Log: aptestroutines.c,v $
 */
