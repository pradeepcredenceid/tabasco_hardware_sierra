/**************
 *  Filename: qaGobiApiLocTest.c
 *
 *  Purpose:  LOC API test driver function definitions
 *
 * Copyright: © 2015 Sierra Wireless, Inc., all rights reserved
 *
 **************/

/* Linux definitions */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* include files */
#include "SwiDataTypes.h"
#include "qmerrno.h"
#include "qmudefs.h"

#include "qaGobiApiLoc.h"
#include "qaGobiApiCbk.h"
#include "qaGobiApiLocTest.h"
#include "qatesthelper.h"
#include "qatestproto.h"

/*******************
    TEST DATA
 *******************/
/* Constants */

/* Global Variables */
ULONGLONG     qmiLocEventMaskPositionReport           = 0x00000001;
ULONGLONG     qmiLocEventMaskGnssSvInfo               = 0x00000002;
ULONGLONG     qmiLocEventMaskNmea                     = 0x00000004;
ULONGLONG     qmiLocEventMaskNiNotifyVerifyReq        = 0x00000008;
ULONGLONG     qmiLocEventMaskInjectTimeReq            = 0x00000010;
ULONGLONG     qmiLocEventMaskInjectPredictedOrbitsReq = 0x00000020;
ULONGLONG     qmiLocEventMaskInjectPositionReq        = 0x00000040;
ULONGLONG     qmiLocEventMaskEngineState              = 0x00000080;
ULONGLONG     qmiLocEventMaskFixSessionState          = 0x00000100;
ULONGLONG     qmiLocEventMaskWifiReq                  = 0x00000200;
ULONGLONG     qmiLocEventMaskSensorStreamingReadyStatus = 0x00000400;
ULONGLONG     qmiLocEventMaskTimeSyncReq              = 0x00000800;
ULONGLONG     qmiLocEventMaskSetSpiStreamingReport    = 0x00001000;
ULONGLONG     qmiLocEventMaskLocationServerConnectionReq = 0x00002000;
ULONGLONG     qmiLocEventMaskNiGeofenceNotification   = 0x00004000;
ULONGLONG     qmiLocEventMaskGeofenceGenAlert         = 0x00008000;
ULONGLONG     qmiLocEventMaskGeofenceBreachNotification = 0x00010000;
ULONGLONG     qmiLocEventMaskPedometerControl         = 0x00020000;
ULONGLONG     qmiLocEventMaskMotionDataControl        = 0x00040000;
ULONGLONG     qmiLocEventMaskBatchFullNotification    = 0x00080000;
ULONGLONG     qmiLocEventMaskLiveBatchedPositionReport = 0x00100000;
ULONGLONG     qmiLocEventMaskInjectWifiApDataReq       = 0x00200000;
ULONGLONG     qmiLocEventMaskGeofenceBatchBreachNotification = 0x00400000;
ULONGLONG     qmiLocEventMaskVehicleDataReadyStatus    = 0x00800000;
ULONGLONG     qmiLocEventMaskGnssMeasurementReport     = 0x01000000;
ULONGLONG     qmiLocEventMaskInvalidValue              = 0xFFFFFFFF;

local ULONG   qmiLocRecurrence        = 1;
local ULONG   qmiLocAccuracy          = 1;
local ULONG   intermediateReportState = 1;
local ULONG   configAltitudeAssumed   = 1;

local CHAR appProvider[] = "ABC";
local CHAR appName[]     = "XYZ";
local CHAR appVer[]      = "12";

local ULONGLONG utcTime = 0;
local ULONG uncTime = 10;

local struct LocApplicationInfo    AppInfo = {3, appProvider,3,appName,1,2,appVer };

local double latitude  = 30.7274072;
local double longitude = 76.8436267;
local float  horUnc    = 3.5;
local float  altitudeWrtEllipsoid = 16.0;
local float  vertUnc = 3.5;

/*
 * Name:    doSLQSLOCEventRegister
 *
 * Purpose: Perform the tests that call the SLQSLOCEventRegister() API
 *
 * Return:  none
 *
 * Notes:   none
 *
 */
void doSLQSLOCEventRegister( void )
{
    /* Test Cases */
    SLQSLocEventRegisterTestCase_t SLQSLocEventRegisterTestCase[]=
    {
         { eQCWWAN_ERR_NONE, 1, qmiLocEventMaskPositionReport, "Mask Position Report"},
         { eQCWWAN_ERR_NONE, 2, qmiLocEventMaskGnssSvInfo, "Mask GNSS SV Info"},
         { eQCWWAN_ERR_NONE, 3, qmiLocEventMaskNmea, "Mask NMEA"},
         { eQCWWAN_ERR_NONE, 4, qmiLocEventMaskEngineState, "Mask Engine State"},
         { eQCWWAN_ERR_NONE, 5, qmiLocEventMaskTimeSyncReq, "Mask Time Sync Req"},
         { eQCWWAN_ERR_NONE, 6, qmiLocEventMaskInvalidValue, "InValid Case"},
    };

    LOCEventRegisterReqResp req;
    FILE *fp = tfopen("../../loc/test/results/slqsloceventregister.txt", "w");
    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSLocEventRegisterTestCase )/
                      sizeof( SLQSLocEventRegisterTestCase[0] );
        while ( tc < ncases )
        {
            SLQSLocEventRegisterTestCase_t *pd =
                   &SLQSLocEventRegisterTestCase[tc++];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tc,
                        ncases,
                        pd->desc);

            req.eventRegister = pd->eventRegister;
            fprintf(fp, "Event mask = %#010llx\n",req.eventRegister);
            ULONG nRet = SLQSLOCEventRegister( &req );
            doprintreason( fp, nRet );

            if ( eQCWWAN_ERR_NONE == nRet )
            {
                fprintf(fp, "SLQSLOCEventRegister Successful\n");
                fprintf(fp, "Event mask = %#010llx\n",req.eventRegister);
            }
        }
    }
    if (fp)
        tfclose(fp);
}

/*
 * Name:    doSLQSLOCSetExtPowerState
 *
 * Purpose: Perform the tests that call the SLQSLOCSetExtPowerState() API
 *
 * Return:  none
 *
 * Notes:   none
 *
 */
void doSLQSLOCSetExtPowerState(void)
{

    /* Test Cases */
    SLQSLocSetExtPowerTestCase_t SLQSLocSetExtPowerTestCase[]=
    {
         { eQCWWAN_ERR_NONE, 1, 0, "Device is not connected to an external power source"},
         { eQCWWAN_ERR_NONE, 2, 1, "Device is connected to an external power source"},
         { eQCWWAN_ERR_NONE, 3, 2, "Unknown external power state"},
         { eQCWWAN_ERR_NONE, 4, 3, "Failue Case"},
    };
LOCExtPowerStateReqResp req;
FILE *fp = tfopen("../../loc/test/results/slqslocsetextpower.txt", "w");
if (fp)
{
    BYTE tc = 0;
    BYTE ncases = sizeof( SLQSLocSetExtPowerTestCase)/
                  sizeof( SLQSLocSetExtPowerTestCase[0] );
    while ( tc < ncases )
    {
        SLQSLocSetExtPowerTestCase_t *pd =
               &SLQSLocSetExtPowerTestCase[tc++];
        fprintf(fp, "\nTest Case %d / %d : %s\n",
                    tc,
                    ncases,
                    pd->desc);

        req.extPowerState = pd->extPowerState;
        fprintf(fp, "Event mask = %ld\n",req.extPowerState);
        ULONG nRet = SLQSLOCSetExtPowerState( &req );
        doprintreason( fp, nRet );

        if ( eQCWWAN_ERR_NONE == nRet )
        {
            fprintf(fp, "SLQSLOCExtPowerState Configured Successful\n");
            fprintf(fp, "External Power State = %ld\n",req.extPowerState);
        }
    }
}
if (fp)
    tfclose(fp);
}
/*
 * Name:    doSLQSLOCStart
 *
 * Purpose: Perform the tests that call the SLQSLOCStart() API
 *
 * Return:  none
 *
 * Notes:   none
 *
 */

void doSLQSLOCStart(void)
{
    ULONG               nRet;
    FILE                *fp = NULL;
    LOCStartReq req;
    /* Define the test cases to be executed */
    SLQSLocStartTestCase_t SLQSLocStartTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, 1, NULL, NULL, NULL, NULL, NULL, NULL,
           "Valid Test Case - All Optional params NULL"},

        { eQCWWAN_ERR_NONE, 2, 1, &qmiLocRecurrence, NULL, NULL, NULL, NULL, NULL,
           "Valid Test Case - LOC Recurrence Perodic with 0 session ID"},

        { eQCWWAN_ERR_NONE, 3, 1, NULL, &qmiLocAccuracy, NULL, NULL, NULL, NULL,
           "Valid Test Case - Low Accuracy"},

        { eQCWWAN_ERR_NONE, 4, 1, NULL, NULL, &intermediateReportState, NULL, NULL, NULL,
           "Valid Test Case - Intermediate Report Turned On"},

        { eQCWWAN_ERR_NONE, 5, 1, NULL, NULL, NULL, NULL, &AppInfo, NULL,
           "Valid Test Case - Set App Info"},

        { eQCWWAN_ERR_NONE, 6, 1, NULL, NULL, NULL, NULL, NULL, &configAltitudeAssumed,
           "Valid Test Case - Altitude Assumed in GNSS SV INFO enabled"},
     };

    fp = tfopen( "../../loc/test/results/slqslocstart.txt", "w" );
    if ( fp )
    {
        BYTE tCaseNum     = 0;
        BYTE ncases = sizeof( SLQSLocStartTestCases ) /
                      sizeof( SLQSLocStartTestCases[0] );

        while( tCaseNum < ncases )
        {
        SLQSLocStartTestCase_t *pd =
                &SLQSLocStartTestCases[tCaseNum++];

        fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tCaseNum,
                        ncases,
                        pd->desc);
        req.SessionId                 =  pd->sessionId;
        req.pRecurrenceType           =  pd->pRecurrenceType;
        req.pHorizontalAccuracyLvl    =  pd->pHorizontalAccuracyLvl;
        req.pIntermediateReportState  =  pd->pIntermediateReportState;
        req.pMinIntervalTime          =  pd->pMinIntervalTime;
        req.pApplicationInfo          =  pd->pApplicationInfo;
        req.pConfigAltitudeAssumed    =  pd->pConfigAltitudeAssumed;

        fprintf( fp,"\nParameters set for System Selection Preference:\n" );
        fprintf(fp, "Session Id = %d\n",req.SessionId);
        IFPRINTF( fp, "Recurrence Type: %ld\n", pd->pRecurrenceType );
        IFPRINTF( fp, "Horizontal Accuracy Lvl: %ld\n", pd->pHorizontalAccuracyLvl );
        IFPRINTF( fp, "Intermediate Report State: %ld\n", pd->pIntermediateReportState);
        IFPRINTF( fp, "Min Interval Time : %ld\n", pd->pMinIntervalTime );
        if( pd->pApplicationInfo)
        {
            fprintf( fp, "\nLOC Application Info :\n");
            BYTE i = 0;
            fprintf( fp, "App Provider Length : 0x%x\n",
                      pd->pApplicationInfo->appProviderLength);

            fprintf( fp, "APP Provider:");
            for ( i = 0; i < pd->pApplicationInfo->appProviderLength; i++ )
            {
                fprintf( fp, "%c", pd->pApplicationInfo->pAppProvider[i] );
            }
            fprintf( fp, "\nApp Provider Length : 0x%x\n",
                      pd->pApplicationInfo->appNameLength);

            fprintf( fp, "APP Name:");
            for ( i = 0; i < pd->pApplicationInfo->appNameLength; i++ )
            {
               fprintf( fp, "%c", pd->pApplicationInfo->pAppName[i] );
            }

            fprintf( fp, "\nApp Version Valid : 0x%x\n",
                      pd->pApplicationInfo->appVersionValid);

            fprintf( fp, "App Version Length : 0x%x\n",
                      pd->pApplicationInfo->appVersionLength);

            fprintf( fp, "APP Version:");
            for ( i = 0; i < pd->pApplicationInfo->appVersionLength; i++ )
            {
               fprintf( fp, "%c", pd->pApplicationInfo->pAppVersion[i] );
            }
            fprintf( fp, "\n");
        }

        //        IFPRINTF( fp, "Application Info : %ld\n", pd->pApplicationInfo );
        IFPRINTF( fp, "Altitude Assumed : %ld\n", pd->pConfigAltitudeAssumed );

        nRet = SLQSLOCStart(&req );

        /* Display result code and text */
          doprintreason( fp, nRet );
         }
     }

      if (fp)
      tfclose(fp);
}
/*
 * Name:    doSLQSLOCStop
 *
 * Purpose: Perform the tests that call the SLQSLOCStop() API
 *
 * Return:  none
 *
 * Notes:   none
 *
 */

void doSLQSLOCStop(void)
{

    /* Test Cases */
    SLQSLocStopTestCase_t SLQSLocStopTestCases[]=
    {
         { eQCWWAN_ERR_NONE, 1, 0, "Session Id 0"},
         { eQCWWAN_ERR_NONE, 2, 1, "session Id 1"},
     };
    LOCStopReq req;

    FILE *fp = tfopen("../../loc/test/results/slqslocstop.txt", "w");
    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSLocStopTestCases)/
                  sizeof( SLQSLocStopTestCases[0] );
         while ( tc < ncases )
        {
            SLQSLocStopTestCase_t *pd =
                   &SLQSLocStopTestCases[tc++];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tc,
                        ncases,
                        pd->desc);

            req.sessionId = pd->sessionId;
            fprintf(fp, "Session Id = %d\n",req.sessionId);
            ULONG nRet = SLQSLOCStop( &req );
            doprintreason( fp, nRet );

            if ( eQCWWAN_ERR_NONE == nRet )
            {
                fprintf(fp, "SLQSLOCStop  Successful\n");
                fprintf(fp, "LOC Stop Session ID = %d\n",req.sessionId);
            }
        }
    }
    if (fp)
        tfclose(fp);
}


void doSLQSLOCSetOpMode(void)
{
    ULONG               nRet;
    FILE                *fp = NULL;
    /* Define the test cases to be executed */
    SLQSLocSetOpModeTestCase_t SLQSLocSetOpModeTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, "set default operation mode (1)"}
    };

    fp = tfopen( "../../loc/test/results/slqslocsetopmode.txt", "w" );
    if ( fp )
    {
        BYTE tCaseNum     = 0;
        BYTE ncases = sizeof( SLQSLocSetOpModeTestCases ) /
            sizeof( SLQSLocSetOpModeTestCases[0] );

        while( tCaseNum < ncases )
        {
            SLQSLocSetOpModeTestCase_t *pd =
                &SLQSLocSetOpModeTestCases[tCaseNum++];

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                    tCaseNum,
                    ncases,
                    pd->desc);

            fprintf( fp, "Operating mode : %ld\n", pd->mode );

            nRet = SLQSLOCSetOpMode(pd->mode );

            /* Display result code and text */
            doprintreason( fp, nRet );
        }
    }

    if (fp)
        tfclose(fp);
}

void doSLQSLOCDelAssData(void)
{
    ULONG nRet;
    FILE  *fp = NULL;
    GnssData gnss;
    SVInfo svinfo;
    CellDb celldb;
    ClkInfo clk;
    BdsSVInfo bds;
    
    /* SV Info */
    SV sv[1] = {{1,1,1}};
    svinfo.len = 1;
    svinfo.pSV = sv;

    /* GNSS data */
    gnss.mask = 1;

    /* Cell data base */
    celldb.mask = 1;

    /* Clock Info */
    clk.mask = 1;

    /* BDS SV Info */
    BdsSV bsv[1] = {{201,1}};
    bds.len = 1;
    bds.pSV = bsv;

    /* Define the test cases to be executed */
    SLQSLocDelAssDataTestCase_t SLQSLocDelAssDataTestCase[] =
    {
        { eQCWWAN_ERR_NONE, 1, NULL, NULL, NULL, NULL, NULL,
            "Delete All"},
        { eQCWWAN_ERR_NONE, 2, &svinfo, NULL, NULL, NULL, NULL,
            "Delete SV Info"},
        { eQCWWAN_ERR_NONE, 3, NULL, &gnss, NULL, NULL, NULL,
            "Delete GNSS Data"},
        { eQCWWAN_ERR_NONE, 4, NULL, NULL, &celldb, NULL, NULL,
            "Delete Cell Database"},
        { eQCWWAN_ERR_NONE, 5, NULL, NULL, NULL, &clk, NULL,
            "Delete Clock Info"},
        { eQCWWAN_ERR_NONE, 6, NULL, NULL, NULL, NULL, &bds,
            "Delete BDS SV Info"},
    };

    fp = tfopen( "../../loc/test/results/slqslocsetopmode.txt", "w" );
    if ( fp )
    {
        BYTE tCaseNum     = 0;
        BYTE ncases = sizeof( SLQSLocDelAssDataTestCase ) /
            sizeof( SLQSLocDelAssDataTestCase[0] );

        while( tCaseNum < ncases )
        {
            SLQSLocDelAssDataTestCase_t *pd =
                &SLQSLocDelAssDataTestCase[tCaseNum++];

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                    tCaseNum,
                    ncases,
                    pd->desc);

            LocDelAssDataReq req;
            memset(&req, 0, sizeof(req));

            req.pSVInfo = pd->pSVInfo;
            req.pGnssData = pd->pGnssData;
            req.pCellDb = pd->pCellDb;
            req.pClkInfo = pd->pClkInfo;
            req.pBdsSVInfo = pd->pBdsSVInfo;

            nRet = SLQSLOCDelAssData(req);

            /* Display result code and text */
            doprintreason( fp, nRet );
        }
    }

    if (fp)
        tfclose(fp);
}

void GetCurrentUTCTime(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    utcTime =
    (ULONGLONG)(tv.tv_sec) * 1000 +
    (ULONGLONG)(tv.tv_usec) / 1000;

    fprintf(stderr,"\n UTC Time = %llu, Uncertainty Time = %lu\n", utcTime, uncTime );
    return;
}    

/*************
 *
 * Name:    cbkTestSLQSSetLocInjectUTCTimeCallback
 *
 * Purpose: SLQSSetLocInjectUTCTimeCallback API callback function
 *
 **************/
local void cbkTestSLQSSetLocInjectUTCTimeCallback(
    QmiCbkLocInjectUTCTimeInd *pInjectUTCTimeNotification  )
{
    FILE *fp;

    fp = tfopen("TestReport/SLQSSetLocInjectUTCTimeCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetLocInjectUTCTimeCallback");
    else
    {
        fprintf( fp, "CB : Inject UTC Time \n" );
        fprintf( fp, "Status %lu \n",
                 pInjectUTCTimeNotification->status );

        fprintf( fp, "\n\n" );
    }
    tfclose(fp);
}

void doSLQSLOCInjectUTCTime(void)
{
    ULONG               nRet;
    FILE                *fp = NULL;
    /* Define the test cases to be executed */
    SLQSLocInjectUTCTimeTestCase_t SLQSLocInjectUTCTimeTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, "Inject UTC time"}
    };

    fp = tfopen( "../../loc/test/results/slqslocinjectUTCtime.txt", "w" );
    if ( fp )
    {
        ULONG rc;
        BYTE tCaseNum     = 0;
        BYTE ncases = sizeof( SLQSLocInjectUTCTimeTestCases ) /
            sizeof( SLQSLocInjectUTCTimeTestCases[0] );

        rc = SLQSSetLocInjectUTCTimeCallback( &cbkTestSLQSSetLocInjectUTCTimeCallback );
        if ( rc != eQCWWAN_ERR_NONE )
            fprintf( stderr, "SLQSSetLocInjectUTCTimeCallback: Failed : %lu\r\n", rc );
        else
            printf( "SLQSSetLocInjectUTCTimeCallback: Enabled callback\r\n" );            
            
        while( tCaseNum < ncases )
        {
            SLQSLocInjectUTCTimeTestCase_t *pd =
                &SLQSLocInjectUTCTimeTestCases[tCaseNum++];

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                    tCaseNum,
                    ncases,
                    pd->desc);

            GetCurrentUTCTime();
            nRet = SLQSLOCInjectUTCTime(utcTime, uncTime);

            /* Display result code and text */
            doprintreason( fp, nRet );
        }
        sleep(1);
        rc = SLQSSetLocInjectUTCTimeCallback( NULL );
        if ( rc != eQCWWAN_ERR_NONE )
            fprintf( stderr, "SLQSSetLocInjectUTCTimeCallback: Failed : %lu\r\n", rc );
        else
            printf( "SLQSSetLocInjectUTCTimeCallback: Disabled\r\n" );    
        
    }

    if (fp)
        tfclose(fp);
}

/*************
 *
 * Name:    cbkTestSLQSSetLocGetServerCallback
 *
 * Purpose: SLQSSetLocGetServerCallback API callback function
 *
 **************/
local void cbkTestSLQSSetLocGetServerCallback(
    QmiCbkLocGetServerInd *pGetServerNotification  )
{
    FILE *fp;
    ULONG ipv4, ipv6;
    WORD  iC;
    
    fp = tfopen("TestReport/SLQSSetLocInjectUTCTimeCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetLocInjectUTCTimeCallback");
    else
    {
        fprintf( fp, "CB : GetServer \n" );
        fprintf( fp, "Server Status %lu \n",
                 pGetServerNotification->status );
                 
        fprintf( fp, "Type of Server %lu \n",
                 pGetServerNotification->serverType );

        if(pGetServerNotification->pAddressIPV4)
        {
            ipv4 = pGetServerNotification->pAddressIPV4->ipv4Addr;
            fprintf( fp, "IPv4 Address    : %d.%d.%d.%d:%d\n\n",
                         (unsigned int)(ipv4 >> 24) & 0xFF,
                         (unsigned int)(ipv4 >> 16) & 0xFF,
                         (unsigned int)(ipv4 >> 8) & 0xFF,
                         (unsigned int)ipv4 & 0xFF,
                         pGetServerNotification->pAddressIPV4->port);                 
        }
        
        if(pGetServerNotification->pAddressIPV6)
        {
            fprintf( fp, "IPv6 Address    : ");
            for ( iC = 0 ; iC < 8 ; iC++ )
            {
                fprintf( fp,"%x:",pGetServerNotification->pAddressIPV6->ipv6Addr[iC]);
            }
            fprintf( fp,"\n");
            fprintf( fp, "IPv6 Port: %lu\n",pGetServerNotification->pAddressIPV6->port );
            fprintf( fp, "\n\n" );
        }
        
        if(pGetServerNotification->pAddressURL)
        {
            WORD  index = 0;
            UrlAddress *lresp = pGetServerNotification->pAddressURL;
            fprintf( fp, "URL Address :" );
            {
                fprintf( fp, "%s ", lresp->url );
            }
            fprintf( fp, "\n" );
        }    
    }
    tfclose(fp);
}
void doSLQSLOCGetServer(void)
{
    ULONG               nRet;
    FILE                *fp = NULL;
    /* Define the test cases to be executed */
    SLQSLOCGetServerTestCase_t SLQSLOCGetServerTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, 3, NULL, "UTMS server: All"},
        { eQCWWAN_ERR_NONE, 2, 1, NULL, "CDMA server: All"}        
    };

    fp = tfopen( "../../loc/test/results/slqslocgetserver.txt", "w" );
    if ( fp )
    {
        ULONG rc;
        BYTE tCaseNum     = 0;
        LOCGetServerReq req;
        memset(&req, 0, sizeof(req));
        
        BYTE ncases = sizeof(SLQSLOCGetServerTestCases) /
            sizeof( SLQSLOCGetServerTestCases[0] );

        rc = SLQSSetLocGetServerCallback( &cbkTestSLQSSetLocGetServerCallback );
        if ( rc != eQCWWAN_ERR_NONE )
            fprintf( stderr, "SLQSSetLocGetServerCallback: Failed : %lu\r\n", rc );
        else
            printf( "SLQSSetLocGetServerCallback: Enabled callback\r\n" );            
            
        while( tCaseNum < ncases )
        {
            SLQSLOCGetServerTestCase_t *pd =
                &SLQSLOCGetServerTestCases[tCaseNum++];

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                    tCaseNum,
                    ncases,
                    pd->desc);
            
            req.serverType = pd->serverType;
            req.pAddressType = NULL;
            
            nRet = SLQSLOCGetServer(req);
            
            /* Display result code and text */
            doprintreason( fp, nRet );
            sleep(1);
        }
        sleep(1);
        rc = SLQSSetLocGetServerCallback( NULL );
        if ( rc != eQCWWAN_ERR_NONE )
            fprintf( stderr, "SLQSSetLocGetServerCallback: Failed : %lu\r\n", rc );
        else
            printf( "SLQSSetLocGetServerCallback: Disabled\r\n" );    
        
    }

    if (fp)
        tfclose(fp);
}

/*************
 *
 * Name:    cbkTestSLQSSetLocSetServerCallback
 *
 * Purpose: SLQSSetLocSetServerCallback API callback function
 *
 **************/
local void cbkTestSLQSSetLocSetServerCallback(
    QmiCbkLocSetServerInd *pSetServerNotification  )
{
    FILE *fp;

    fp = tfopen("TestReport/SLQSSetLocSetServerCallback.txt", "a");

    if ( fp == NULL )
        perror("cbkTestSLQSSetLocSetServerCallback");
    else
    {
        fprintf( fp, "CB: Set Server\n" );
        fprintf( fp, "Server Status %lu \n",
                 pSetServerNotification->status );

        fprintf( fp, "\n\n" );
    }
    tfclose(fp);
}

void doSLQSLOCSetServer(void)
{
    ULONG               nRet;
    FILE                *fp = NULL;
    LOCSetServerReq     req;
    
    IPv4Address     ipv4;
    IPv6Address     ipv6;
    UrlAddress      url;
    
    ipv4.ipv4Addr = 0x4B7DCBC0;
    ipv4.port = 0x1C6C;
    ipv6.ipv6Addr[0] = 0x0001;
    ipv6.ipv6Addr[1] = 0x0002;
    ipv6.ipv6Addr[2] = 0x0003;
    ipv6.ipv6Addr[3] = 0x0004;
    ipv6.ipv6Addr[4] = 0x0005;
    ipv6.ipv6Addr[5] = 0x0006;
    ipv6.ipv6Addr[6] = 0x0007;
    ipv6.ipv6Addr[7] = 0x0008;
    ipv6.port = 0x00001389;
    
    strcpy(url.url, "supl.google.com");
    /* Define the test cases to be executed */
    SLQSLOCSetServerTestCase_t SLQSLocSetServerTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, 1, &ipv4, NULL, NULL, "LOC Set CDMA Server ipv4"},
        { eQCWWAN_ERR_NONE, 1, 1, NULL, &ipv6, NULL, "LOC Set CDMA Server ipv6"},
        { eQCWWAN_ERR_NONE, 3, 3, NULL, NULL, &url, "LOC Set UTMS Server url"}
    };

    memset(&req, 0, sizeof(req));
    fp = tfopen( "../../loc/test/results/slqslocsetserver.txt", "w" );
    if ( fp )
    {
        ULONG rc;
        BYTE tCaseNum     = 0;

        
        BYTE ncases = sizeof( SLQSLocSetServerTestCases ) /
            sizeof( SLQSLocSetServerTestCases[0] );

        rc = SLQSSetLocSetServerCallback( &cbkTestSLQSSetLocSetServerCallback );
        if ( rc != eQCWWAN_ERR_NONE )
            fprintf( stderr, "SLQSSetLocSetServerCallback: Failed : %lu\r\n", rc );
        else
            printf( "SLQSSetLocSetServerCallback: Enabled callback\r\n" );            
            
        while( tCaseNum < ncases )
        {
            SLQSLOCSetServerTestCase_t *pd =
                &SLQSLocSetServerTestCases[tCaseNum++];

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                    tCaseNum,
                    ncases,
                    pd->desc);
            
            req.serverType=pd->serverType;
            req.pIPv4Address = pd->pIPv4;
            req.pIPv6Address = pd->pIPv6;
            req.pURLAddress = pd->pURL;
            
            nRet = SLQSLOCSetServer(req);

            /* Display result code and text */
            doprintreason( fp, nRet );
            sleep(1);
        }
        sleep(1);
        rc = SLQSSetLocSetServerCallback( NULL );
        if ( rc != eQCWWAN_ERR_NONE )
            fprintf( stderr, "SLQSSetLocSetServerCallback: Failed : %lu\r\n", rc );
        else
            printf( "SLQSSetLocSetServerCallback: Disabled\r\n" );    
        
    }

    if (fp)
        tfclose(fp);
}


/*************
 *
 * Name:    cbkTestSLQSSetLocGetOpModeCallback
 *
 * Purpose: SLQSSetLocGetOpModeCallback API callback function
 *
 **************/
local void cbkTestSLQSSetLocGetOpModeCallback(
    QmiCbkLocGetOpModeInd *pGetOpModeNotification  )
{
    FILE *fp;
    
    fp = tfopen("TestReport/SLQSSetLocGetOpModeCallback.txt", "a");

    if ( fp == NULL )
        perror("SLQSSetLocGetOpModeCallback");
    else
    {
        fprintf( fp, "CB : GetOpMode \n" );
        fprintf( fp, "Get Op Mode Status %lu \n",
                 pGetOpModeNotification->status );
        
        if(pGetOpModeNotification->pMode)
        {         
            fprintf( fp, "Current Operation mode : %lu \n", *pGetOpModeNotification->pMode );
            fprintf( fp, "1=Default 2=MSB 3=MSA 4=Standalone 5=Cell Id \n");                 
        }
    }
    tfclose(fp);
}

void doSLQSLOCGetOpMode(void)
{
    ULONG               nRet;
    FILE                *fp = NULL;
    /* Define the test cases to be executed */
    SLQSLOCGetOpModeTestCase_t SLQSLOCGetOpModeTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, "Get current operation mode"}
    };

    fp = tfopen( "../../loc/test/results/slqslocgetopmode.txt", "w" );
    if ( fp )
    {
        ULONG rc;
        BYTE tCaseNum     = 0;

        BYTE ncases = sizeof(SLQSLOCGetOpModeTestCases) /
            sizeof( SLQSLOCGetOpModeTestCases[0] );

        rc = SLQSSetLocGetOpModeCallback( &cbkTestSLQSSetLocGetOpModeCallback );
        if ( rc != eQCWWAN_ERR_NONE )
            fprintf( stderr, "SLQSSetLocGetOpModeCallback: Failed : %lu\r\n", rc );
        else
            printf( "SLQSSetLocGetOpModeCallback: Enabled callback\r\n" );            
            
        while( tCaseNum < ncases )
        {
            SLQSLOCGetOpModeTestCase_t *pd =
                &SLQSLOCGetOpModeTestCases[tCaseNum++];

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                    tCaseNum,
                    ncases,
                    pd->desc);
            
            
            nRet = SLQSLOCGetOpMode();
            
            /* Display result code and text */
            doprintreason( fp, nRet );
            sleep(1);
        }
        sleep(1);
        rc = SLQSSetLocGetOpModeCallback( NULL );
        if ( rc != eQCWWAN_ERR_NONE )
            fprintf( stderr, "SLQSSetLocGetOpModeCallback: Failed : %lu\r\n", rc );
        else
            printf( "SLQSSetLocGetOpModeCallback: Disabled\r\n" );    
        
    }

    if (fp)
        tfclose(fp);
}

/*************
 *
 * Name:    cbkTestSLQSSetLocGetFixCriteriaCallback
 *
 * Purpose: SLQSSetLocGetFixCriteriaCallback API callback function
 *
 **************/
local void cbkTestSLQSSetLocGetFixCriteriaCallback(
    QmiCbkLocGetFixCriteriaInd *pGetFixCriteriaNotification  )
{
    FILE *fp;
    
    fp = tfopen("TestReport/SLQSSetLocGetFixCriteriaCallback.txt", "a");

    if ( fp == NULL )
        perror("SLQSSetLocGetFixCriteriaCallback");
    else
    {
        fprintf( fp, "\nCB : GetFixCriteria \n" );
        fprintf( fp, "Get Fix Criteria Status %lu \n\n",
                 pGetFixCriteriaNotification->status );
        
        if(pGetFixCriteriaNotification->pAccuracy)
        {         
            fprintf( fp, "Accuracy : %lu \n", *pGetFixCriteriaNotification->pAccuracy );
            fprintf( fp, "1=Low 2=Medium 3=High \n\n");                 
        }
        
        if(pGetFixCriteriaNotification->pIntermediate)
        {         
            fprintf( fp, "Intermediate Report status : %lu \n", *pGetFixCriteriaNotification->pIntermediate );
            fprintf( fp, "1=ON 2=OFF \n\n");                 
        }
        
        if(pGetFixCriteriaNotification->pMinInterval)
        {         
            fprintf( fp, "Time interval between Fixes : %lu \n\n", *pGetFixCriteriaNotification->pMinInterval );
        }

        if(pGetFixCriteriaNotification->pAppID)
        {    
            BYTE i;
            LocAppProvider* pApp = (LocAppProvider *)pGetFixCriteriaNotification->pAppID;
            fprintf( fp, "Application Provider : ");
            for ( i = 0 ; i < pApp->providerLen ; i++ )
            {
                fprintf( fp,"%c:",pApp->provider[i]);
            }
            fprintf( fp,"\n");
            
            fprintf( fp, "Application Name : ");
            for ( i = 0 ; i < pApp->nameLen ; i++ )
            {
                fprintf( fp,"%c:",pApp->name[i]);
            }
            fprintf( fp,"\n");
            
            fprintf( fp, "Application version valid: %d", pApp->verValid );
            
            fprintf( fp, "Application version : ");
            for ( i = 0 ; i < pApp->versionLen ; i++ )
            {
                fprintf( fp,"%c:",pApp->version[i]);
            }
            fprintf( fp,"\n");
        }
    }
    tfclose(fp);
}

void doSLQSLOCGetFixCriteria(void)
{
    ULONG               nRet;
    FILE                *fp = NULL;
    /* Define the test cases to be executed */
    SLQSLOCGetFixCriteriaTestCase_t SLQSLOCGetFixCriteriaTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, "Get Fix Criteria"}
    };

    fp = tfopen( "../../loc/test/results/slqslocgetfixcriteria.txt", "w" );
    if ( fp )
    {
        ULONG rc;
        BYTE tCaseNum     = 0;

        BYTE ncases = sizeof(SLQSLOCGetFixCriteriaTestCases) /
            sizeof( SLQSLOCGetFixCriteriaTestCases[0] );

        rc = SLQSSetLocGetFixCriteriaCallback( &cbkTestSLQSSetLocGetFixCriteriaCallback);
        if ( rc != eQCWWAN_ERR_NONE )
            fprintf( stderr, "SLQSSetLocGetOpModeCallback: Failed : %lu\r\n", rc );
        else
            printf( "SLQSSetLocGetOpModeCallback: Enabled callback\r\n" );            
            
        while( tCaseNum < ncases )
        {
            SLQSLOCGetFixCriteriaTestCase_t *pd =
                &SLQSLOCGetFixCriteriaTestCases[tCaseNum++];

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                    tCaseNum,
                    ncases,
                    pd->desc);
            
            
            nRet = SLQSLOCGetFixCriteria();
            
            /* Display result code and text */
            doprintreason( fp, nRet );
            sleep(1);
        }
        sleep(1);
        rc = SLQSSetLocGetFixCriteriaCallback( NULL );
        if ( rc != eQCWWAN_ERR_NONE )
            fprintf( stderr, "SLQSSetLocGetFixCriteriaCallback: Failed : %lu\r\n", rc );
        else
            printf( "SLQSSetLocGetFixCriteriaCallback: Disabled\r\n" );    
        
    }

    if (fp)
        tfclose(fp);
}

void doSwiLocGetAutoStart(void)
{
    ULONG nRet;
    FILE  *fp = NULL;
    SwiLocGetAutoStartResp resp;

    fp = tfopen( "../../loc/test/results/swilocgetautostart.txt", "w" );
    if ( fp )
    {
        fprintf(fp, "\t null pointer\n");
        nRet = SwiLocGetAutoStart(NULL);
        doprintreason( fp, nRet );

        nRet = SwiLocGetAutoStart(&resp);

        if (nRet == eQCWWAN_ERR_NONE)
        {
            if (resp.function_reported)
                printf("\tfunction: %d\n", resp.function );
            if (resp.fix_type_reported)
                printf("\tfix_type: %d \n", resp.fix_type);
            if (resp.max_time_reported)
                printf("\tmax_time: %d\n", resp.max_time);
            if (resp.max_dist_reported)
                printf("\tmax_dist: %d\n", resp.max_dist);
            if (resp.fix_rate_reported)
                printf("\tfix_rate: %d\n", resp.fix_rate);
        }
        doprintreason( fp, nRet );
        tfclose(fp);
    }
}

void doSwiLocSetAutoStart(void)
{
    ULONG nRet;
    FILE  *fp = NULL;
    SwiLocSetAutoStartReq req;

    fp = tfopen( "../../loc/test/results/swilocgetautostart.txt", "w" );
    if ( fp )
    {
        fprintf(fp, "\t null pointer\n");
        nRet = SwiLocSetAutoStart(NULL);
        doprintreason( fp, nRet );

        //below values are extracted from EM7455
        memset(&req, 0, sizeof(req));
        fprintf(fp, "\t set nothing\n");
        nRet = SwiLocSetAutoStart(&req);
        doprintreason( fp, nRet );
        req.set_function = 1;
        req.function = 2;
        fprintf(fp, "\t set function to %d\n", req.function);
        nRet = SwiLocSetAutoStart(&req);
        doprintreason( fp, nRet );
        req.set_fix_type = 1;
        req.fix_type = 1;
        fprintf(fp, "\t and set fix_type %d\n", req.fix_type);
        nRet = SwiLocSetAutoStart(&req);
        doprintreason( fp, nRet );
        req.set_max_time = 1;
        req.max_time = 0xff;
        fprintf(fp, "\t and set max_time %d\n", req.max_time);
        nRet = SwiLocSetAutoStart(&req);
        doprintreason( fp, nRet );
        req.set_max_dist = 1;
        req.max_dist = 1000;
        fprintf(fp, "\t and set max_dist %d\n", req.max_dist);
        nRet = SwiLocSetAutoStart(&req);
        doprintreason( fp, nRet );
        req.set_fix_rate = 1;
        req.fix_rate = 1;
        fprintf(fp, "\t and set fix_rate %d\n", req.fix_rate);
        nRet = SwiLocSetAutoStart(&req);
        doprintreason( fp, nRet );

        tfclose(fp);
    }
}

void doSLQSLOCInjectPosition(void)
{
    ULONG               nRet;
    FILE                *fp = NULL;

    /* Define the test cases to be executed */
    SLQSLocInjectPositionTestCase_t SLQSLocInjectPositionTestCases[] =
    {
            { eQCWWAN_ERR_NONE, 1, NULL, NULL, NULL, NULL, NULL, NULL,
                     NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                     NULL, "All parameters NULL"},
            { eQCWWAN_ERR_NONE, 2, NULL, NULL, NULL, NULL, NULL, NULL,
                     NULL, NULL, NULL, NULL, NULL, &utcTime, NULL, NULL, NULL,
                     NULL, "Input UTC Time"},
            { eQCWWAN_ERR_NONE, 3, (ULONGLONG*)&latitude, (ULONGLONG*)&longitude, (ULONG*)&horUnc, NULL, NULL, NULL,
                     NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                     NULL, "Input Latitude , Longitude & Hor Unc Value"},
            { eQCWWAN_ERR_NONE, 4, NULL, NULL, NULL, NULL, NULL, (ULONG*)&altitudeWrtEllipsoid,
                     NULL, (ULONG*)&vertUnc, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                     NULL, "Input Altitude wrt Ellipsoid"},
    };

    fp = tfopen( "../../loc/test/results/slqslocinjectposition.txt", "w" );
    if ( fp )
    {
        ULONG rc;
        BYTE tCaseNum     = 0;
        BYTE ncases = sizeof( SLQSLocInjectPositionTestCases ) /
            sizeof( SLQSLocInjectPositionTestCases[0] );

        while( tCaseNum < ncases )
        {
            SLQSLocInjectPositionTestCase_t *pd =
                &SLQSLocInjectPositionTestCases[tCaseNum++];

            fprintf(fp, "\nTest Case %d / %d : %s\n",
                    tCaseNum,
                    ncases,
                    pd->desc);

            LocInjectPositionReq locInjectPositionReq;
            memset(&locInjectPositionReq, 0, sizeof(locInjectPositionReq));

            if(tCaseNum==2)
                GetCurrentUTCTime();

            locInjectPositionReq.pAltitudeSrcInfo         = pd->pAltitudeSrcInfo;
            locInjectPositionReq.pAltitudeWrtEllipsoid    = pd->pAltitudeWrtEllipsoid;
            locInjectPositionReq.pAltitudeWrtMeanSeaLevel = pd->pAltitudeWrtMeanSeaLevel;
            locInjectPositionReq.pHorConfidence           = pd->pHorConfidence;
            locInjectPositionReq.pHorReliability          = pd->pHorReliability;
            locInjectPositionReq.pHorUncCircular          = pd->pHorUncCircular;
            locInjectPositionReq.pLatitude                = pd->pLatitude;
            locInjectPositionReq.pLongitude               = pd->pLongitude;
            locInjectPositionReq.pPositionSrc             = pd->pPositionSrc;
            locInjectPositionReq.pRawHorConfidence        = pd->pRawHorConfidence;
            locInjectPositionReq.pRawHorUncCircular       = pd->pRawHorUncCircular;
            locInjectPositionReq.pTimestampAge            = pd->pTimestampAge;
            locInjectPositionReq.pTimestampUtc            = pd->pTimestampUtc;
            locInjectPositionReq.pVertConfidence          = pd->pVertConfidence;
            locInjectPositionReq.pVertReliability         = pd->pVertReliability;
            locInjectPositionReq.pVertUnc                 = pd->pVertUnc;

            nRet = SLQSLOCInjectPosition(&locInjectPositionReq);

            /* Display result code and text */
            doprintreason( fp, nRet );
            if ( eQCWWAN_ERR_NONE == nRet )
            {
                fprintf(fp, "SLQSLOCInjectPosition Successful\n");
            }
        }
    }
    if (fp)
        tfclose(fp);
}

/*
 * Name:    doSLQSLOCSetCradleMountConfig
 *
 * Purpose: Perform the tests that call the SLQSLOCSetCradleMountConfig() API
 *
 * Return:  none
 *
 * Notes:   none
 *
 */
void doSLQSLOCSetCradleMountConfig( void )

{
    BYTE confidence  = 70;

    /* Test Cases */
    SLQSLOCSetCradleMountConfigTestCase_t SLQSLOCSetCradleMountConfigTestCase[]=
    {
         { eQCWWAN_ERR_NONE, 1, 1, NULL, "Device mounted on Cradle"},
         { eQCWWAN_ERR_NONE, 2, 1, &confidence, "Device mounted on Cradle with confidence"},
         { eQCWWAN_ERR_NONE, 3, 0, NULL, "Device not mounted on Cradle "},
         { eQCWWAN_ERR_NONE, 4, 9 ,NULL, "InValid Value"},
    };

    LocSetCradleMountReq req;
    FILE *fp = tfopen("../../loc/test/results/slqsloccradlemountconfig.txt", "w");
    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSLOCSetCradleMountConfigTestCase )/
                      sizeof( SLQSLOCSetCradleMountConfigTestCase[0] );
        while ( tc < ncases )
        {
        	SLQSLOCSetCradleMountConfigTestCase_t *pd =
                   &SLQSLOCSetCradleMountConfigTestCase[tc++];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tc,
                        ncases,
                        pd->desc);

            req.state = pd->state;
            req.pConfidence = pd->pConfidence;
            fprintf(fp, "Cradle Mount State = %d\n",req.state);
            if(req.pConfidence != NULL)
            fprintf(fp, "Cradle Mount Confidence = %d\n",*req.pConfidence);
            ULONG nRet = SLQSLOCSetCradleMountConfig( &req );
            doprintreason( fp, nRet );

            if ( eQCWWAN_ERR_NONE == nRet )
            {
                fprintf(fp, "SLQSLOCSetCradleMountConfig Successful\n");
            }
        }
    }
    if (fp)
        tfclose(fp);
}


/*
 * Name:    doSLQSLOCInjectSensorData
 *
 * Purpose: Perform the tests that call the SLQSLOCInjectSensorData() API
 *
 * Return:  none
 *
 * Notes:   none
 *
 */
void doSLQSLOCInjectSensorData( void )

{
    ULONG opaqueIdentifier = 123456;

    sensorData AcceleroData;
    AcceleroData.timeOfFirstSample = 100000;
    AcceleroData.flags             = 0x02;
    AcceleroData.sensorDataLen     = 1;
    AcceleroData.timeOffset[0]     = 10000;
    float xAxis = 10.234;
    float yAxis = 10.235;
    float zAxis = 10.236;
    AcceleroData.xAxis[0]          = *(ULONG*)&xAxis;
    AcceleroData.yAxis[0]          = *(ULONG*)&yAxis;
    AcceleroData.zAxis[0]          = *(ULONG*)&zAxis;

    tempratureData   AcceleroTempData;
    AcceleroTempData.timeOfFirstSample  = 100000;
    AcceleroTempData.timeSource         = 0x01;
    AcceleroTempData.temperatureDataLen = 1;
    AcceleroTempData.timeOffset[0]      = 10000;
    float temperature    = 28.24;
    AcceleroTempData.temperature[0] = *(ULONG*)&temperature;

    /* Test Cases */
    SLQSLOCInjectSensorDataTestCase_t SLQSLOCInjectSensorDataTestCase[]=
    {
         { eQCWWAN_ERR_NONE, 1,NULL, NULL, NULL,NULL, NULL, NULL, NULL, "All Optional Null"},
         { eQCWWAN_ERR_NONE, 2, &opaqueIdentifier, NULL, NULL,NULL, NULL, NULL, NULL, "Opaque Identifier"},
         { eQCWWAN_ERR_NONE, 3, &opaqueIdentifier, &AcceleroData, NULL,NULL, NULL, NULL, NULL, "Accelerometer Data"},
         { eQCWWAN_ERR_NONE, 4, &opaqueIdentifier, &AcceleroData, NULL,NULL, NULL, &AcceleroTempData, NULL, "Accelerometer temperature"}
    };

    LocInjectSensorDataReq req;
    BYTE count;

    FILE *fp = tfopen("../../loc/test/results/slqslocinjectsensordata.txt", "w");
    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSLOCInjectSensorDataTestCase )/
                      sizeof( SLQSLOCInjectSensorDataTestCase[0] );
        while ( tc < ncases )
        {
            SLQSLOCInjectSensorDataTestCase_t *pd =
                   &SLQSLOCInjectSensorDataTestCase[tc++];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tc,
                        ncases,
                        pd->desc);

            req.pOpaqueIdentifier = pd->pOpaqueIdentifier;
            req.pAcceleroData     = pd->pAcceleroData;
            req.pGyroData         = pd->pGyroData;
            req.pAcceleroTimeSrc  = pd->pAcceleroTimeSrc;
            req.pGyroTimeSrc      = pd->pGyroTimeSrc;
            req.pAcceleroTempData = pd->pAcceleroTempData;
            req.pGyroTempData     = pd->pGyroTempData;

            IFPRINTF(fp, "Opaque Identifier = %d\n",req.pOpaqueIdentifier);
            if(NULL != req.pAcceleroData)
            {
                fprintf(fp, "\nAccelerometer Data :  \n");
                fprintf(fp, "Time of First Sample = %d\n",req.pAcceleroData->timeOfFirstSample);
                fprintf(fp, "Flags = %d\n",req.pAcceleroData->flags);
                for(count=0 ; count< req.pAcceleroData->sensorDataLen ; count++)
                {
                    fprintf(fp, "Time offset = %d\n",req.pAcceleroData->timeOffset[count]);
                    fprintf(fp, "Sensor x-Axis = %f\n",*(float*)&req.pAcceleroData->xAxis[count]);
                    fprintf(fp, "Sensor y-Axis = %f\n",*(float*)&req.pAcceleroData->yAxis[count]);
                    fprintf(fp, "Sensor z-Axis = %f\n",*(float*)&req.pAcceleroData->zAxis[count]);
                }
            }
            IFPRINTF(fp, "Accelerometer Time Src = %d\n",req.pAcceleroTimeSrc);
            IFPRINTF(fp, "Gyroscope Time Src = %d\n",req.pGyroTimeSrc);
            if(NULL != req.pAcceleroTempData)
            {
                fprintf(fp, "\nAccelerometer Temperature Data :  \n");
                fprintf(fp, "Time Source = %d\n",req.pAcceleroTempData->timeSource);
                fprintf(fp, "Time of First Sample = %d\n",req.pAcceleroTempData->timeOfFirstSample);
                for(count=0 ; count< req.pAcceleroTempData->temperatureDataLen ; count++)
                {
                    fprintf(fp, "Time offset = %d\n",req.pAcceleroTempData->timeOffset[count]);
                    fprintf(fp, "Sensor Temperature = %f\n",*(float*)&req.pAcceleroTempData->temperature[count]);
                }
            }

            ULONG nRet = SLQSLOCInjectSensorData( &req );
            doprintreason( fp, nRet );

            if ( eQCWWAN_ERR_NONE == nRet )
            {
                fprintf(fp, "SLQSLOCInjectSensorData Successful\n");
            }
        }
    }
    if (fp)
        tfclose(fp);
}
