/*
 *
 *  Filename: qaGobiApiWdsTest.c
 *
 *  Purpose:  Contains functions implementing specific tests for
 *            Wireless Data Service (WDS)
 *            called by the main() routine in qatest.c
 *
 *  Copyright: © 2011-13 Sierra Wireless, Inc., all rights reserved
 *
 **************/

/* include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* include files */
#include "SwiDataTypes.h"
#include "qmerrno.h"
#include "qaGobiApiWds.h"
#include "qaGobiApiWdsTest.h"
#include "qatesthelper.h"
#include "qatestproto.h"
#include "sludefs.h"

/*******************
    TEST DATA
 *******************/
/* Constants */
#define IPv6 6
#define TECHNOLOGY_3GPP         1
#define TECHNOLOGY_3GPP2        2
#define ZERO_SESSION_ID         0

/* Global Variables */
local struct ssdatasession_params s4[3];
local struct ssdatasession_params s6[3];
local struct ssdatasession_params s46[3];

local WORD SetProfileList[5] = { 0x01, 0x02, 0x04, 0x03, 0x05 };
local WORD SetLTEAttachProfile = 1;
local BYTE SetDefaultPDNEnabled = 1;
local BYTE SetRelease3GPP = 2;

local WORD LTEAttachProfile;
local WORD ProfileList[5];
local BYTE defaultPDNEnabled;
local BYTE Release3GPP;
local CHAR username[50]="\0";
local CHAR password[50]="\0";
local ULONG PAPPer     = 0x01;
local ULONG CHAPPer    = 0x02;
local ULONG instanceId = 0x00;
local ULONG tech;

/* Get ProfileSettings test data */
local BYTE                    profType3GPP = 0;
local BYTE                    profIndex = 1;
local CHAR                    profName[255];
local WORD                    ValprofNameSize = 255;
local WORD                    InValprofNameSize = 0;
local BYTE                    PdpType;
local BYTE                    PDPHDRCompType;
local BYTE                    PDPDataCompType;
local CHAR                    ApnName[255];
local WORD                    ValApnNameSize = 255;
local WORD                    InValApnNameSize = 0;
local ULONG                   PriDNSIPv4AddPref;
local ULONG                   SecDNSIPv4AddPref;
local struct UMTSQoS          UMTSReqQoS;
local struct UMTSQoS          UMTSMinQos;
local struct GPRSRequestedQoS GPRSReqQoS;
local struct GPRSRequestedQoS GPRSminQoS;
local CHAR                    UserName[255];
local WORD                    ValUserNameSize = 255;
local WORD                    InValUserNameSize = 0;
local CHAR                    Passwd[255];
local WORD                    ValPasswdSize = 255;
local WORD                    InValPasswdSize = 0;
local BYTE                    AuthPref;
local ULONG                   IPv4AddPref;
local BYTE                    PCSCFAddPCO;
local BYTE                    PDPAccCtrlFlag;
local BYTE                    PCSCFAddDHCP;
local BYTE                    IMCNFlag;
local struct TFTIDParams      TFTID1;
local struct TFTIDParams      TFTID2;
local BYTE                    PDPCon;
local BYTE                    SecFlag;
local BYTE                    priID;
local USHORT                  IPv6AddrPref[8];
local struct UMTSReqQoSSigInd UMTSReqQoSsigInd;
local struct UMTSReqQoSSigInd UMTSMinQoSsigInd;
local USHORT                  priDNSIPv6AddPref[8];
local USHORT                  secDNSIPv6AddPref[8];
local BYTE                    addrAllocPref;
local struct QosClassID       QoSCLSID;
local BYTE                    ApnDisFlag;
local ULONG                   PDNInactivTim;
local BYTE                    APNClass;
local WORD                    extErrCode;

local BYTE   profType3GPP2 = 1;
local BYTE   profIndex3GPP2 = 0;
local BYTE   NegoDNSServPref;
local ULONG  PPPSessCloTimD0;
local ULONG  PPPSessCloTim1X;
local BYTE   AllowLinger;
local USHORT LcpAckTimeout;
local USHORT IpcpAckTimeout;
local USHORT AuthTimeout;
local BYTE   LcpCreqRetCt;
local BYTE   IpCpCReqRetCt;
local BYTE   authRetryCt;
local BYTE   authProt;
local CHAR   userID[127];
local WORD   ValuserIDSize = 255;
local WORD   InValuserIDSize = 0;
local CHAR   authPwd[127];
local WORD   ValauthPwdSize = 255;
local WORD   InValauthPwdSize = 0;
local BYTE   DataRate;
local ULONG  AppType;
local BYTE   DataMode;
local BYTE   appPrio;
local CHAR   ApnStr[100];
local WORD   ValApnStrSize = 255;
local WORD   InValApnStrSize = 0;
local BYTE   PdnType;
local BYTE   isPcscfAdd;
local ULONG  PriDNSIPv4Add;
local ULONG  SecDNSIPv4Add;
local USHORT PriDNSIPv6Add[8];
local USHORT SecDNSIPv6Add[8];
local BYTE   RatType;
local BYTE   ApnEnable3GPP2;
local QmiWDSDataBearers          DataBearers;
local QmiWDSDataBearerTechnology curDataBearerTechnology;
local QmiWDSDataBearerTechnology lastCallDataBearerTechnology;

/* Wds Set Event Report parameters */
local BYTE      unSetInd        = 0x00;
local BYTE      setInd          = 0x01;
local TrStatInd transferStatInd = { 0x10, 0x00000080 };

/* Forward declartion */
void GetAuthenticationPrams( struct ssdatasession_params *pin );

/*******************
  TEST FUNCTIONS
 *******************/

/*
 * Name:     doGetDefaultProfile
 *
 * Purpose:  Perform the tests that call the GetDefaultProfile() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doGetDefaultProfile( void )
{
    BYTE  tCaseNum = 0;
    ULONG nRet;
    ULONG profileType;
    ULONG PDPType;
    ULONG IPAddress;
    ULONG PrimaryDNS;
    ULONG SecondaryDNS;
    ULONG Authentication;
    BYTE  nameSize = nMaxStrLen-1;
    CHAR  Name[nMaxStrLen];
    BYTE  apnSize = nMaxStrLen-1;
    CHAR  APNName[nMaxStrLen];
    BYTE  userSize = nMaxStrLen-1;
    CHAR  Username[nMaxStrLen];
    BYTE  pwdSize = nMaxStrLen-1;
    CHAR  passwork[nMaxStrLen];

    FILE  *fp=NULL;

    fp = tfopen("../../wds/test/results/getdefaultprofile.txt","w");

    if (fp)
    {
        /*
         * Assuming SetDefaultProfile needs to be called atleast once
         * before making calls to GetDefaultProfile
         */

        while(tCaseNum < MAX_GET_DEFAULT_PROFILE_TESTCASE_NUM)
        {
            profileType     = GetDefaultProfileTestCases[tCaseNum].profileType;

            fprintf(fp,"\n\nTest Case %d / %d : %s\n",
                       (tCaseNum + 1),
                       MAX_GET_DEFAULT_PROFILE_TESTCASE_NUM,
                       GetDefaultProfileTestCases[tCaseNum].desc);

            nRet = GetDefaultProfile(profileType,
                                     &PDPType,
                                     &IPAddress,
                                     &PrimaryDNS,
                                     &SecondaryDNS,
                                     &Authentication,
                                     nameSize,
                                     &Name[0],
                                     apnSize,
                                     &APNName[0],
                                     userSize,
                                     &Username[0],
                                     pwdSize,
                                     &passwork[0]);

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
            {
                fprintf(fp,"Received Profile Details are as follows:\n");
                fprintf(fp,"\nPDPType :%lx",PDPType);
                fprintf(fp,"\nIPAddress :%lx",IPAddress);
                fprintf(fp,"\nPrimaryDNS :%lx",PrimaryDNS);
                fprintf(fp,"\nSecondaryDNS :%lx",SecondaryDNS);
                fprintf(fp,"\nAuthentication :%lx",Authentication);
                fprintf(fp,"\nName : %s",Name);
                fprintf(fp,"\nAPNName :%s",APNName);
                fprintf(fp,"\nUsername :%s",Username);
                fprintf(fp,"\nPassword :%s",passwork);
            }

            tCaseNum++;
        }
    }

    if (fp)
        tfclose(fp);
}

/*
 * Name:     doGetDefaultProfileLTE
 *
 * Purpose:  Perform the tests that call the GetDefaultProfileLTE() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doGetDefaultProfileLTE( void )
{
    BYTE   tCaseNum = 0;
    ULONG  nRet;
    ULONG  profileType;
    ULONG  PDPType;
    ULONG  IPAddressv4;
    ULONG  PrimaryDNSv4;
    ULONG  SecondaryDNSv4;
    USHORT IPAddressv6[8] = { 0,0,0,0,0,0,0,0 };
    USHORT PrimaryDNSv6 = 0;
    USHORT SecondaryDNSv6 = 0;
    ULONG  Authentication;
    BYTE   nameSize = nMaxStrLen-1;
    CHAR   Name[nMaxStrLen];
    BYTE   apnSize = nMaxStrLen-1;
    CHAR   APNName[nMaxStrLen];
    BYTE   userSize = nMaxStrLen-1;
    CHAR   Username[nMaxStrLen];
    USHORT idx;
    FILE  *fp=NULL;

    fp = tfopen("../../wds/test/results/getdefaultprofilelte.txt","w");

    if (fp)
    {
        /*
         * Assuming SetDefaultProfile needs to be called atleast once
         * before making calls to GetDefaultProfile
         */
        /*rCodeSetDP = SetDefaultProfile(0,
                                   &setPDPType,
                                   &setIPAddress,
                                   &setPrimaryDNSv4,
                                   &setSecondaryDNSv4,
                                   &setAuthenticationv4,
                                   &setPrimaryDNSv6,
                                   &setSecondaryDNSv6,
                                   &setAuthenticationv6,
                                   &setName[0],
                                   &setAPNName[0],
                                   &setUsername[0],
                                   &setPassword[0]);

        if (rCodeSetDP != eQCWWAN_ERR_NONE)
        {
            fprintf(fp,"As Profile is not set No further test cases can be run\n");
            fprintf(fp,"Hence Quiting\n");
            exit(1);
        }*/

        while(tCaseNum < MAX_GET_DEFAULT_PROFILE_TESTCASE_NUM)
        {
            profileType     = GetDefaultProfileTestCases[tCaseNum].profileType;

            fprintf(fp,"\n\nTest Case %d / %d : %s\n",
                       (tCaseNum + 1),
                       MAX_GET_DEFAULT_PROFILE_TESTCASE_NUM,
                       GetDefaultProfileTestCases[tCaseNum].desc);

            nRet = GetDefaultProfileLTE(profileType,
                                     &PDPType,
                                     &IPAddressv4,
                                     &PrimaryDNSv4,
                                     &SecondaryDNSv4,
                                     IPAddressv6,
                                     &PrimaryDNSv6,
                                     &SecondaryDNSv6,
                                     &Authentication,
                                     nameSize,
                                     &Name[0],
                                     apnSize,
                                     &APNName[0],
                                     userSize,
                                     &Username[0]);

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
            {
                fprintf(fp,"Received Profile Details are as follows:\n");
                fprintf(fp,"\nPDPType :%lx",PDPType);
                fprintf(fp,"\nIPAddressv4 :%lx",IPAddressv4);
                fprintf(fp,"\nPrimaryDNSv4 :%lx",PrimaryDNSv4);
                fprintf(fp,"\nSecondaryDNSv4 :%lx",SecondaryDNSv4);

                for (idx = 0; idx < 8; idx++)
                fprintf(fp, "IP V6 Address is: %hx\n", IPAddressv6[idx] );

                fprintf(fp,"\nPrimaryDNSv6 :%d",PrimaryDNSv6);
                fprintf(fp,"\nSecondaryDNSv6 :%d",SecondaryDNSv6);
                fprintf(fp,"\nAuthentication :%lx",Authentication);
                fprintf(fp,"\nName : %s",Name);
                fprintf(fp,"\nAPNName :%s",APNName);
                fprintf(fp,"\nUsername :%s",Username);
            }

            tCaseNum++;
        }
    }

    if (fp)
        tfclose(fp);
}

/*
 * Name:     doGetAutoconnect
 *
 * Purpose:  Perform the tests that call the GetAutoconnect() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doGetAutoconnect( void )
{
    BYTE  tCaseNum = 0;
    ULONG nRet;
    ULONG setting;
    FILE  *fp=NULL;

    fp = tfopen("../../wds/test/results/getautoconnect.txt","w");

    if (fp)
    {
        while(tCaseNum < MAX_GET_AUTOCONNECT_TESTCASE_NUM)
        {
            fprintf(fp,"\n\nTest Case %d / %d : %s\n",
                       (tCaseNum + 1),
                       MAX_GET_AUTOCONNECT_TESTCASE_NUM,
                       GetAutoconnectTestCases[tCaseNum].desc);

            nRet = GetAutoconnect ( &setting );

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
            {
                fprintf(fp,"Autoconnect Setting is : %lu\n",setting);
            }

            tCaseNum++;
        }
    }

    if (fp)
        tfclose(fp);
}

/*
 * Name:     doGetSessionState
 *
 * Purpose:  Perform the test that calls the GetSessionState() API
 *
 * Parms:    not used - for future variable parameters ?
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doGetSessionState( void )
{
    BYTE   tCaseNum = 0;
    ULONG  nRet;
    FILE   *fp=NULL;
    ULONG  State = 0;

    /* Test cases */
    GetSessionStateTestCase_t GetSessionStateTestCases[] =
    {
        {eQCWWAN_ERR_NONE, 1, 0, "Valid SessionState"},
        {eQCWWAN_ERR_NONE, 1, 1, "Valid SessionState"},
        {eQCWWAN_ERR_NONE, 1, 2, "Valid SessionState"}
    };

    fp = tfopen("../../wds/test/results/getsessionstate.txt","w");
    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof(GetSessionStateTestCases)/
                      sizeof(GetSessionStateTestCases[0]);

        while(tc < ncases )
        {
            GetSessionStateTestCase_t *pd = &GetSessionStateTestCases[tc++];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tc,
                        ncases,
                        pd->desc);

            /* Calling GetSessionState */
            nRet = GetSessionState( &State,
                                    pd->instance );

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
            {
                fprintf(fp, "session state: %lu\n", State );
            }

            tCaseNum++;
        }

    }
    if (fp)
        tfclose(fp);
}

/*
 * Name:     doGetPacketStatus
 *
 * Purpose:  Perform the test that calls the GetPacketStatus() API
 *
 * Parms:    not used - for future variable parameters ?
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doGetPacketStatus( void )
{
    BYTE   tCaseNum = 0;
    ULONG  TXPacketSuccesses = 0;
    ULONG  RXPacketSuccesses = 0;
    ULONG  TXPacketErrors = 0;
    ULONG  RXPacketErrors = 0;
    ULONG  TXPacketOverflows = 0;
    ULONG  RXPacketOverflows = 0;
    BYTE   instance;
    ULONG  nRet;
    FILE   *fp=NULL;

    fp = tfopen("../../wds/test/results/getpacketstatus.txt","w");
    if (fp)
    {

        while(tCaseNum < MAX_GET_PACKET_STATUS_TESTCASE_NUM)
        {
            fprintf(fp,"Test Case %d / %d : %s\n",
                        tCaseNum + 1,
                        MAX_GET_SESSION_STATE_TESTCASE_NUM,
                        GetPacketStatusTestCases[tCaseNum].desc);
            instance =  GetPacketStatusTestCases[tCaseNum].instance;

            nRet = GetPacketStatus (
                        &TXPacketSuccesses,
                        &RXPacketSuccesses,
                        &TXPacketErrors,
                        &RXPacketErrors,
                        &TXPacketOverflows,
                        &RXPacketOverflows,
                        instance );

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
            {
                fprintf(fp, "TXPacketSuccesses: %lx\n", TXPacketSuccesses );
                fprintf(fp, "RXPacketSuccesses: %lx\n", RXPacketSuccesses );
                fprintf(fp, "TXPacketErrors: %lx\n", TXPacketErrors );
                fprintf(fp, "RXPacketErrors: %lx\n", RXPacketErrors );
                fprintf(fp, "TXPacketOverflows: %lx\n", TXPacketOverflows );
                fprintf(fp, "RXPacketOverflows: %lx\n", RXPacketOverflows );
            }

            tCaseNum++;
        }

    }
    if (fp)
        tfclose(fp);
}

/*
 * Name:     doGetByteTotals
 *
 * Purpose:  Perform the test that calls the GetByteTotals() API
 *
 * Parms:    not used - for future variable parameters ?
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doGetByteTotals( void )
{
    ULONGLONG  TXTotalBytes;
    ULONGLONG  RXTotalBytes;
    BYTE       instance;
    ULONG      nRet;
    FILE       *fp=NULL;

    /* Test cases */
    GetByteTotalsTestCase_t GetByteTotalsTestCases[] =
    {
        {eQCWWAN_ERR_NONE, 1, 0, "Valid ByteTotals"},
        {eQCWWAN_ERR_NONE, 2, 1, "Valid ByteTotals"},
        {eQCWWAN_ERR_NONE, 3, 2, "Valid ByteTotals"}
    };

    fp = tfopen("../../wds/test/results/getbytetotals.txt","w");
    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof(GetByteTotalsTestCases)/
                      sizeof(GetByteTotalsTestCases[0]);

        while(tc < ncases)
        {
            GetByteTotalsTestCase_t *pd = &GetByteTotalsTestCases[tc++];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tc,
                        ncases,
                        pd->desc);

            instance = pd->instance;

            nRet = GetByteTotals (
                        &TXTotalBytes,
                        &RXTotalBytes,
                        instance );

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
            {
                fprintf(fp, "TXTotalBytes: %llx\n", TXTotalBytes );
                fprintf(fp, "RXTotalBytes: %llx\n", RXTotalBytes );
            }
        }

    }
    if (fp)
        tfclose(fp);
}

/*
 * Name:     doGetDormancyState
 *
 * Purpose:  Perform the test that calls the GetDormancyState() API
 *
 * Parms:    not used - for future variable parameters ?
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doGetDormancyState( void )
{
    BYTE   tCaseNum = 0;
    ULONG  nRet;
    FILE   *fp=NULL;

    fp = tfopen("../../wds/test/results/getdormancystate.txt","w");
    if (fp)
    {
        while(tCaseNum < MAX_GET_DORMANCY_STATE_TESTCASE_NUM)
        {
            fprintf(fp,"\nTest Case %d / %d : %s\n",
                        tCaseNum+1,
                        MAX_GET_DORMANCY_STATE_TESTCASE_NUM,
                        GetDormancyStateTestCases[tCaseNum].desc);

            nRet = GetDormancyState (
                        GetDormancyStateTestCases[tCaseNum].pDormancyState,
                        GetDormancyStateTestCases[tCaseNum].instance_id);

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
            {
                fprintf(fp, "Dormancy State: %lx\n", *(GetDormancyStateTestCases[tCaseNum].pDormancyState));
            }

            tCaseNum++;
        }

    }
    if (fp)
        tfclose(fp);
}

/*
 * Name:     doGetDataBearerTechnology
 *
 * Purpose:  Perform the test that calls the GetDataBearerTechnology() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doGetDataBearerTechnology( void )
{
    ULONG nRet;
    ULONG *pDataBearer;
    FILE   *fp=NULL;

    GetDataBrTechTestCase_t GetDataBrTechTestCases[] =
    {
        {eQCWWAN_ERR_NONE, 1, NULL,        0, "NULL Parameter Testing"},
        {eQCWWAN_ERR_NONE, 2, &DataBearer, 0, "Valid DataBearer Technology instance:0"},
        {eQCWWAN_ERR_NONE, 3, &DataBearer, 1, "Valid DataBearer Technology instance:1"}
    };

    fp = tfopen("../../wds/test/results/getdatabearer.txt","w");

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( GetDataBrTechTestCases )/
                      sizeof( GetDataBrTechTestCases[0]);

        while(tc < ncases)
        {
            GetDataBrTechTestCase_t *pd = &GetDataBrTechTestCases[tc++];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tc,
                        ncases,
                        pd->desc);
            pDataBearer = pd->pDataBearer;
            nRet = GetDataBearerTechnology ( pDataBearer,
                                             pd->instance );

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
                fprintf(fp, "Databearer %lu\n", *pDataBearer );

        }
    }
    if (fp)
        tfclose(fp);
}

/*
 * Name:     doSLQSGetDataBearerTechnology
 *
 * Purpose:  Perform the test that calls the SLQSGetDataBearerTechnology() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetDataBearerTechnology( void )
{
    BYTE  tCaseNum = 0;
    ULONG nRet;
    FILE   *fp=NULL;
    QmiWDSDataBearers          *pDataBearers;
    QmiWDSDataBearerTechnology *pCurDBTech;
    QmiWDSDataBearerTechnology *pLastCallDBTech;
    BYTE                       instance;

    GetCurDataBrTechTestCase_t GetCurDataBrTechTestCases[] =
    {
        {eQCWWAN_ERR_NONE, 1, NULL, NULL,  NULL, 0, "NULL Parameter Testing"},
        {eQCWWAN_ERR_NONE, 2, &DataBearers, &curDataBearerTechnology, NULL, 0,
                                          "Valid current databearer technology, instance = 0"},
        {eQCWWAN_ERR_NONE, 3, &DataBearers, NULL, &lastCallDataBearerTechnology, 0,
                                        "Valid last call databearer technology, instance = 0"},
        {eQCWWAN_ERR_NONE, 4, &DataBearers, &curDataBearerTechnology,
                                    &lastCallDataBearerTechnology, 0,  "All Valid, instance = 0"},
        {eQCWWAN_ERR_NONE, 5, &DataBearers, &curDataBearerTechnology, NULL, 1,
                                          "Valid current databearer technology, instance = 1"},
        {eQCWWAN_ERR_NONE, 6, &DataBearers, NULL, &lastCallDataBearerTechnology, 1,
                                        "Valid last call databearer technology, instance = 1"},
        {eQCWWAN_ERR_NONE, 7, &DataBearers, &curDataBearerTechnology,
                                    &lastCallDataBearerTechnology, 1,  "All Valid, instance = 1"},
        {eQCWWAN_ERR_NONE, 8, &DataBearers, &curDataBearerTechnology, NULL, 2,
                                          "Valid current databearer technology, instance = 2"},
        {eQCWWAN_ERR_NONE, 9, &DataBearers, NULL, &lastCallDataBearerTechnology, 2,
                                        "Valid last call databearer technology, instance = 2"},
        {eQCWWAN_ERR_NONE, 10, &DataBearers, &curDataBearerTechnology,
                                    &lastCallDataBearerTechnology, 2,  "All Valid, instance = 2"}
    };

    fp = tfopen("../../wds/test/results/getdatabearer.txt","w");
    if (fp)
    {
        BYTE ncases = sizeof(GetCurDataBrTechTestCases)/
                      sizeof(GetCurDataBrTechTestCases[0]);
        while(tCaseNum < ncases)
        {
            GetCurDataBrTechTestCase_t *pd = &GetCurDataBrTechTestCases[tCaseNum];
            fprintf(fp, "\nTest Case %lu / %d : %s\n",
                        pd->testCaseId,
                        ncases,
                        pd->desc);

            pDataBearers    = pd->pDataBearers;
            pCurDBTech      = pd->pCurDataBearerTechnology;
            pLastCallDBTech = pd->pLastCallDataBearerTechnology;
            instance        = pd->instance;

            if(pDataBearers != NULL)
            {
                pDataBearers->pCurDataBearerTechnology = pCurDBTech;
                pDataBearers->pLastCallDataBearerTechnology = pLastCallDBTech;
            }

            nRet = SLQSGetDataBearerTechnology (pDataBearers,
                                                instance );

            /* Display result code and text */
            doprintreason (fp, nRet);

            /* Display the results based on the data bearer mask */
            if (!nRet)
            {
                if (pDataBearers)
                {
                    fprintf(fp, "DataBearerMask: 0x%x\n",
                                            pDataBearers->dataBearerMask );
                    if (pCurDBTech &&
                       (pDataBearers->dataBearerMask & QMI_WDS_CURRENT_CALL_DB_MASK))
                    {
                        fprintf(fp, "Current data bearer details\n" );
                        fprintf(fp, "Network: %d\n",
                                 pCurDBTech->currentNetwork );
                        fprintf(fp, "Rat Mask: %lu\n", pCurDBTech->ratMask);
                        fprintf(fp, "So Mask:  %lu\n", pCurDBTech->soMask );
                    }
                    if (pLastCallDBTech &&
                       (pDataBearers->dataBearerMask & QMI_WDS_LAST_CALL_DB_MASK))
                    {
                        fprintf(fp, "Last Call data bearer details\n" );
                        fprintf(fp, "Network:  %d\n",
                                    pLastCallDBTech->currentNetwork );
                        fprintf(fp, "Rat Mask: %lu\n", pLastCallDBTech->ratMask);
                        fprintf(fp, "So Mask:  %lu\n", pLastCallDBTech->soMask );
                    }
                }
            }
            tCaseNum++;
        }
    }
    if (fp)
        tfclose(fp);
}

/*
 * Name:     doGetSessionDuration
 *
 * Purpose:  Perform the test that calls the GetSessionDuration() API
 *
 * Parms:    not used - for future variable parameters ?
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doGetSessionDuration( void )
{
    ULONGLONG  Duration;
    ULONG      nRet;
    FILE       *fp=NULL;
    GetSessionDurationTestCase_t
        GetSessionDurationTestCases[MAX_GET_SESSION_DURATION_TESTCASE_NUM] =
    {
        {eQCWWAN_ERR_NONE, 1, 0, "Valid SessionDuration, instance=0"},
        {eQCWWAN_ERR_NONE, 2, 1, "Valid SessionDuration, instance=1"},
        {eQCWWAN_ERR_NONE, 3, 2, "Valid SessionDuration, instance=2"}
    };

    fp = tfopen("../../wds/test/results/getsessionduration.txt","w");
    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( GetSessionDurationTestCases )/
                      sizeof( GetSessionDurationTestCases[0] );
        while(tc < ncases)
        {
            GetSessionDurationTestCase_t *pd = &GetSessionDurationTestCases[tc++];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tc,
                        ncases,
                        pd->desc);

            nRet = GetSessionDuration ( &Duration,
                                         pd->instance );

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
            {
                fprintf(fp, "Session Duration: %llx\n", Duration );
            }
        }
    }
    if (fp)
        tfclose(fp);
}

/*
 * Name:     doGetConnectionRate
 *
 * Purpose:  Perform the tests that call the doGetConnectionRate() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doGetConnectionRate( void )
{
    ULONG  CurrentChannelTXRate=0;
    ULONG  CurrentChannelRXRate=0;
    ULONG  MaxChannelTXRate=0;
    ULONG  MaxChannelRXRate=0;
    BYTE   instance;
    FILE   *fp=NULL;

    GetConnectionRateTestCaseStep_t GetConnectionRateTestCases[] =
    {
        {eQCWWAN_ERR_NONE, 1, 0, "All Valid instance = 0"},
        {eQCWWAN_ERR_NONE, 2, 1, "All Valid instance = 1"},
        {eQCWWAN_ERR_NONE, 3, 2, "All Valid instance = 2"},
    };
    fp = tfopen("../../wds/test/results/getconnectionrate.txt","w");

    if (fp)
    {
        BYTE tc = 0;
        BYTE  ncases = sizeof( GetConnectionRateTestCases )/
                       sizeof( GetConnectionRateTestCases[0]);
        while ( tc < ncases )
        {
            GetConnectionRateTestCaseStep_t *pd = &GetConnectionRateTestCases[tc++];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tc,
                        ncases,
                        pd->desc);
            instance = pd->instance;
            ULONG nRet = GetConnectionRate( &CurrentChannelTXRate,
                                            &CurrentChannelRXRate,
                                            &MaxChannelTXRate,
                                            &MaxChannelRXRate,
                                            instance );

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
            {
                fprintf(fp, "Current channel Tx rate :%lx\n",CurrentChannelTXRate);
                fprintf(fp, "Current channel Rx rate :%lx\n",CurrentChannelRXRate);
                fprintf(fp, "Maximum Tx rate :%lx\n",MaxChannelTXRate);
                fprintf(fp, "Maximum Rx rate : %lx\n",MaxChannelRXRate);
            }
        }
   }

   if (fp)
       tfclose(fp);
}

/*
 * Name:   doSetAutoconnect
 *
 * Purpose:  Perform the tests that call the SetAutoconnect() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSetAutoconnect( void )
{
    BYTE  appCurTestCaseStepNum = 0;
    ULONG setting=0;
    FILE  *fp=NULL;
    ULONG nRet;

    fp = tfopen("../../wds/test/results/setautoconnect.txt","w");

    if (fp)
    {
        while (appCurTestCaseStepNum < MAX_SET_AUTOCONNECT_TESTCASE_NUM  )
        {
            fprintf(fp,"\n\nTest Case %d / %d : %s\n",
                       (appCurTestCaseStepNum + 1),
                       MAX_SET_AUTOCONNECT_TESTCASE_NUM ,
                       SetAutoconnectTestCases[appCurTestCaseStepNum].desc);

            setting = SetAutoconnectTestCases[appCurTestCaseStepNum].Setting;
            setting = SetAutoconnectTestCases[appCurTestCaseStepNum].Setting;

            fprintf(fp, " Parameters Set\n");
            fprintf(fp, "Auto Connect Setting: %lu\n",setting);

            nRet = SetAutoconnect(setting);

            /* Display result code and text */
            doprintreason (fp, nRet);
            appCurTestCaseStepNum++;
        }
    }

    if (fp)
        tfclose(fp);
}

/*
 * Name:  doSetDefaultProfile
 *
 * Purpose:  Perform the tests that call theSetDefaultProfile() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSetDefaultProfile( void )
{
    BYTE  tCaseNum = 0;
    ULONG nRet;
    ULONG profileType;
    ULONG PDPType;
    ULONG IPAddress;
    ULONG PrimaryDNS;
    ULONG SecondaryDNS;
    ULONG Authentication;
    CHAR  Name[nMaxStrLen];
    CHAR  APNName[nMaxStrLen];
    CHAR  Username[nMaxStrLen];
    CHAR  Password[nMaxStrLen];
    FILE  *fp = NULL;

    fp = tfopen("../../wds/test/results/setdefaultprofile.txt", "w");

    if(fp)
    {
         while (tCaseNum < MAX_SET_DEFAULT_PROFILE_TESTCASE_NUM)
         {
             profileType    = SetDefaultProfileTestCases[tCaseNum].profileType;
             PDPType        = SetDefaultProfileTestCases[tCaseNum].PDPType;
             IPAddress      = SetDefaultProfileTestCases[tCaseNum].IPAddress;
             PrimaryDNS     = SetDefaultProfileTestCases[tCaseNum].\
                              PrimaryDNS;
             SecondaryDNS   = SetDefaultProfileTestCases[tCaseNum].\
                              SecondaryDNS;
             Authentication = SetDefaultProfileTestCases[tCaseNum].\
                              Authentication;
             strcpy(Name,     SetDefaultProfileTestCases[tCaseNum].Name);
             strcpy(APNName,  SetDefaultProfileTestCases[tCaseNum].APNName);
             strcpy(Username, SetDefaultProfileTestCases[tCaseNum].Username);
             strcpy(Password, SetDefaultProfileTestCases[tCaseNum].Password);

             fprintf(fp, "\n\nTest Case %d / %d : %s\n",
                        (tCaseNum + 1),
                        MAX_SET_DEFAULT_PROFILE_TESTCASE_NUM,
                        SetDefaultProfileTestCases[tCaseNum].desc);
             fprintf(fp, "Parameter Set:\n");
             fprintf(fp, "profileType :%ld\n", profileType);
             fprintf(fp, "PDPType :%ld\n", PDPType);
             fprintf(fp, "IPAddress :%ld\n", IPAddress);
             fprintf(fp, "PrimaryDNS :%ld\n", PrimaryDNS);
             fprintf(fp, "SecondaryDNS :%ld\n", SecondaryDNS);
             fprintf(fp, "Authentication :%ld\n", Authentication);
             fprintf(fp, "Name :%s\n", Name);
             fprintf(fp, "APNName :%s\n", APNName);
             fprintf(fp, "Username :%s\n", Username);
             fprintf(fp, "Password :%s\n", Password);

             nRet = SetDefaultProfile(profileType,
                                      &PDPType,
                                      &IPAddress,
                                      &PrimaryDNS,
                                      &SecondaryDNS,
                                      &Authentication,
                                      &Name[0],
                                      &APNName[0],
                                      &Username[0],
                                      &Password[0]);

             /* Display result code and text */
             doprintreason (fp, nRet);

             tCaseNum++;
         }
    }

    if (fp)
        tfclose(fp);
}

/*
 * Name: doSetDefaultProfileLTE
 *
 * Purpose:  Perform the tests that call the GetAutoconnect() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSetDefaultProfileLTE( void )
{
    BYTE   tCaseNum = 0;
    ULONG  nRet;
    ULONG  profileType;
    ULONG  PDPType;
    ULONG  IPAddressv4;
    ULONG  PrimaryDNSv4;
    ULONG  SecondaryDNSv4;
    USHORT IPAddressv6[8];
    USHORT PrimaryDNSv6;
    USHORT SecondaryDNSv6;
    ULONG  Authentication;
    CHAR   Name[nMaxStrLen];
    CHAR   APNName[nMaxStrLen];
    CHAR   Username[nMaxStrLen];
    CHAR   Password[nMaxStrLen];
    USHORT idx;
    FILE   *fp = NULL;

    fp = tfopen("../../wds/test/results/setdefaultprofilelte.txt", "w");

    if(fp)
    {
         while (tCaseNum < MAX_SET_DEFAULT_PROFILELTE_TESTCASE_NUM)
         {
             profileType    = SetDefaultProfileLTETestCases[tCaseNum].\
                              profileType;
             PDPType        = SetDefaultProfileLTETestCases[tCaseNum].\
                              PDPType;
             IPAddressv4    = SetDefaultProfileLTETestCases[tCaseNum].\
                              IPAddressv4;
             PrimaryDNSv4   = SetDefaultProfileLTETestCases[tCaseNum].\
                              PrimaryDNSv4;
             SecondaryDNSv4 = SetDefaultProfileLTETestCases[tCaseNum].\
                              SecondaryDNSv4;

             memcpy(IPAddressv6,SetDefaultProfileLTETestCases[tCaseNum].\
                 IPAddressv6,sizeof(SetDefaultProfileLTETestCases[tCaseNum].\
                 IPAddressv6) );

             PrimaryDNSv6   = SetDefaultProfileLTETestCases[tCaseNum].\
                              PrimaryDNSv6;
             SecondaryDNSv6 = SetDefaultProfileLTETestCases[tCaseNum].\
                              SecondaryDNSv6;
             Authentication = SetDefaultProfileLTETestCases[tCaseNum].\
                              Authentication;
             strcpy(Name,     SetDefaultProfileLTETestCases[tCaseNum].Name);
             strcpy(APNName,  SetDefaultProfileLTETestCases[tCaseNum].APNName);
             strcpy(Username, SetDefaultProfileLTETestCases[tCaseNum].Username);
             strcpy(Password, SetDefaultProfileLTETestCases[tCaseNum].Password);

             fprintf(fp, "\n\nTest Case %d / %d : %s\n",
                        (tCaseNum + 1),
                        MAX_SET_DEFAULT_PROFILELTE_TESTCASE_NUM,
                        SetDefaultProfileLTETestCases[tCaseNum].desc);

             fprintf(fp, "Parameter Set:\n");
             fprintf(fp, "profileType :%ld\n", profileType);
             fprintf(fp, "PDPType :%ld\n", PDPType);
             fprintf(fp, "IPAddressv4 :%ld\n", IPAddressv4);
             fprintf(fp, "PrimaryDNSv4 :%ld\n", PrimaryDNSv4);
             fprintf(fp, "SecondaryDNSv4 :%ld\n", SecondaryDNSv4);

             for (idx = 0; idx < 8; idx++)
                fprintf(fp, "IP V6 Address is: %hx\n", IPAddressv6[idx] );

             fprintf(fp, "PrimaryDNSv6 :%d\n", PrimaryDNSv6);
             fprintf(fp, "SecondaryDNSv6 :%d\n", SecondaryDNSv6);
             fprintf(fp, "Authentication :%ld\n", Authentication);
             fprintf(fp, "Name :%s\n", Name);
             fprintf(fp, "APNName :%s\n", APNName);
             fprintf(fp, "Username :%s\n", Username);
             fprintf(fp, "Password :%s\n", Password);

             nRet = SetDefaultProfileLTE(profileType,
                                         &PDPType,
                                         &IPAddressv4,
                                         &PrimaryDNSv4,
                                         &SecondaryDNSv4,
                                         IPAddressv6,
                                         &PrimaryDNSv6,
                                         &SecondaryDNSv6,
                                         &Authentication,
                                         &Name[0],
                                         &APNName[0],
                                         &Username[0],
                                         &Password[0]);

             /* Display result code and text */
             doprintreason (fp, nRet);

             tCaseNum++;
         }
    }

    if (fp)
        tfclose(fp);
}

/*
 * Name:    doSetMobileIP
 *
 * Purpose:  Perform the tests that call the SetMobileIP() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSetMobileIP( void )
{
    BYTE   appCurTestCaseStepNum = 0;
    ULONG  mode;
    FILE   *fp=NULL;

    fp = tfopen("../../wds/test/results/setmobileip.txt","w");

    if (fp)
    {
        while (appCurTestCaseStepNum < MAX_SET_MOBILE_IP_TESTCASE_NUM)
        {
            fprintf(fp,"\n\nTest Case %d / %d : %s\n",
                       (appCurTestCaseStepNum + 1),
                       MAX_SET_MOBILE_IP_TESTCASE_NUM,
                       SetMobileIPTestCases[appCurTestCaseStepNum].desc);

            mode = SetMobileIPTestCases[appCurTestCaseStepNum].mode;
            ULONG nRet = SetMobileIP( mode );

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
            {
                fprintf(fp, "Set Mobile IP is %lx\n",mode);
            }

            appCurTestCaseStepNum++;
        }
   }

   if (fp)
       tfclose(fp);
}
/*
 * Name:     doGetMobileIP
 *
 * Purpose:  Perform the tests that call the GetMobileIP() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doGetMobileIP( void )
{
    BYTE   appCurTestCaseStepNum = 0;
    ULONG  mode=0;
    FILE   *fp=NULL;

    fp = tfopen("../../wds/test/results/Getmobileip.txt","w");

    if (fp)
    {
        while (appCurTestCaseStepNum < MAX_GET_MOBILE_IP_TESTCASE_NUM)
        {
            fprintf(fp,"\n\nTest Case %d / %d : %s\n",
                   (appCurTestCaseStepNum + 1),
                   MAX_GET_MOBILE_IP_TESTCASE_NUM,
                   GetMobileIPTestCases[appCurTestCaseStepNum].desc);

            ULONG nRet = GetMobileIP( &mode );

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
            {
                fprintf(fp, "Get Mobile IP is %lx\n",mode);
            }

            appCurTestCaseStepNum++;
        }
   }

   if (fp)
       tfclose(fp);
}

/*
 * Name:     GetMobileIPProfile
 *
 * Purpose:  Perform the tests that call the doGetMobileIPProfile() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doGetMobileIPProfile( void )
{
    BYTE appCurTestCaseStepNum = 0;
    BYTE  index;
    BYTE  enabled = 0;
    ULONG address = 0;
    ULONG primaryHA = 0;
    ULONG secondaryHA = 0;
    BYTE  revTunneling = 0;
    BYTE  naiSize;
    CHAR  nAI[nMaxStrLen]="0";
    ULONG hASPI = 0;
    ULONG aAASPI = 0;
    ULONG hAState = 0;
    ULONG aAAState = 0;
    FILE  *fp=NULL;

    fp = tfopen("../../wds/test/results/getmobileipprofile.txt","w");

    if (fp)
    {
        while (appCurTestCaseStepNum < MAX_GET_MOBILE_IP_PROFILE_TESTCASE_NUM )
        {
            index = GetMobileIPProfileTestCases
                                      [appCurTestCaseStepNum].index;
            naiSize = GetMobileIPProfileTestCases
                                      [appCurTestCaseStepNum].naiSize;
            fprintf(fp,"\n\nTest Case %d / %d : %s\n",
                      (appCurTestCaseStepNum + 1),
                      MAX_GET_MOBILE_IP_PROFILE_TESTCASE_NUM,
                      GetMobileIPProfileTestCases[appCurTestCaseStepNum].desc);

            fprintf(fp,"Parameter Set:\n");
            fprintf(fp,"index :%d\n",index);
            fprintf(fp,"enabled :%d\n",enabled);

            ULONG nRet = GetMobileIPProfile( index,
                                             &enabled,
                                             &address,
                                             &primaryHA,
                                             &secondaryHA,
                                             &revTunneling,
                                             naiSize,
                                             nAI,
                                             &hASPI,
                                             &aAASPI,
                                             &hAState,
                                             &aAAState );

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
            {
                fprintf(fp, "Enabled %d\n",  enabled);
                fprintf(fp, "Ip Address: %lx\n",address);
                fprintf(fp, "Primary Home address: %lx\n", primaryHA);
                fprintf(fp, "Seconday Home Address:%lx\n", secondaryHA);
                fprintf(fp, "Rev Tunneling: %d\n", revTunneling);
                fprintf(fp, "HASPI: %lx\n", hASPI);
                fprintf(fp, "NAI: %s\n", nAI);
                fprintf(fp, "AASPI: %lx\n", aAASPI);
                fprintf(fp, "HA State: %lx\n", hAState);
                fprintf(fp, "AAA State: %lx\n", aAAState );

            }

            appCurTestCaseStepNum++;
        }
   }

   if (fp)
       tfclose(fp);
}

/*
 * Name:     doGetLastMobileIPError
 *
 * Purpose:  Perform the tests that call the GetLastMobileIPError() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */

void doGetLastMobileIPError( void )
{
    BYTE   appCurTestCaseStepNum = 0;
    ULONG  Error=0;
    FILE   *fp=NULL;

    fp = tfopen("../../wds/test/results/Getlastmobileiperror.txt","w");

    if (fp)
    {
        while (appCurTestCaseStepNum < MAX_GET_LAST_MOBILE_IP_ERROR_TESTCASE_NUM)
        {
            fprintf(fp,"\n\nTest Case %d / %d : %s\n",
                 (appCurTestCaseStepNum + 1),
                  MAX_GET_LAST_MOBILE_IP_ERROR_TESTCASE_NUM,
                  GetLastMobileIPErrorTestCases[appCurTestCaseStepNum].desc);

            ULONG nRet = GetLastMobileIPError( &Error );

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet )
            {
                fprintf(fp, "Last Mobile IP Error is %lx\n",Error);
            }

            appCurTestCaseStepNum++;
        }
   }

   if (fp)
       tfclose(fp);
}

/*
 * Name:     doSLQSGetRuntimeSettings
 *
 * Purpose:  Perform the tests that call the SLQSGetRuntimeSettings() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetRuntimeSettings( void )
{
    BYTE                        tCaseNum = 0, idx = 0;
    BYTE                        qmiInst = 0xFF, sessType = 0xFF;
    ULONG                       nRet;
    ULONG                       ipv4, ipv4gw, ipv4nm;
    FILE                        *fp = NULL;
    struct WdsRunTimeSettings lRunTimeSettings;

    fp = tfopen("../../wds/test/results/slqsgetruntimesettings.txt", "w");

    if (fp)
    {
        while (tCaseNum < MAX_SLQS_GET_RUNTIME_SETTINGS_TESTCASE_NUM)
        {
            fprintf(fp,
                    "\nTest Case %d / %d : %s\n",
                    (tCaseNum + 1),
                    MAX_SLQS_GET_RUNTIME_SETTINGS_TESTCASE_NUM,
                    SLQSGetRuntimeSettingsTestCases[tCaseNum].desc);

            /* Loop through QMI instance in the sequence - v4, v6, v4v6 */

            qmiInst  = tCaseNum/3;
            sessType = tCaseNum%3;

            fprintf( fp,
                     "\nQMI Instance: %x, Session Type: %x \n",
                     qmiInst,
                     sessType );

            /* Initialize session ID to NULL */
            lRunTimeSettings.v4sessionId = NULL;
            lRunTimeSettings.v6sessionId = NULL;

            /* IPv4 */
            if ( 0 == sessType )
            {
                if( ZERO_SESSION_ID != s4[qmiInst].v4sessionId )
                {
                    lRunTimeSettings.v4sessionId = &s4[qmiInst].v4sessionId;
                }
                lRunTimeSettings.v6sessionId = NULL;
            }

            /* IPv6 */
            if ( 1 == sessType )
            {
                lRunTimeSettings.v4sessionId = NULL;
                if( ZERO_SESSION_ID != s6[qmiInst].v6sessionId )
                {
                    lRunTimeSettings.v6sessionId = &s6[qmiInst].v6sessionId;
                }
            }

            /* IPv4v6 */
            if ( 2 == sessType )
            {
                if( ZERO_SESSION_ID != s46[qmiInst].v4sessionId )
                {
                    lRunTimeSettings.v4sessionId = &s46[qmiInst].v4sessionId;
                }
                if( ZERO_SESSION_ID != s46[qmiInst].v6sessionId )
                {
                    lRunTimeSettings.v6sessionId = &s46[qmiInst].v6sessionId;
                }
            }

            /* Intialize the structure to be sent to API */
            /* Intialize Profile Name */
            lRunTimeSettings.rts.pProfileName   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pProfileName;

            /* Intialize PDP Type */
            lRunTimeSettings.rts.pPDPType   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pPDPType;

            /* Intialize APN Name */
            lRunTimeSettings.rts.pAPNName   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pAPNName;

            /* Intialize IPV4 Primary DNS */
            lRunTimeSettings.rts.pPrimaryDNSV4   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pPrimaryDNSV4;

            /* Intialize IPV4 Secondary DNS */
            lRunTimeSettings.rts.pSecondaryDNSV4   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pSecondaryDNSV4;

            /* Intialize UMTS Granted QOS */
            lRunTimeSettings.rts.pUMTSGrantedQoS   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pUMTSGrantedQos;

            /* Intialize GPRS Granted QOS */
            lRunTimeSettings.rts.pGPRSGrantedQoS   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pGPRSGrantedQos;

            /* Intialize User name */
            lRunTimeSettings.rts.pUsername   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pUsername;

            /* Intialize Authentication */
            lRunTimeSettings.rts.pAuthentication   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pAuthentication;

            /* Intialize IPV4 address */
            lRunTimeSettings.rts.pIPAddressV4   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pIPAddressV4;

            /* Intialize Profile Id */
            lRunTimeSettings.rts.pProfileID   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pProfileID;

            /* Intialize IPV4 Gateway address */
            lRunTimeSettings.rts.pGWAddressV4   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pGWAddressV4;

            /* Intialize IPV4 Subnet Mask */
            lRunTimeSettings.rts.pSubnetMaskV4   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pSubnetMaskV4;

            /* Intialize PCSF address */
            lRunTimeSettings.rts.pPCSCFAddrPCO   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pPCSCFAddrPCO;

            /* Intialize IPV4 Server address list */
            lRunTimeSettings.rts.pServerAddrList =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pServerAddrList;

            /* Intialize FQDN address structure */
            lRunTimeSettings.rts.pPCSCFFQDNAddrList =
                 SLQSGetRuntimeSettingsTestCases[tCaseNum].pPCSCFFQDNAddrList;

            /* Intialize IPV6 Primary DNS */
            lRunTimeSettings.rts.pPrimaryDNSV6   =
                 SLQSGetRuntimeSettingsTestCases[tCaseNum].pPrimDNSV6;

            /* Intialize IPV6 Secondary DNS */
            lRunTimeSettings.rts.pSecondaryDNSV6 =
                 SLQSGetRuntimeSettingsTestCases[tCaseNum].pSecondDNSV6;

            /* Intialize Mtu */
            lRunTimeSettings.rts.pMtu =
                 SLQSGetRuntimeSettingsTestCases[tCaseNum].pMtu;

            /* Intialize Domain list */
            lRunTimeSettings.rts.pDomainList =
                SLQSGetRuntimeSettingsTestCases[tCaseNum].pDomainList;

            /* Intialize IPFamily Prefrence */
            lRunTimeSettings.rts.pIPFamilyPreference   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pIPFamilyPreference;

            /* Intialize IMCN Flag */
            lRunTimeSettings.rts.pIMCNflag   =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pIMCNflag;

            /* Intialize Technology */
            lRunTimeSettings.rts.pTechnology =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pTechnology;

            /* Intialize IPV6 address */
            lRunTimeSettings.rts.pIPV6AddrInfo =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pIPV6AddrInfo;

            /* Intialize the IPV6 Gateway address */
            lRunTimeSettings.rts.pIPV6GWAddrInfo =
                  SLQSGetRuntimeSettingsTestCases[tCaseNum].pIPV6GWAddrInfo;

            /* call the API, get the values */
            nRet = SLQSGetRuntimeSettings( &lRunTimeSettings );

            /* Display result code and text */
            doprintreason (fp, nRet);

            if ( !nRet )
            {
                fprintf(fp, "SLQSGetRuntimeSettings Successful\n");

                fprintf(fp, "ProfileName    : %s\n",
                             lRunTimeSettings.rts.pProfileName);

                fprintf(fp, "PDPType        : %lx\n",
                             *(lRunTimeSettings.rts.pPDPType) );

                fprintf(fp, "APNName        : %s\n",
                             lRunTimeSettings.rts.pAPNName);

                fprintf(fp, "PrimaryDNSV4   : %lx\n",
                             *(lRunTimeSettings.rts.pPrimaryDNSV4) );

                fprintf(fp, "SecondaryDNSV4 : %lx\n",
                             *(lRunTimeSettings.rts.pSecondaryDNSV4) );

                fprintf(fp, "UMTS Granted QoS Parameters\n");
                fprintf(fp, "\ttrafficClass       : %d\n",
                            lRunTimeSettings.rts.pUMTSGrantedQoS->trafficClass);
                fprintf(fp, "\tmaxUplinkBitrate   : %lx\n",
                            lRunTimeSettings.rts.pUMTSGrantedQoS->
                                             maxUplinkBitrate);
                fprintf(fp, "\tmaxDownlinkBitrate : %lx\n",
                            lRunTimeSettings.rts.pUMTSGrantedQoS->
                                             maxDownlinkBitrate);
                fprintf(fp, "\tgrntUplinkBitrate  : %lx\n",
                            lRunTimeSettings.rts.pUMTSGrantedQoS->
                                             grntUplinkBitrate);
                fprintf(fp, "\tgrntDownlinkBitrate: %lx\n",
                            lRunTimeSettings.rts.pUMTSGrantedQoS->
                                             grntDownlinkBitrate);
                fprintf(fp, "\tqosDeliveryOrder   : %d\n",
                            lRunTimeSettings.rts.pUMTSGrantedQoS->qosDeliveryOrder);
                fprintf(fp, "\tmaxSDUSize         : %lx\n",
                            lRunTimeSettings.rts.pUMTSGrantedQoS->maxSDUSize);
                fprintf(fp, "\tsduErrorRatio      : %d\n",
                            lRunTimeSettings.rts.pUMTSGrantedQoS->sduErrorRatio);
                fprintf(fp, "\tresBerRatio        : %d\n",
                            lRunTimeSettings.rts.pUMTSGrantedQoS->resBerRatio);
                fprintf(fp, "\tdeliveryErrSDU     : %d\n",
                            lRunTimeSettings.rts.pUMTSGrantedQoS->deliveryErrSDU);
                fprintf(fp, "\ttransferDelay      : %lx\n",
                            lRunTimeSettings.rts.pUMTSGrantedQoS->transferDelay);
                fprintf(fp, "\ttrafficPriority    : %lx\n",
                            lRunTimeSettings.rts.pUMTSGrantedQoS->trafficPriority);

                fprintf(fp, "GPRS Granted QoS Parameters\n");
                fprintf(fp, "\tprecedenceClass     : %lx\n",
                            lRunTimeSettings.rts.pGPRSGrantedQoS->precedenceClass);
                fprintf(fp, "\tdelayClass          : %lx\n",
                            lRunTimeSettings.rts.pGPRSGrantedQoS->delayClass);
                fprintf(fp, "\treliabilityClass    : %lx\n",
                            lRunTimeSettings.rts.pGPRSGrantedQoS->reliabilityClass);
                fprintf(fp, "\tpeakThroughputClass : %lx\n",
                            lRunTimeSettings.rts.pGPRSGrantedQoS->
                                             peakThroughputClass);
                fprintf(fp, "\tmeanThroughputClass : %lx\n",
                            lRunTimeSettings.rts.pGPRSGrantedQoS->
                                             meanThroughputClass);

                fprintf(fp, "Username       : %s\n",
                            lRunTimeSettings.rts.pUsername);
                fprintf(fp, "Authentication : %lx\n",
                            *(lRunTimeSettings.rts.pAuthentication) );

                ipv4 = *(lRunTimeSettings.rts.pIPAddressV4);
                fprintf( fp, "IPAddressV4    : %d.%d.%d.%d\n",
                         (unsigned int)(ipv4 >> 24) & 0xFF,
                         (unsigned int)(ipv4 >> 16) & 0xFF,
                         (unsigned int)(ipv4 >> 8) & 0xFF,
                         (unsigned int)ipv4 & 0xFF );

                fprintf(fp, "Profile Identifier Parameters \n");
                fprintf(fp, "\tProfile Type : %d\n",
                            lRunTimeSettings.rts.pProfileID->profileType);
                fprintf(fp, "\tProfile ID   : %d\n",
                            lRunTimeSettings.rts.pProfileID->profileIndex);

                ipv4gw = *(lRunTimeSettings.rts.pGWAddressV4);
                fprintf( fp, "GWAddressV4    : %d.%d.%d.%d\n",
                         (unsigned int)(ipv4gw >> 24) & 0xFF,
                         (unsigned int)(ipv4gw >> 16) & 0xFF,
                         (unsigned int)(ipv4gw >> 8) & 0xFF,
                         (unsigned int)ipv4gw & 0xFF );
                ipv4nm = *(lRunTimeSettings.rts.pSubnetMaskV4);
                fprintf( fp, "SubnetMaskV4    : %d.%d.%d.%d\n",
                         (unsigned int)(ipv4nm >> 24) & 0xFF,
                         (unsigned int)(ipv4nm >> 16) & 0xFF,
                         (unsigned int)(ipv4nm >> 8) & 0xFF,
                         (unsigned int)ipv4nm & 0xFF );
                fprintf(fp, "PCSCFAddrPCO   : %d\n",
                            *(lRunTimeSettings.rts.pPCSCFAddrPCO) );

                fprintf(fp, "IPV4 Server AddressList \n");
                fprintf(fp, "\tPCSCF Server AddressList Count %d\n",
                            lRunTimeSettings.rts.pServerAddrList->numInstances);
                for ( idx = 0; idx < ServerAddrList.numInstances; idx++ )
                    fprintf(fp, "\tPCSCF Server Address[%d] %lx\n",
                                idx,
                                lRunTimeSettings.rts.pServerAddrList->
                                pscsfIPv4Addr[idx] );

                fprintf(fp, "PCSCFFQDNAddressList \n");
                fprintf(fp, "\tPCSCFFQDNAddressList Count %d\n",
                            lRunTimeSettings.rts.pPCSCFFQDNAddrList->numInstances);
                for ( idx = 0;
                      idx < lRunTimeSettings.rts.pPCSCFFQDNAddrList->numInstances ;
                      idx++ )
                {
                    fprintf(fp, "\tPCSCFFQDNAddressLength[%d]  %d\n",
                                idx,
                                lRunTimeSettings.rts.pPCSCFFQDNAddrList->
                                pcsfFQDNAddress[idx].fqdnLen );
                    fprintf(fp, "\tPCSCFFQDNAddress[%d]        %s\n",
                                idx,
                                lRunTimeSettings.rts.pPCSCFFQDNAddrList->
                                pcsfFQDNAddress[idx].fqdnAddr );
                }

                fprintf(fp, "Primary DNS V6   : ");
                for (idx = 0; idx < 8; idx++)
                    fprintf(fp, "%hx ", lRunTimeSettings.rts.pPrimaryDNSV6[idx] );
                    fprintf(fp, "\n");

                fprintf(fp, "Secondary DNS V6 : ");
                for (idx = 0; idx < 8; idx++)
                    fprintf(fp, "%hx ", lRunTimeSettings.rts.pSecondaryDNSV6[idx] );
                    fprintf(fp, "\n");

                fprintf(fp, "Mtu            : %lx\n",
                            *(lRunTimeSettings.rts.pMtu) );

                fprintf(fp, "DomainNameList \n");
                fprintf(fp, "\tDomainNameList Count %d\n",
                            lRunTimeSettings.rts.pDomainList->numInstances);

                for ( idx = 0; idx < DomainList.numInstances; idx++ )
                {
                    fprintf(fp, "\tDomainLen[%d]   %d\n",
                                idx,
                                lRunTimeSettings.rts.pDomainList->
                                domain[idx].domainLen);
                    fprintf(fp, "\tDomainName[%d]  %s\n",
                                idx,
                                lRunTimeSettings.rts.pDomainList->
                                domain[idx].domainName);
                }

                fprintf(fp, "IPFamilyPref   : %d\n",
                            *(lRunTimeSettings.rts.pIPFamilyPreference) );

                fprintf(fp, "IMCNflag       : %d\n",
                            *(lRunTimeSettings.rts.pIMCNflag) );

                fprintf(fp, "Technology     : %d\n",
                            *(lRunTimeSettings.rts.pTechnology) );

                fprintf(fp, "IPAddressV6 Information\n");
                fprintf(fp, "\tIPAddressV6 Address   :");
                for (idx = 0; idx < 8; idx++)
                    fprintf(fp, "%hx ",
                                 lRunTimeSettings.rts.pIPV6AddrInfo->
                                 IPAddressV6[idx] );
                fprintf(fp, "\n");
                fprintf(fp, "\tIPAddressV6 Length    : %d\n",
                            lRunTimeSettings.rts.pIPV6AddrInfo->IPV6PrefixLen );

                fprintf(fp, "IPV6 Gateway Address Information\n");
                fprintf(fp, "\tIPV6 Gateway Address   :");
                for (idx = 0; idx < 8; idx++)
                    fprintf(fp, "%hx ",
                                lRunTimeSettings.rts.pIPV6GWAddrInfo->
                                gwAddressV6[idx] );
                fprintf(fp, "\n");
                fprintf(fp, "\tIPV6 Gateway Address Length    : %d\n",
                            lRunTimeSettings.rts.pIPV6GWAddrInfo->gwV6PrefixLen );
            }

            tCaseNum++;
        }
    }

    if (fp)
        tfclose(fp);
}

/*
 * Name:     doSLQSSetProfile
 *
 * Purpose:  Perform the tests that call the SLQSSetProfile() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSSetProfile( void )
{
    BYTE  tCaseNum = 0;
    ULONG nRet;
    ULONG profileType;
    BYTE  profileId;
    ULONG PDPType;
    ULONG IPAddress;
    ULONG PrimaryDNS;
    ULONG SecondaryDNS;
    ULONG Authentication;
    CHAR  Name[nMaxStrLen];
    CHAR  APNName[nMaxStrLen];
    CHAR  Username[nMaxStrLen];
    CHAR  Password[nMaxStrLen];
    FILE  *fp = NULL;

    fp = tfopen("TestReport/slqssetprofile.txt", "w");

    if(fp)
    {
         while (tCaseNum < MAX_SLQS_SET_PROFILE_TESTCASE_NUM)
         {
             profileType    = SLQSSetProfileTestCases[tCaseNum].profileType;
             profileId      = SLQSSetProfileTestCases[tCaseNum].profileId;
             PDPType        = SLQSSetProfileTestCases[tCaseNum].PDPType;
             IPAddress      = SLQSSetProfileTestCases[tCaseNum].IPAddress;
             PrimaryDNS     = SLQSSetProfileTestCases[tCaseNum].\
                              PrimaryDNS;
             SecondaryDNS   = SLQSSetProfileTestCases[tCaseNum].\
                              SecondaryDNS;
             Authentication = SLQSSetProfileTestCases[tCaseNum].\
                              Authentication;
             strcpy(Name,     SLQSSetProfileTestCases[tCaseNum].Name);
             strcpy(APNName,  SLQSSetProfileTestCases[tCaseNum].APNName);
             strcpy(Username, SLQSSetProfileTestCases[tCaseNum].Username);
             strcpy(Password, SLQSSetProfileTestCases[tCaseNum].Password);

             fprintf(fp, "\n\nTest Case %d / %d : %s\n",
                        (tCaseNum + 1),
                        MAX_SLQS_SET_PROFILE_TESTCASE_NUM,
                        SLQSSetProfileTestCases[tCaseNum].desc);
             fprintf(fp, "Parameter Set:\n");
             fprintf(fp, "profileType :%ld\n", profileType);
             fprintf(fp, "profileId :%d\n", profileId);
             fprintf(fp, "PDPType :%ld\n", PDPType);
             fprintf(fp, "IPAddress :%ld\n", IPAddress);
             fprintf(fp, "PrimaryDNS :%ld\n", PrimaryDNS);
             fprintf(fp, "SecondaryDNS :%ld\n", SecondaryDNS);
             fprintf(fp, "Authentication :%ld\n", Authentication);
             fprintf(fp, "Name :%s\n", Name);
             fprintf(fp, "APNName :%s\n", APNName);
             fprintf(fp, "Username :%s\n", Username);
             fprintf(fp, "Password :%s\n", Password);

             nRet = SLQSSetProfile(profileType,
                                   profileId,
                                   &PDPType,
                                   &IPAddress,
                                   &PrimaryDNS,
                                   &SecondaryDNS,
                                   &Authentication,
                                   &Name[0],
                                   &APNName[0],
                                   &Username[0],
                                   &Password[0]);

             /* Display result code and text */
             doprintreason (fp, nRet);

             tCaseNum++;
         }
    }

    if (fp)
        tfclose(fp);
}

/*

 * Name:     doSLQSGetProfile
 *
 * Purpose:  Perform the tests that call the SLQSGetProfile() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetProfile( void )
{
    BYTE  tCaseNum = 0;
    ULONG nRet;
    ULONG profileType;
    BYTE  profileId;
    ULONG PDPType;
    ULONG IPAddress;
    ULONG PrimaryDNS = 0;
    ULONG SecondaryDNS = 0;
    ULONG Authentication;
    BYTE  nameSize = nMaxStrLen-1;
    CHAR  Name[nMaxStrLen];
    BYTE  apnSize = nMaxStrLen-1;
    CHAR  APNName[nMaxStrLen];
    BYTE  userSize = nMaxStrLen-1;
    CHAR  Username[nMaxStrLen];
    WORD  extendedErrorCode = 0;

    FILE  *fp=NULL;

    fp = tfopen("TestReport/slqsgetprofile.txt","w");

    if (fp)
    {
        while(tCaseNum < MAX_SLQS_GET_PROFILE_TESTCASE_NUM)
        {
            profileType = SlqsGetProfileSettingsTestCases[tCaseNum].profileType;
            profileId   = SlqsGetProfileSettingsTestCases[tCaseNum].profileId;

            fprintf(fp,"\n\nTest Case %d / %d : %s\n",
                       (tCaseNum + 1),
                       MAX_SLQS_GET_PROFILE_TESTCASE_NUM,
                       SlqsGetProfileSettingsTestCases[tCaseNum].desc);

            nRet = SLQSGetProfile(profileType,
                                  profileId,
                                  &PDPType,
                                  &IPAddress,
                                  &PrimaryDNS,
                                  &SecondaryDNS,
                                  &Authentication,
                                  nameSize,
                                  &Name[0],
                                  apnSize,
                                  &APNName[0],
                                  userSize,
                                  &Username[0],
                                  &extendedErrorCode );

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
            {
                fprintf(fp,"Details for Profile Id : %d",profileId);
                fprintf(fp,"\nPDPType :%lx",PDPType);
                fprintf(fp,"\nIPAddress :%lx",IPAddress);
                fprintf(fp,"\nPrimaryDNS :%lx",PrimaryDNS);
                fprintf(fp,"\nSecondaryDNS :%lx",SecondaryDNS);
                fprintf(fp,"\nAuthentication :%lx",Authentication);
                fprintf(fp,"\nName : %s",Name);
                fprintf(fp,"\nAPNName :%s",APNName);
                fprintf(fp,"\nUsername :%s",Username);
            }
            else
            {
                fprintf( fp,"Extended Error Code :%d",extendedErrorCode );
                extendedErrorCode = 0;
            }
            tCaseNum++;
        }
    }

    if (fp)
        tfclose(fp);
}

/*
 * Name:     GetAuthenticationPrams
 *
 * Purpose:  Receives authentication parameters input from user.
 *
 * params:   ssdatasession_params
 *           - Structure contains Paramters to start data session
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void GetAuthenticationPrams( struct ssdatasession_params *pin )
{
    CHAR  *pEndOfLine;
    CHAR  selOption[OPTION_LEN];
    ULONG userOption;
    BYTE  len;
    slmemset(username, 0, sizeof(username));
    slmemset(password, 0, sizeof(password));

    while(1)
    {
        fprintf( stderr,
                 "\nPlease select one of the following options or"
                 " press <Enter> to go to main menu:\n"\
                 "1. To enter PAP authentication preference\n"\
                 "2. To enter CHAP authentication\n"\
                 "3. To proceed with-out authentication preference\n"\
                 "Option: " );

        /* Receive the input from the user */
        fgets( selOption, ( OPTION_LEN ), stdin );

        /*If only <ENTER> is pressed quit authentication params */
        if( ENTER_KEY == selOption[0])
        {
            fprintf( stderr,"Enter valid authentication pref!!\n\n");
            return;
        }

        /* If '/n' character is not read, there are more characters in input
         * stream. Clear the input stream.
         */
        pEndOfLine = strchr( selOption, ENTER_KEY );

        if( NULL == pEndOfLine )
        {
            FlushStdinStream();
        }
        len = strlen( selOption );

        /* Convert the option added by user into integer */
        selOption[ len - 1 ] = '\0';
        userOption = atoi( selOption );

        /* If user has entered an invalid input, prompt again */
        if( 2 < strlen(selOption) )
        {
            fprintf(stderr,"Please choose valid option from menu \n");
            continue;
        }
        switch( userOption )
        {
            case ePAP_AUTH_PREF:
                pin->pAuthentication = &PAPPer;
                break;

            case eCHAP_AUTH_PREF:
                pin->pAuthentication = &CHAPPer;
                break;

            case eNO_AUTH_PREF:
                pin->pAuthentication = NULL;
                return;

            default:
               continue;
        }

        while(1)
        {
            /* Take mobile as input from user */
            fprintf( stderr,
                     "Enter user name"
                     " followed by ENTER KEY:\n" );

            fgets( username, 50, stdin );
            /* If '/n' character is not read, there are more characters in input
             * stream. Clear the input stream.
             */
             pEndOfLine = strchr( username, ENTER_KEY );

            /* If only <ENTER> is pressed quit authentication params */
            if( ENTER_KEY == username[0])
            {
               continue;
            }

             if( NULL == pEndOfLine )
             {
                FlushStdinStream();
             }
             else
             {
                 *pEndOfLine = 0;
                 pin->pUsername = username;
                 break;
             }
         }

         while(1)
         {
             /* Take mobile as input from user */
             fprintf( stderr,
                      "Enter password"
                      " followed by ENTER KEY:\n" );

             fgets( password, 50, stdin );
             /* If '/n' character is not read, there are more characters in input
              * stream. Clear the input stream.
              */
             pEndOfLine = strchr( password, ENTER_KEY );
            /* If only <ENTER> is pressed quit authentication params */
            if( ENTER_KEY == password[0])
            {
               continue;
            }

            if( NULL == pEndOfLine )
            {
                FlushStdinStream();
            }
            else
            {
                *pEndOfLine = 0;
                pin->pPassword = password;
                break;
            }
        }
        return;
    }
}

void GetInstanceId( void )
{
    CHAR  *pEndOfLine;
    CHAR  selOption[OPTION_LEN];
    ULONG userOption;
    BYTE  len;
    instanceId = 0;

    while(1)
    {
        fprintf( stderr,
                 "\nPlease enter InstanceID value (  1 to 8) or"
                 " press <Enter> to go to proceed with authentication pref \n"\
                 "Option: " );

        /* Receive the input from the user */
        fgets( selOption, ( OPTION_LEN ), stdin );

        /*If only <ENTER> is pressed quit authentication params */
        if( ENTER_KEY == selOption[0])
        {
            fprintf( stderr,"Default Instance ID - 1 chosen !!\n\n");
            return;
        }

        /* If '/n' character is not read, there are more characters in input
         * stream. Clear the input stream.
         */
        pEndOfLine = strchr( selOption, ENTER_KEY );

        if( NULL == pEndOfLine )
        {
            FlushStdinStream();
        }
        len = strlen( selOption );

        /* Convert the option added by user into integer */
        selOption[ len - 1 ] = '\0';
        userOption = atoi( selOption );

        /* If user has entered an invalid input, prompt again */
        if( 2 < strlen(selOption) )
        {
            fprintf(stderr,"Please choose valid option from menu \n");
            continue;
        }
        instanceId = userOption;
        switch( userOption )
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
                instanceId = userOption -1;
                fprintf(stderr,"\nInstanceID is = %ld\n\n", userOption );
                break;
            default:
                fprintf(stderr,"\nPlesae enter valid InstanceID\n");
               continue;
        }
        return;
    }
}

/*
 * Name:     doSLQSStartStopDataSession_stop
 *
 * Purpose:  Perform the tests that call the SLQSStartStopDataSession() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSStartStopDataSession_stopv4p1(void)
{
    fprintf( stderr, "Parameter Set:\nAction :%s\nTechnology :%s\n"\
             "profID3GPP :%s\nprofID3GPP2 :%s\n",
             "STOP", "", "", "" );

    s4[0].action=0; /* stop session */
    /* Get User Input for instance ID */
    GetInstanceId();
    s4[0].instanceId = instanceId; /* Pick instanceId to stop session */
    ULONG nRet = SLQSStartStopDataSession(&s4[0]);
    doprintreason (stderr, nRet);
}

void doSLQSStartStopDataSession_stopv4p2(void)
{
    fprintf( stderr, "Parameter Set:\nAction :%s\nTechnology :%s\n"\
             "profID3GPP :%s\nprofID3GPP2 :%s\n",
             "STOP", "", "", "" );

    s4[1].action=0; /* stop session */
    /* Get User Input for instance ID */
    GetInstanceId();
    s4[1].instanceId= instanceId; /* Pick instanceId to stop session */
    ULONG nRet = SLQSStartStopDataSession(&s4[1]);
    doprintreason (stderr, nRet);
}

void doSLQSStartStopDataSession_stopv6p3(void)
{
    fprintf( stderr, "Parameter Set:\nAction :%s\nTechnology :%s\n"\
             "profID3GPP :%s\nprofID3GPP2 :%s\n",
             "STOP", "", "", "" );

    s6[0].action=0; /* stop session */
    /* Get User Input for instance ID */
    GetInstanceId();
    s6[0].instanceId = instanceId; /* Pick instanceId to stop session */
    ULONG nRet = SLQSStartStopDataSession(&s6[0]);
    doprintreason (stderr, nRet);
}

void doSLQSStartStopDataSession_stopv6p4(void)
{
    fprintf( stderr, "Parameter Set:\nAction :%s\nTechnology :%s\n"\
             "profID3GPP :%s\nprofID3GPP2 :%s\n",
             "STOP", "", "", "" );

    s6[1].action=0; /* stop session */
    /* Get User Input for instance ID */
    GetInstanceId();
    s6[0].instanceId = instanceId; /* Pick instanceId to stop session */
    ULONG nRet = SLQSStartStopDataSession(&s6[1]);
    doprintreason (stderr, nRet);
}

void doSLQSStartStopDataSession_stopv4v6p5(void)
{
    fprintf( stderr, "Parameter Set:\nAction :%s\nTechnology :%s\n"\
             "profID3GPP :%s\nprofID3GPP2 :%s\n",
             "STOP", "", "", "" );

    s46[0].action=0; /* stop session */
    /* Get User Input for instance ID */
    GetInstanceId();
    s46[0].instanceId = instanceId; /* Pick instanceId to stop session */
    ULONG nRet = SLQSStartStopDataSession(&s46[0]);
    doprintreason (stderr, nRet);
}
void doSLQSStartStopDataSession_stopv4v6p6(void)
{
    fprintf( stderr, "Parameter Set:\nAction :%s\nTechnology :%s\n"\
             "profID3GPP :%s\nprofID3GPP2 :%s\n",
             "STOP", "", "", "" );

    s46[1].action=0; /* stop session */
    /* Get User Input for instance ID */
    GetInstanceId();
    s46[0].instanceId = instanceId; /* Pick instanceId to stop session */
    ULONG nRet = SLQSStartStopDataSession(&s46[1]);
    doprintreason (stderr, nRet);
}

/*
 * Name:     doSLQSStartStopDataSession_start
 *
 * Purpose:  Perform the tests that call the SLQSStartStopDataSession() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSStartStopDataSession_startv4p1(void)
{
    ULONG tech = TECHNOLOGY_3GPP, prof = 3;
    s4[0].action = 1;
    /* Get User Input for instance ID */
    GetInstanceId();
    s4[0].instanceId = instanceId;
    s4[0].pTechnology = NULL;
    s4[0].pProfileId3GPP = &prof;
    s4[0].pProfileId3GPP2 = NULL;
    s4[0].ipfamily = 4;

    fprintf( stderr, "Parameter Set:\nAction :%s\nTechnology :%s\n"\
             "profID3GPP :%lu\nprofID3GPP2 :%s\n",
             "START", (tech == TECHNOLOGY_3GPP? "3GPP":"3GPP2"), prof, "" );

    GetAuthenticationPrams(&s4[0]);
    ULONG nRet = SLQSStartStopDataSession(&s4[0]);
    doprintreason (stderr, nRet);
}

void doSLQSStartStopDataSession_startv4p2(void)
{
    ULONG tech = TECHNOLOGY_3GPP, prof = 1;
    s4[1].action = 1;
    /* Get User Input for instance ID */
    GetInstanceId();
    s4[1].instanceId = instanceId;
    s4[1].pTechnology = &tech;
    s4[1].pProfileId3GPP = &prof;
    s4[1].pProfileId3GPP2 = NULL;
    s4[1].ipfamily = 4;

    fprintf( stderr, "Parameter Set:\nAction :%s\nTechnology :%s\n"\
             "profID3GPP :%lu\nprofID3GPP2 :%s\n",
             "START", (tech == TECHNOLOGY_3GPP? "3GPP":"3GPP2"), prof, "" );

    GetAuthenticationPrams(&s4[1]);
    ULONG nRet = SLQSStartStopDataSession(&s4[1]);
    doprintreason (stderr, nRet);
}

void doSLQSStartStopDataSession_startv6p3(void)
{
    ULONG tech = TECHNOLOGY_3GPP, prof = 1;
    s6[0].action = 1;
    /* Get User Input for instance ID */
    GetInstanceId();
    s6[0].instanceId = instanceId;
    s6[0].pTechnology = &tech;
    s6[0].pProfileId3GPP = &prof;
    s6[0].pProfileId3GPP2 = NULL;
    s6[0].ipfamily = 6;

    fprintf( stderr, "Parameter Set:\nAction :%s\nTechnology :%s\n"\
             "profID3GPP :%lu\nprofID3GPP2 :%s\n",
             "START", (tech == TECHNOLOGY_3GPP? "3GPP":"3GPP2"), prof, "" );

    GetAuthenticationPrams(&s6[0]);
    ULONG nRet = SLQSStartStopDataSession(&s6[0]);
    doprintreason (stderr, nRet);
}

void doSLQSStartStopDataSession_startv6p4(void)
{
    ULONG tech = TECHNOLOGY_3GPP, prof = 2;
    s6[1].action = 1;
    /* Get User Input for instance ID */
    GetInstanceId();
    s6[1].instanceId = instanceId;
    s6[1].pTechnology = &tech;
    s6[1].pProfileId3GPP = &prof;
    s6[1].pProfileId3GPP2 = NULL;
    s6[1].ipfamily = 6;

    fprintf( stderr, "Parameter Set:\nAction :%s\nTechnology :%s\n"\
             "profID3GPP :%lu\nprofID3GPP2 :%s\n",
             "START", (tech == TECHNOLOGY_3GPP? "3GPP":"3GPP2"), prof, "" );

    GetAuthenticationPrams(&s6[1]);
    ULONG nRet = SLQSStartStopDataSession(&s6[1]);
    doprintreason (stderr, nRet);
}
void doSLQSStartStopDataSession_startv4v6p5(void)
{
    ULONG tech = TECHNOLOGY_3GPP, prof = 1;
    s46[0].action = 1;
    /* Get User Input for instance ID */
    GetInstanceId();
    s46[0].instanceId = instanceId;
    s46[0].pTechnology = &tech;
    s46[0].pProfileId3GPP = &prof;
    s46[0].pProfileId3GPP2 = NULL;
    s46[0].ipfamily = 7;

    fprintf( stderr, "Parameter Set:\nAction :%s\nTechnology :%s\n"\
             "profID3GPP :%lu\nprofID3GPP2 :%s\n",
             "START", (tech == TECHNOLOGY_3GPP? "3GPP":"3GPP2"), prof, "" );

    GetAuthenticationPrams(&s46[0]);
    ULONG nRet = SLQSStartStopDataSession(&s46[0]);
    doprintreason (stderr, nRet);
}

void doSLQSStartStopDataSession_startv4v6p6(void)
{
    ULONG tech = TECHNOLOGY_3GPP, prof = 2;
    s46[1].action = 1;
    /* Get User Input for instance ID */
    GetInstanceId();
    s46[1].instanceId = instanceId;
    s46[1].pTechnology = &tech;
    s46[1].pProfileId3GPP = &prof;
    s46[1].pProfileId3GPP2 = NULL;
    s46[1].ipfamily = 7;

    fprintf( stderr, "Parameter Set:\nAction :%s\nTechnology :%s\n"\
             "profID3GPP :%lu\nprofID3GPP2 :%s\n",
             "START", (tech == TECHNOLOGY_3GPP? "3GPP":"3GPP2"), prof, "" );

    GetAuthenticationPrams(&s46[1]);
    ULONG nRet = SLQSStartStopDataSession(&s46[1]);
    doprintreason (stderr, nRet);
}

/*
 * Name:     doSLQSStartDataSessionOnSprint
 *
 * Purpose:  Perform the tests to start data session on Sprint since Sprint does not accept
                  any profile from the QMI command. This call the SLQSStartStopDataSession() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSStartDataSessionOnSprint(void)
{
    tech = TECHNOLOGY_3GPP2;

    s4[0].action = 1;
    s4[0].instanceId = 1;
    s4[0].pTechnology = &tech;
    s4[0].pProfileId3GPP2 = NULL;
    s4[0].ipfamily = 4;

    fprintf( stderr, "Parameter Set:\nAction :%s\nTechnology :%s\n"\
             "profID3GPP :%s\nprofID3GPP2 :%s\n",
             "START", "3GPP2", "", "" );

    GetAuthenticationPrams(&s4[0]);
    ULONG nRet = SLQSStartStopDataSession(&s4[0]);
    doprintreason (stderr, nRet);
}

/*
 * Name:     doSLQSStopDataSessionOnSprint
 *
 * Purpose:  Perform the tests to stop data session on Sprint since Sprint does not accept
                  any profile from the QMI command. This call the SLQSStartStopDataSession() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSStopDataSessionOnSprint(void)
{
    tech = TECHNOLOGY_3GPP2;

    s4[0].action = 0;
    s4[0].instanceId = 1;
    s4[0].pTechnology = &tech;
    s4[0].pProfileId3GPP = NULL;
    s4[0].pProfileId3GPP2 = NULL;
    s4[0].ipfamily = 4;

    fprintf( stderr, "Parameter Set:\nAction :%s\nTechnology :%s\n"\
             "profID3GPP :%s\nprofID3GPP2 :%s\n",
             "START", "3GPP2", "", "" );

    GetAuthenticationPrams(&s4[0]);
    ULONG nRet = SLQSStartStopDataSession(&s4[0]);
    doprintreason (stderr, nRet);
}

/*
 * Name:     doSetMobileIPParameters
 *
 * Purpose:  Perform the tests that call the SetMobileIPParameters() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSetMobileIPParameters( void )
{
    BYTE  tCaseNum = 0;
    CHAR  SPC[7];
    ULONG Mode;
    BYTE  RetryLimit;
    BYTE  RetryInterval;
    BYTE  ReRegPeriod;
    BYTE  ReRegTraffic;
    BYTE  HAAuthenticator;
    BYTE  HA2002bis;
    FILE  *fp=NULL;

    fp = tfopen("../../wds/test/results/setmobileipparameters.txt","w");

    if (fp)
    {
        while (tCaseNum < MAX_SET_MOBILE_IP_PARAM_TESTCASE_NUM)
        {
            strcpy(SPC,SetMobileIPParamTestCases[tCaseNum].SPC);
            Mode = SetMobileIPParamTestCases[tCaseNum].Mode;
            RetryLimit = SetMobileIPParamTestCases[tCaseNum].RetryLimit;
            RetryInterval = SetMobileIPParamTestCases[tCaseNum].RetryInterval;
            ReRegPeriod = SetMobileIPParamTestCases[tCaseNum].ReRegPeriod;
            ReRegTraffic = SetMobileIPParamTestCases[tCaseNum].ReRegTraffic;
            HAAuthenticator = SetMobileIPParamTestCases[tCaseNum].
                                                             HAAuthenticator;
            HA2002bis = SetMobileIPParamTestCases[tCaseNum].HA2002bis;

            fprintf(fp,"\n\nTest Case %d / %d : %s\n",
                       (tCaseNum + 1),
                       MAX_SET_MOBILE_IP_PARAM_TESTCASE_NUM,
                       SetMobileIPParamTestCases[tCaseNum].desc);
            fprintf(fp, "Parameter Set:\n");
            fprintf(fp, "SPC:%s\n",SPC );
            fprintf(fp, "Mode:%lu\n",Mode);
            fprintf(fp, "RetryLimit:%d\n",RetryLimit);
            fprintf(fp, "RetryInterval:%d\n",RetryInterval);
            fprintf(fp, "ReRegPeriod:%d\n",ReRegPeriod);
            fprintf(fp, "ReRegTraffic:%d\n",ReRegTraffic);
            fprintf(fp, "HAAuthenticator:%d\n",HAAuthenticator);
            fprintf(fp, "HA2002bis:%d\n",HA2002bis);

            ULONG nRet = SetMobileIPParameters( SPC,
                                                &Mode,
                                                &RetryLimit,
                                                &RetryInterval,
                                                &ReRegPeriod,
                                                &ReRegTraffic,
                                                &HAAuthenticator,
                                                &HA2002bis );

            fprintf(fp, "Return Code: %lu\n", nRet);
            doprintreason( fp, nRet );

            tCaseNum++;
        }
   }

   if (fp)
       tfclose(fp);
}

/*
 * Name:     doSLQSDeleteProfile
 *
 * Purpose:  Perform the tests that call the SLQSDeleteProfile() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSDeleteProfile( void )
{
    struct SLQSDeleteProfileParams deleteProfile;
    WORD                           extendedErrorCode = 0;
    BYTE                           tCaseNum = 0;
    ULONG                          nRet;
    FILE                           *fp = NULL;

    fp = tfopen( "../../wds/test/results/slqsdeleteprofile.txt", "w" );

    /* Define the test cases to be executed */
    const SLQSDeleteProfileTestCase_t SLQSDeleteProfileTestCase
          [MAX_SLQS_DELETE_PROFILE_TESTCASE_NUM] =
    {
        { eQCWWAN_ERR_QMI_EXTENDED_INTERNAL, 1, 0x18, &deleteProfile,
                                           "Invalid Profile Id" },
        { eQCWWAN_ERR_NONE,                  2, 0x03, &deleteProfile,
                                           "All Valid - Valid profile Id" },
        { eQCWWAN_ERR_NONE,                  3, 0x04, &deleteProfile,
                                           "All Valid - Valid profile Id" }
    };

    if (fp)
    {
        while( tCaseNum < MAX_SLQS_DELETE_PROFILE_TESTCASE_NUM )
        {
            extendedErrorCode = 0;
            fprintf( fp, "\n\nTest Case %d / %d : %s\n",
                       (tCaseNum + 1),
                       MAX_SLQS_DELETE_PROFILE_TESTCASE_NUM,
                       SLQSDeleteProfileTestCase[tCaseNum].desc );

            fprintf( fp, "Parameter Set:\n" );
            SLQSDeleteProfileTestCase[tCaseNum].pDeleteProfile->profileIndex =
            SLQSDeleteProfileTestCase[tCaseNum].profileIndex;

            SLQSDeleteProfileTestCase[tCaseNum].pDeleteProfile->profileType =0;
            fprintf( fp, "Profile Type : 0x%x\n",
            SLQSDeleteProfileTestCase[tCaseNum].pDeleteProfile->profileType );
            fprintf( fp, "Profile Index : 0x%x\n",
            SLQSDeleteProfileTestCase[tCaseNum].pDeleteProfile->profileIndex );

            /* Call the API */
            nRet = SLQSDeleteProfile (
                          SLQSDeleteProfileTestCase[tCaseNum].pDeleteProfile,
                          &extendedErrorCode );

            /* Display result code and text */
            doprintreason ( fp, nRet );
            fprintf( fp, "Extended Error Code : %d\n", extendedErrorCode );
            tCaseNum++;
        }
    }
    if (fp)
        tfclose(fp);
}

/*
 * Name:     doSLQSCreate3GPPProfile
 *
 * Purpose:  Perform the tests that call the SLQSCreateProfile() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSCreate3GPPProfile( void )
{
    BYTE   tCaseNum  = 0;
    ULONG  nRet;
    FILE   *fp = NULL;
    BYTE   profileType  = 0;
    BYTE   profileIndex = 0;
    USHORT extErrorCode = 0;

    struct CreateProfileIn  lCreateProfileIn;
    struct CreateProfileOut lWdsCreateProfileOut;
    lWdsCreateProfileOut.pProfileType   = &profileType;
    lWdsCreateProfileOut.pProfileIndex  = &profileIndex;
    lWdsCreateProfileOut.pExtErrorCode  = &extErrorCode;

    fp = tfopen("../../wds/test/results/slqscreateprofile.txt", "w");
    if (fp)
    {
        while (tCaseNum < MAX_SLQS_CREATE_PROFILE_TESTCASE_NUM)
        {
            fprintf(fp,
                    "\nTest Case %d / %d : %s\n",
                    (tCaseNum + 1),
                    MAX_SLQS_CREATE_PROFILE_TESTCASE_NUM,
                    SLQSCreateProfileTestCases[tCaseNum].desc);

            /*Intialize the structure to be sent to API */
            /*Intialize ProfileID: reserved for future use */
            lCreateProfileIn.pProfileID =
                   SLQSCreateProfileTestCases[tCaseNum].pProfileID;;

            /* Intialize Profile Type*/
            lCreateProfileIn.pProfileType =
                   SLQSCreateProfileTestCases[tCaseNum].pProfileType;

            /* Intialize pProfilename */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pProfilename =
                   SLQSCreateProfileTestCases[tCaseNum].pProfilename;

            /* Intialize PDPType*/
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pPDPtype =
                   SLQSCreateProfileTestCases[tCaseNum].pPDPtype;

            /* Intialize PdpHdrCompType */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pPdpHdrCompType =
                   SLQSCreateProfileTestCases[tCaseNum].pPdpHdrCompType;

            lCreateProfileIn.curProfile.SlqsProfile3GPP.pPdpDataCompType =
                   SLQSCreateProfileTestCases[tCaseNum].pPdpDataCompType;

            /* Intialize APNName */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pAPNName =
                   SLQSCreateProfileTestCases[tCaseNum].pAPNName;

            /* Intialize priDNSIPv4AddPref */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pPriDNSIPv4AddPref =
                   SLQSCreateProfileTestCases[tCaseNum].pPriDNSIPv4AddPref;

            /* Intialize secDNSIPv4AddPref */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pSecDNSIPv4AddPref =
                   SLQSCreateProfileTestCases[tCaseNum].pSecDNSIPv4AddPref;

            /* Intialize UMTSReqQoS */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pUMTSReqQoS =
                   SLQSCreateProfileTestCases[tCaseNum].pUMTSReqQoS;

            /* Intialize UMTSMinQoS */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pUMTSMinQoS =
                   SLQSCreateProfileTestCases[tCaseNum].pUMTSMinQoS;

            /* Intialize GPRSRequestedQos */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pGPRSRequestedQos =
                   SLQSCreateProfileTestCases[tCaseNum].pGPRSRequestedQos;

            /* Intialize GPRSMinimumQoS */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pGPRSMinimumQoS =
                   SLQSCreateProfileTestCases[tCaseNum].pGPRSMinimumQoS;

            /* Intialize Username */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pUsername =
                   SLQSCreateProfileTestCases[tCaseNum].pUsername;

            /* Intialize Password */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pPassword =
                   SLQSCreateProfileTestCases[tCaseNum].pPassword;

            lCreateProfileIn.curProfile.SlqsProfile3GPP.pAuthenticationPref
                   = SLQSCreateProfileTestCases[tCaseNum].pAuthenticationPref;

            /* Intialize IPv4AddrPref */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pIPv4AddrPref =
                   SLQSCreateProfileTestCases[tCaseNum].pIPv4AddrPref;

            /* Intialize pcscfAddrUsingPCO */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pPcscfAddrUsingPCO =
                   SLQSCreateProfileTestCases[tCaseNum].pPcscfAddrUsingPCO;

            /* Intialize pdpAccessConFlag */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pPdpAccessConFlag =
                   SLQSCreateProfileTestCases[tCaseNum].pPdpAccessConFlag;

            /* Intialize pcscfAddrUsingDhcp */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pPcscfAddrUsingDhcp =
                   SLQSCreateProfileTestCases[tCaseNum].pPcscfAddrUsingDhcp;

            /* Intialize imCnFlag */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pImCnFlag =
                   SLQSCreateProfileTestCases[tCaseNum].pImCnFlag;

            /* Intialize imCnFlag */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pTFTID1Params =
                   SLQSCreateProfileTestCases[tCaseNum].pTFTID1Params;

            /* Intialize imCnFlag */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pTFTID2Params=
                   SLQSCreateProfileTestCases[tCaseNum].pTFTID2Params;

            /* Intialize pdpContext */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pPdpContext =
                   SLQSCreateProfileTestCases[tCaseNum].pPdpContext;

            /* Intialize secondaryFlag */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pSecondaryFlag =
                   SLQSCreateProfileTestCases[tCaseNum].pSecondaryFlag;

            /* Intialize primaryID */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pPrimaryID =
                   SLQSCreateProfileTestCases[tCaseNum].pPrimaryID;

            /* Intialize pIPv6AddPref */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pIPv6AddPref =
                   SLQSCreateProfileTestCases[tCaseNum].pIPv6AddPref;

            /* Intialize pUMTSReqQoSSigInd */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pUMTSReqQoSSigInd =
                   SLQSCreateProfileTestCases[tCaseNum].pUMTSReqQoSSigInd;

            /* Intialize pUMTSMinQoSSigInd */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pUMTSMinQosSigInd =
                   SLQSCreateProfileTestCases[tCaseNum].pUMTSMinQosSigInd;

            /* Intialize pPriDNSIPv6addpref */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pPriDNSIPv6addpref =
                   SLQSCreateProfileTestCases[tCaseNum].pPriDNSIPv6addpref;

            /* Intialize pSecDNSIPv6addpref */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pSecDNSIPv6addpref =
                   SLQSCreateProfileTestCases[tCaseNum].pSecDNSIPv6addpref;

            /* Intialize pAddrAllocPref */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pAddrAllocPref =
                   SLQSCreateProfileTestCases[tCaseNum].pAddrAllocPref;

            /* Intialize pAddrAllocPref */
            lCreateProfileIn.curProfile.SlqsProfile3GPP.pQosClassID =
                   SLQSCreateProfileTestCases[tCaseNum].pQosClassID;

            /* Call the API, get the values */
            nRet = SLQSCreateProfile(&lCreateProfileIn,
                                     &lWdsCreateProfileOut);

            /* Print Reason */
            doprintreason (fp, nRet);
            if ( !nRet )
            {
                fprintf(fp, "SLQSCreateProfile Successful\n");

                fprintf(fp, "profileType: %d\n", profileType);

                fprintf(fp, "ProfileIndex: %d\n", profileIndex);
            }
            if( EXTENDED_ERR == nRet)
            {

                fprintf(fp, "Extended Error Code: %d\n", extErrorCode);
            }
            tCaseNum++;
        }
    }
    if (fp)
    tfclose(fp);
}

/*
 * Name:     doSLQSCreate3GPP2Profile
 *
 * Purpose:  Perform the tests that call the SLQSCreateProfile() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSCreate3GPP2Profile(void)
{
    BYTE   tCaseNum  = 0;
    ULONG  nRet;
    FILE   *fp = NULL;
    BYTE   profileType  = 0;
    BYTE   profileIndex = 0;
    USHORT extErrorCode = 0;

    struct CreateProfileIn  lCreateProfileIn;
    struct CreateProfileOut lWdsCreateProfileOut;
    lWdsCreateProfileOut.pProfileType   = &profileType;
    lWdsCreateProfileOut.pProfileIndex  = &profileIndex;
    lWdsCreateProfileOut.pExtErrorCode  = &extErrorCode;

    fp = tfopen("../../wds/test/results/slqscreateprofile.txt", "w");
    if (fp)
    {
        while (tCaseNum < MAX_SLQS_CREATE_PROFILE2_TESTCASE_NUM)
        {
            fprintf(fp,
                    "\nTest Case %d / %d : %s\n",
                    (tCaseNum + 1),
                    MAX_SLQS_CREATE_PROFILE2_TESTCASE_NUM,
                    SLQSCreateProfile2TestCases[tCaseNum].desc);

            /*Intialize the structure to be sent to API */
            /*Intialize ProfileID: reserved for future use */
             lCreateProfileIn.pProfileID =
                    SLQSCreateProfile2TestCases[tCaseNum].pProfileID;;

            /* Intialize Profile Type*/
            lCreateProfileIn.pProfileType =
                   SLQSCreateProfile2TestCases[tCaseNum].pProfileType;

            /* Intialize pProfilename */
            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pNegoDnsSrvrPref =
                   SLQSCreateProfile2TestCases[tCaseNum].pNegoDnsSrvrPref;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pPppSessCloseTimerDO =
                   SLQSCreateProfile2TestCases[tCaseNum].pPppSessCloseTimerDO;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pPppSessCloseTimer1x =
                   SLQSCreateProfile2TestCases[tCaseNum].pPppSessCloseTimer1x;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pAllowLinger =
                   SLQSCreateProfile2TestCases[tCaseNum].pAllowLinger;

            lCreateProfileIn.curProfile. SlqsProfile3GPP2.pLcpAckTimeout =
                   SLQSCreateProfile2TestCases[tCaseNum].pLcpAckTimeout;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pIpcpAckTimeout =
                   SLQSCreateProfile2TestCases[tCaseNum].pIpcpAckTimeout;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pAuthTimeout =
                   SLQSCreateProfile2TestCases[tCaseNum].pAuthTimeout;


            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pLcpCreqRetryCount =
                   SLQSCreateProfile2TestCases[tCaseNum].pLcpCreqRetryCount;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pIpcpCreqRetryCount =
                   SLQSCreateProfile2TestCases[tCaseNum].pIpcpCreqRetryCount;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pAuthRetryCount =
                   SLQSCreateProfile2TestCases[tCaseNum].pAuthRetryCount;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pAuthProtocol =
                   SLQSCreateProfile2TestCases[tCaseNum].pAuthProtocol;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pUserId =
                   SLQSCreateProfile2TestCases[tCaseNum].pUserId;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pAuthPassword =
                   SLQSCreateProfile2TestCases[tCaseNum].pAuthPassword;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pDataRate =
                   SLQSCreateProfile2TestCases[tCaseNum].pDataRate;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pAppType =
              SLQSCreateProfile2TestCases[tCaseNum].pAppType;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pDataMode =
              SLQSCreateProfile2TestCases[tCaseNum].pDataMode;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pAppPriority =
                   SLQSCreateProfile2TestCases[tCaseNum].pAppPriority;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pApnString =
                   SLQSCreateProfile2TestCases[tCaseNum].pApnString;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pPdnType =
              SLQSCreateProfile2TestCases[tCaseNum].pPdnType;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pIsPcscfAddressNedded =
                   SLQSCreateProfile2TestCases[tCaseNum].pIsPcscfAddressNedded;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pPrimaryV4DnsAddress =
                   SLQSCreateProfile2TestCases[tCaseNum].pPrimaryV4DnsAddress;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pSecondaryV4DnsAddress
                   = SLQSCreateProfile2TestCases[tCaseNum].pSecondaryV4DnsAddress;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pPriV6DnsAddress =
                   SLQSCreateProfile2TestCases[tCaseNum].pPriV6DnsAddress;

            lCreateProfileIn.curProfile.SlqsProfile3GPP2.pSecV6DnsAddress =
                   SLQSCreateProfile2TestCases[tCaseNum].pSecV6DnsAddress;

            /* call the API, get the values */
            nRet = SLQSCreateProfile(&lCreateProfileIn,
                                     &lWdsCreateProfileOut);
            doprintreason (fp, nRet);
            if ( !nRet )
            {
                fprintf(fp, "SLQSCreateProfile Successful\n");

                fprintf(fp, "profileType: %d\n", profileType);

                fprintf(fp, "ProfileIndex: %d\n", profileIndex);
            }
            if( EXTENDED_ERR == nRet )
            {
                fprintf(fp, "Extended Error Code: %d\n", extErrorCode);
            }
            tCaseNum++;
        }
    }
    if (fp)
    tfclose(fp);
}

/*
 * Name:     doSLQSCreateProfile
 *
 * Purpose:  Executes doSLQSCreate3GPPProfile and doSLQSCreate3GPP2Profile
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSCreateProfile( void )
{
    doSLQSCreate3GPPProfile();
    doSLQSCreate3GPP2Profile();
}

/*
 * Name:     doSLQSAutoConnect
 *
 * Purpose:  Perform the tests that call the SLQSAutoConnect() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSAutoConnect( void )
{
    BYTE   tCaseNum = 0;
    ULONG  nRet;
    FILE   *fp = NULL;
    CHAR   selOption[OPTION_LEN];
    CHAR   *pEndOfLine = NULL;
    struct slqsautoconnect lSlqsautoconnect;

    fp = tfopen("../../wds/test/results/slqsautoconnectsettings.txt", "w");
    if (fp)
    {
        while(1)
        {
            fprintf( stderr,
                     "\nPlease select one of the following options or"
                     " press <Enter> to go to main menu:\n"\
                     "1.Autoconnect Enabled \n"\
                     "2.Autoconnect Disabled\n"\
                     "3.Autoconnect Paused\n"\
                     "4.Autoconnect roaming Always allowed\n"\
                     "5.Autoconnect while in home service area only\n"\
                     "6.GetAutoconnect Settings\n"
                     "Option: " );

            /* Receive the input from the user */
            fgets( selOption, ( OPTION_LEN ), stdin );

            /* If '/n' character is not read, there are more characters in input
             * stream. Clear the input stream.
             */
            pEndOfLine = strchr( selOption, ENTER_KEY );
            if( NULL == pEndOfLine )
            {
                FlushStdinStream();
            }
            /* If user has entered an invalid input, prompt again */
            if( 2 < strlen(selOption) )
            {
                fprintf(stderr,"Please choose valid option from menu \n");
                continue;
            }
            switch( selOption[0] )
            {
                case eACENABLED:
                case eACDISABLED:
                case eACPAUSED:
                case eACRALWAYSALLOWED:
                case eACRHOMENWONLY:
                    tCaseNum = selOption[0] - 1 + 0x30 ;
                    break;

                case eEXIT_APP:
                    return;
                    break;

                default:
                    break;
            }
            tCaseNum = atoi(selOption) - 1;
            fprintf(fp,
                    "\nTest Case %d / %d : %s\n",
                    (tCaseNum + 1),
                    MAX_SLQS_AC_SETTINGS_TESTCASE_NUM,
                    SLQSAutoConnectTestCases[tCaseNum].desc);

            /* Intialize the structure to be sent to API */
            /* Intialize action */
            lSlqsautoconnect.action =
                SLQSAutoConnectTestCases[tCaseNum].action;

            /* Intialize acsetting */
            lSlqsautoconnect.acsetting =
                SLQSAutoConnectTestCases[tCaseNum].acsetting;

            /* Intialize acroamsetting */
            lSlqsautoconnect.acroamsetting =
                SLQSAutoConnectTestCases[tCaseNum].acroamsetting;

            /* call the API, get the values */
            nRet = SLQSAutoConnect (&lSlqsautoconnect);

            doprintreason (fp, nRet);
            if ( !nRet )
            {
                fprintf(fp, "SLQSAutoConnect Successful\n");

                if ( 0 == lSlqsautoconnect.action )
                {
                    fprintf(fp, "acsetting: %d\n",
                                lSlqsautoconnect.acsetting);

                    fprintf(fp, "acroamsetting: %d\n",
                                lSlqsautoconnect.acroamsetting);
                }
            }
            tCaseNum++;
        }
    }
    if (fp)
    tfclose(fp);
}

/*
 * Name:     doSLQSModify3GPPProfile
 *
 * Purpose:  Perform the tests that call the SLQSModifyProfile() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSModify3GPPProfile( void )
{
    BYTE   tCaseNum  = 0;
    ULONG  nRet;
    FILE   *fp = NULL;
    USHORT extErrorCode = 0;

    struct ModifyProfileIn  lModifyProfileIn;
    struct ModifyProfileOut lModifyProfileOut;
    lModifyProfileOut.pExtErrorCode = &extErrorCode;

    fp = tfopen("../../wds/test/results/slqsModifyprofile.txt", "w");
    if (fp)
    {
        while (tCaseNum < MAX_SLQS_MODIFY_PROFILE_TESTCASE_NUM)
        {
            fprintf(fp,
                    "\nTest Case %d / %d : %s\n",
                    (tCaseNum + 1),
                    MAX_SLQS_MODIFY_PROFILE_TESTCASE_NUM,
                    SLQSModifyProfileTestCases[tCaseNum].desc);

            /*Intialize the structure to be sent to API */
            /*Intialize ProfileID: reserved for future use */
            lModifyProfileIn.pProfileID =
                   SLQSModifyProfileTestCases[tCaseNum].pProfileID;;

            /* Intialize Profile Type*/
            lModifyProfileIn.pProfileType =
                   SLQSModifyProfileTestCases[tCaseNum].pProfileType;

            /* Intialize pProfilename */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pProfilename =
                   SLQSModifyProfileTestCases[tCaseNum].pProfilename;

            /* Intialize PDPType*/
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pPDPtype =
                   SLQSModifyProfileTestCases[tCaseNum].pPDPtype;

            /* Intialize PdpHdrCompType */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pPdpHdrCompType =
                   SLQSModifyProfileTestCases[tCaseNum].pPdpHdrCompType;

            lModifyProfileIn.curProfile.SlqsProfile3GPP.pPdpDataCompType =
                   SLQSModifyProfileTestCases[tCaseNum].pPdpDataCompType;

            /* Intialize APNName */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pAPNName =
                   SLQSModifyProfileTestCases[tCaseNum].pAPNName;

            /* Intialize priDNSIPv4AddPref */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pPriDNSIPv4AddPref =
                   SLQSModifyProfileTestCases[tCaseNum].pPriDNSIPv4AddPref;

            /* Intialize secDNSIPv4AddPref */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pSecDNSIPv4AddPref =
                   SLQSModifyProfileTestCases[tCaseNum].pSecDNSIPv4AddPref;

            /* Intialize UMTSReqQoS */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pUMTSReqQoS =
                   SLQSModifyProfileTestCases[tCaseNum].pUMTSReqQoS;

            /* Intialize UMTSMinQoS */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pUMTSMinQoS =
                   SLQSModifyProfileTestCases[tCaseNum].pUMTSMinQoS;

            /* Intialize GPRSRequestedQos */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pGPRSRequestedQos =
                   SLQSModifyProfileTestCases[tCaseNum].pGPRSRequestedQos;

            /* Intialize GPRSMinimumQoS */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pGPRSMinimumQoS =
                   SLQSModifyProfileTestCases[tCaseNum].pGPRSMinimumQoS;

            /* Intialize Username */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pUsername =
                   SLQSModifyProfileTestCases[tCaseNum].pUsername;

            /* Intialize Password */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pPassword =
                   SLQSModifyProfileTestCases[tCaseNum].pPassword;

            lModifyProfileIn.curProfile.SlqsProfile3GPP.pAuthenticationPref
                   = SLQSModifyProfileTestCases[tCaseNum].pAuthenticationPref;

            /* Intialize IPv4AddrPref */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pIPv4AddrPref =
                   SLQSModifyProfileTestCases[tCaseNum].pIPv4AddrPref;

            /* Intialize pcscfAddrUsingPCO */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pPcscfAddrUsingPCO =
                   SLQSModifyProfileTestCases[tCaseNum].pPcscfAddrUsingPCO;

            /* Intialize pdpAccessConFlag */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pPdpAccessConFlag =
                   SLQSModifyProfileTestCases[tCaseNum].pPdpAccessConFlag;

            /* Intialize pcscfAddrUsingDhcp */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pPcscfAddrUsingDhcp =
                   SLQSModifyProfileTestCases[tCaseNum].pPcscfAddrUsingDhcp;

            /* Intialize imCnFlag */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pImCnFlag =
                   SLQSModifyProfileTestCases[tCaseNum].pImCnFlag;

            /* Intialize imCnFlag */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pTFTID1Params =
                   SLQSModifyProfileTestCases[tCaseNum].pTFTID1Params;

            /* Intialize imCnFlag */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pTFTID2Params=
                   SLQSModifyProfileTestCases[tCaseNum].pTFTID2Params;

            /* Intialize pdpContext */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pPdpContext =
                   SLQSModifyProfileTestCases[tCaseNum].pPdpContext;

            /* Intialize secondaryFlag */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pSecondaryFlag =
                   SLQSModifyProfileTestCases[tCaseNum].pSecondaryFlag;

            /* Intialize primaryID */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pPrimaryID =
                   SLQSModifyProfileTestCases[tCaseNum].pPrimaryID;

            /* Intialize pIPv6AddPref */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pIPv6AddPref =
                   SLQSModifyProfileTestCases[tCaseNum].pIPv6AddPref;

            /* Intialize pUMTSReqQoSSigInd */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pUMTSReqQoSSigInd =
                   SLQSModifyProfileTestCases[tCaseNum].pUMTSReqQoSSigInd;

            /* Intialize pUMTSMinQoSSigInd */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pUMTSMinQosSigInd =
                   SLQSModifyProfileTestCases[tCaseNum].pUMTSMinQosSigInd;

            /* Intialize pPriDNSIPv6addpref */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pPriDNSIPv6addpref =
                   SLQSModifyProfileTestCases[tCaseNum].pPriDNSIPv6addpref;

            /* Intialize pSecDNSIPv6addpref */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pSecDNSIPv6addpref =
                   SLQSModifyProfileTestCases[tCaseNum].pSecDNSIPv6addpref;

            /* Intialize pAddrAllocPref */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pAddrAllocPref =
                   SLQSModifyProfileTestCases[tCaseNum].pAddrAllocPref;

            /* Intialize pAddrAllocPref */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pQosClassID =
                   SLQSModifyProfileTestCases[tCaseNum].pQosClassID;

            /* Intialize pAPNClass */
            lModifyProfileIn.curProfile.SlqsProfile3GPP.pAPNClass =
                   SLQSModifyProfileTestCases[tCaseNum].pAPNClass3GPP;

            /* Call the API, get the values */
            nRet = SLQSModifyProfile( &lModifyProfileIn,
                                      &lModifyProfileOut );

            /* Print Reason */
            doprintreason (fp, nRet);
            if ( !nRet )
            {
                fprintf(fp, "SLQSModifyProfile Successful\n");
            }
            if( EXTENDED_ERR == nRet )
            {
                fprintf(fp, "Extended Error Code: %d\n", extErrorCode);
            }
            tCaseNum++;
        }
    }
    if (fp)
    tfclose(fp);
}

/*
 * Name:     doSLQSModify3GPP2Profile
 *
 * Purpose:  Perform the tests that call the SLQSModifyProfile() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSModify3GPP2Profile(void)
{
    BYTE   tCaseNum  = 0;
    ULONG  nRet;
    FILE   *fp = NULL;
    USHORT extErrorCode = 0;

    struct ModifyProfileIn  lModifyProfileIn;
    struct ModifyProfileOut lModifyProfileOut;
    lModifyProfileOut.pExtErrorCode = &extErrorCode;

    fp = tfopen("../../wds/test/results/slqsModifyprofile.txt", "w");
    if (fp)
    {
        while (tCaseNum < MAX_SLQS_MODIFY_PROFILE2_TESTCASE_NUM)
        {
            fprintf(fp,
                    "\nTest Case %d / %d : %s\n",
                    (tCaseNum + 1),
                    MAX_SLQS_MODIFY_PROFILE2_TESTCASE_NUM,
                    SLQSModifyProfile2TestCases[tCaseNum].desc);

            /*Intialize the structure to be sent to API */
            /*Intialize ProfileID: reserved for future use */
             lModifyProfileIn.pProfileID =
                    SLQSModifyProfile2TestCases[tCaseNum].pProfileID;;

            /* Intialize Profile Type*/
            lModifyProfileIn.pProfileType =
                   SLQSModifyProfile2TestCases[tCaseNum].pProfileType;

            /* Intialize pProfilename */
            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pNegoDnsSrvrPref =
                   SLQSModifyProfile2TestCases[tCaseNum].pNegoDnsSrvrPref;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pPppSessCloseTimerDO =
                   SLQSModifyProfile2TestCases[tCaseNum].pPppSessCloseTimerDO;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pPppSessCloseTimer1x =
                   SLQSModifyProfile2TestCases[tCaseNum].pPppSessCloseTimer1x;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pAllowLinger =
                   SLQSModifyProfile2TestCases[tCaseNum].pAllowLinger;

            lModifyProfileIn.curProfile. SlqsProfile3GPP2.pLcpAckTimeout =
                   SLQSModifyProfile2TestCases[tCaseNum].pLcpAckTimeout;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pIpcpAckTimeout =
                   SLQSModifyProfile2TestCases[tCaseNum].pIpcpAckTimeout;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pAuthTimeout =
                   SLQSModifyProfile2TestCases[tCaseNum].pAuthTimeout;


            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pLcpCreqRetryCount =
                   SLQSModifyProfile2TestCases[tCaseNum].pLcpCreqRetryCount;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pIpcpCreqRetryCount =
                   SLQSModifyProfile2TestCases[tCaseNum].pIpcpCreqRetryCount;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pAuthRetryCount =
                   SLQSModifyProfile2TestCases[tCaseNum].pAuthRetryCount;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pAuthProtocol =
                   SLQSModifyProfile2TestCases[tCaseNum].pAuthProtocol;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pUserId =
                   SLQSModifyProfile2TestCases[tCaseNum].pUserId;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pAuthPassword =
                   SLQSModifyProfile2TestCases[tCaseNum].pAuthPassword;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pDataRate =
                   SLQSModifyProfile2TestCases[tCaseNum].pDataRate;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pAppType =
              SLQSModifyProfile2TestCases[tCaseNum].pAppType;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pDataMode =
              SLQSModifyProfile2TestCases[tCaseNum].pDataMode;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pAppPriority =
                   SLQSModifyProfile2TestCases[tCaseNum].pAppPriority;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pApnString =
                   SLQSModifyProfile2TestCases[tCaseNum].pApnString;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pPdnType =
              SLQSModifyProfile2TestCases[tCaseNum].pPdnType;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pIsPcscfAddressNedded =
                   SLQSModifyProfile2TestCases[tCaseNum].pIsPcscfAddressNedded;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pPrimaryV4DnsAddress =
                   SLQSModifyProfile2TestCases[tCaseNum].pPrimaryV4DnsAddress;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pSecondaryV4DnsAddress
                   = SLQSModifyProfile2TestCases[tCaseNum].pSecondaryV4DnsAddress;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pPriV6DnsAddress =
                   SLQSModifyProfile2TestCases[tCaseNum].pPriV6DnsAddress;

            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pSecV6DnsAddress =
                   SLQSModifyProfile2TestCases[tCaseNum].pSecV6DnsAddress;

            /* Intialize pAPNClass3GPP2 */
            lModifyProfileIn.curProfile.SlqsProfile3GPP2.pAPNClass3GPP2 =
                   SLQSModifyProfile2TestCases[tCaseNum].pAPNClass3GPP2;

            /* call the API, get the values */
             nRet = SLQSModifyProfile(&lModifyProfileIn,
                                      &lModifyProfileOut );
            doprintreason (fp, nRet);
            if ( !nRet )
            {
                fprintf(fp, "SLQSModifyProfile Successful\n");
            }
            if( EXTENDED_ERR == nRet )
            {
                fprintf(fp, "Extended Error Code: %d\n", extErrorCode);
            }
            tCaseNum++;
        }
    }
    if (fp)
    tfclose(fp);
}

/*
 * Name:     doSLQSModifyProfile
 *
 * Purpose:  Executes doSLQSModify3GPPProfile and doSLQSModify3GPP2Profile
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSModifyProfile( void )
{
    doSLQSModify3GPPProfile();
    doSLQSModify3GPP2Profile();
}

/*
 * Name:     doSLQSSet3GPPConfigItem
 *
 * Purpose:  Perform the tests that call the SLQSSet3GPPConfigItem() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSSet3GPPConfigItem( void )
{
    ULONG              nRet;
    FILE               *fp = NULL;
    slqs3GPPConfigItem swi3gppConfigItem;
    BYTE               counter;

    /* Define the test cases to be executed */
    SLQSSetGet3gppConfigItemTestCase_t SLQSSet3gppConfigTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, NULL, NULL, NULL, NULL,
          "Invalid Test Case - All params NULL"},

        { eQCWWAN_ERR_NONE, 2, &SetLTEAttachProfile, NULL, NULL, NULL,
           "Valid test Case - LTEAttachProfile set" },

        { eQCWWAN_ERR_NONE, 3, NULL, SetProfileList, NULL, NULL,
           "Valid test Case - ProfileList set" },

        { eQCWWAN_ERR_NONE, 4, NULL, NULL, &SetDefaultPDNEnabled, NULL,
           "Valid test Case - Always Connect Default PDN set" },

        { eQCWWAN_ERR_NONE, 5, NULL, NULL, NULL, &SetRelease3GPP,
           "Valid test Case - 3GPP Release Set" },

        { eQCWWAN_ERR_NONE, 6, &SetLTEAttachProfile, SetProfileList,
           &SetDefaultPDNEnabled, &SetRelease3GPP,
           "Valid test Case - All parameters set" },
    };

    fp = tfopen( "../../wds/test/results/slqsset3gppconfigitem.txt", "w" );
    if ( fp )
    {
        BYTE tc     = 0;
        BYTE ncases = sizeof( SLQSSet3gppConfigTestCases ) /
                      sizeof( SLQSSet3gppConfigTestCases[0] );

        while( tc < ncases )
        {
            SLQSSetGet3gppConfigItemTestCase_t *pd = &SLQSSet3gppConfigTestCases[tc];

            fprintf( fp, "\n\nTest Case %d / %d : %s\n",
                     (tc + 1),
                     ncases,
                     pd->desc );

            fprintf( fp, "Parameter Set:\n" );
            IFPRINTF( fp, "LTEAttachProfile : %d\n", pd->pLTEAttachProfile );
            if( pd->pProfileList )
            {
                for( counter = 0; counter <= 4; counter++ )
                {
                    fprintf( fp, "pProfileList[%d] : %d\n",
                             counter,
                             pd->pProfileList[counter] );
                }
            }
            else
            {
                fprintf( fp, "pProfileList : %d\n", 0x0);
            }
            IFPRINTF( fp, "Default PDN enabled : %d\n", pd->pDefaultPDNEnabled );
            IFPRINTF( fp, "3gppRelease : %d\n",         pd->p3gppRelease );

            swi3gppConfigItem.pLTEAttachProfile  = pd->pLTEAttachProfile;
            swi3gppConfigItem.pProfileList       = pd->pProfileList;
            swi3gppConfigItem.pDefaultPDNEnabled = pd->pDefaultPDNEnabled;
            swi3gppConfigItem.p3gppRelease       = pd->p3gppRelease;

            nRet = SLQSSet3GPPConfigItem( &swi3gppConfigItem );

            /* Display result code and text */
            doprintreason( fp, nRet );
            tc++;
        }
    }

    if (fp)
        tfclose(fp);
}

/*
 * Name:     doSLQSGet3GPPConfigItem
 *
 * Purpose:  Perform the tests that call the SLQSGet3GPPConfigItem() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGet3GPPConfigItem( void )
{
    ULONG              nRet;
    FILE               *fp = NULL;
    slqs3GPPConfigItem swi3gppConfigItem;
    BYTE               Counter;

    /* Define the test cases to be executed */
    SLQSSetGet3gppConfigItemTestCase_t SLQSGet3gppConfigTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, NULL, NULL, NULL, NULL,
          "Invalid Test Case - All params NULL"},

        { eQCWWAN_ERR_NONE, 2, &LTEAttachProfile, ProfileList, NULL,
          &Release3GPP, "Valid test Case - One Param NULL" },

        { eQCWWAN_ERR_NONE, 3, &LTEAttachProfile, ProfileList,
          &defaultPDNEnabled, &Release3GPP, "Valid test Case" },
    };

    fp = tfopen( "../../wds/test/results/slqsget3gppconfigitem.txt", "w" );
    if ( fp )
    {
        BYTE tCaseNum     = 0;
        BYTE ncases = sizeof( SLQSGet3gppConfigTestCases ) /
                      sizeof( SLQSGet3gppConfigTestCases[0] );

        while( tCaseNum < ncases )
        {
            SLQSSetGet3gppConfigItemTestCase_t *pd =
                &SLQSGet3gppConfigTestCases[tCaseNum];

            swi3gppConfigItem.pLTEAttachProfile  = pd->pLTEAttachProfile;
            swi3gppConfigItem.pProfileList       = pd->pProfileList;
            swi3gppConfigItem.pDefaultPDNEnabled = pd->pDefaultPDNEnabled;
            swi3gppConfigItem.p3gppRelease       = pd->p3gppRelease;

            fprintf( fp,"\n\nTest Case %d / %d : %s\n",
                     (tCaseNum + 1),
                     ncases,
                     pd->desc );

            nRet = SLQSGet3GPPConfigItem( &swi3gppConfigItem );

            /* Display result code and text */
            doprintreason( fp, nRet );

            if (!nRet)
            {
                fprintf( fp,"Details for ConfigItem \n" );

                IFPRINTF( fp, "\nLTEAttachProfile : %d",
                          swi3gppConfigItem.pLTEAttachProfile );
                if( swi3gppConfigItem.pProfileList )
                {
                    for( Counter = 0; Counter <= 4; Counter++ )
                    {
                        fprintf( fp,"\npProfileList[%d] : %d",
                                 Counter,
                                 swi3gppConfigItem.pProfileList[Counter] );
                    }
                }
                else
                {
                    fprintf( fp,"\npProfileList : %d", 0x0 );
                }
                IFPRINTF( fp, "\nDefault PDN enabled : %d",
                          swi3gppConfigItem.pDefaultPDNEnabled );
                IFPRINTF( fp, "\n3gppRelease : %d",
                          swi3gppConfigItem.p3gppRelease );
            }
            tCaseNum++;
        }
    }

    if (fp)
        tfclose(fp);
}

/*
 * Name:     doSLQSGetProfileSettings3GPP
 *
 * Purpose:  Perform the tests that call the SLQSGetProfileSettings() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetProfileSettings3GPP( void )
{
    ULONG                nRet;
    FILE                 *fp = NULL;
    GetProfileSettingIn  ProfileSettingReq;
    GetProfileSettingOut ProfileSetting;
    struct Profile3GPP   *pProfile;
    WORD                 SourceIPTFTID1[8];
    WORD                 SourceIPTFTID2[8];
    BYTE                 idx                = 0;

    /* Define the test cases to be executed */
    SLQSGetProfileSettings3GPPTestCase_t SLQSGetProfileSettings3GPPTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, -1, -1, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Invalid Test Case - Invalid params" },

        { eQCWWAN_ERR_NONE, 2, profType3GPP, profIndex, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, "Valid test Case - No out parameter" },

        { eQCWWAN_ERR_NONE, 3, profType3GPP, profIndex, profName, &InValprofNameSize, &PdpType, &PDPHDRCompType,
          &PDPDataCompType, ApnName, &ValApnNameSize, &PriDNSIPv4AddPref, &SecDNSIPv4AddPref, &UMTSReqQoS, &UMTSMinQos,
          &GPRSReqQoS, &GPRSminQoS, UserName, &ValUserNameSize, Passwd, &ValPasswdSize, &AuthPref, &IPv4AddPref, &PCSCFAddPCO,
          &PDPAccCtrlFlag, &PCSCFAddDHCP, &IMCNFlag, &TFTID1, &TFTID2, &PDPCon, &SecFlag, &priID,
          IPv6AddrPref, &UMTSReqQoSsigInd, &UMTSMinQoSsigInd, priDNSIPv6AddPref, secDNSIPv6AddPref,
          &addrAllocPref, &QoSCLSID, &ApnDisFlag, &PDNInactivTim, &APNClass, &extErrCode,
          "InValid test Case - All params specified with invalid profile name size" },

        { eQCWWAN_ERR_NONE, 4, profType3GPP, profIndex, profName, &ValprofNameSize, &PdpType, &PDPHDRCompType,
          &PDPDataCompType, ApnName, &InValApnNameSize, &PriDNSIPv4AddPref, &SecDNSIPv4AddPref, &UMTSReqQoS, &UMTSMinQos,
          &GPRSReqQoS, &GPRSminQoS, UserName, &ValUserNameSize, Passwd, &ValPasswdSize, &AuthPref, &IPv4AddPref, &PCSCFAddPCO,
          &PDPAccCtrlFlag, &PCSCFAddDHCP, &IMCNFlag, &TFTID1, &TFTID2, &PDPCon, &SecFlag, &priID,
          IPv6AddrPref, &UMTSReqQoSsigInd, &UMTSMinQoSsigInd, priDNSIPv6AddPref, secDNSIPv6AddPref,
          &addrAllocPref, &QoSCLSID, &ApnDisFlag, &PDNInactivTim, &APNClass, &extErrCode,
          "InValid test Case - All params specified with invalid APN name size" },

        { eQCWWAN_ERR_NONE, 5, profType3GPP, profIndex, profName, &ValprofNameSize, &PdpType, &PDPHDRCompType,
          &PDPDataCompType, ApnName, &ValApnNameSize, &PriDNSIPv4AddPref, &SecDNSIPv4AddPref, &UMTSReqQoS, &UMTSMinQos,
          &GPRSReqQoS, &GPRSminQoS, UserName, &InValUserNameSize, Passwd, &ValPasswdSize, &AuthPref, &IPv4AddPref, &PCSCFAddPCO,
          &PDPAccCtrlFlag, &PCSCFAddDHCP, &IMCNFlag, &TFTID1, &TFTID2, &PDPCon, &SecFlag, &priID,
          IPv6AddrPref, &UMTSReqQoSsigInd, &UMTSMinQoSsigInd, priDNSIPv6AddPref, secDNSIPv6AddPref,
          &addrAllocPref, &QoSCLSID, &ApnDisFlag, &PDNInactivTim, &APNClass, &extErrCode,
          "InValid test Case - All params specified with invalid user name size" },

        { eQCWWAN_ERR_NONE, 6, profType3GPP, profIndex, profName, &ValprofNameSize, &PdpType, &PDPHDRCompType,
          &PDPDataCompType, ApnName, &ValApnNameSize, &PriDNSIPv4AddPref, &SecDNSIPv4AddPref, &UMTSReqQoS, &UMTSMinQos,
          &GPRSReqQoS, &GPRSminQoS, UserName, &ValUserNameSize, Passwd, &InValPasswdSize, &AuthPref, &IPv4AddPref, &PCSCFAddPCO,
          &PDPAccCtrlFlag, &PCSCFAddDHCP, &IMCNFlag, &TFTID1, &TFTID2, &PDPCon, &SecFlag, &priID,
          IPv6AddrPref, &UMTSReqQoSsigInd, &UMTSMinQoSsigInd, priDNSIPv6AddPref, secDNSIPv6AddPref,
          &addrAllocPref, &QoSCLSID, &ApnDisFlag, &PDNInactivTim, &APNClass, &extErrCode,
          "InValid test Case - All params specified with invalid APN name size" },

          { eQCWWAN_ERR_NONE, 7, profType3GPP, profIndex, profName, &ValprofNameSize, &PdpType, &PDPHDRCompType,
          &PDPDataCompType, ApnName, &ValApnNameSize, &PriDNSIPv4AddPref, &SecDNSIPv4AddPref, &UMTSReqQoS, &UMTSMinQos,
          &GPRSReqQoS, &GPRSminQoS, UserName, &ValUserNameSize, Passwd, &ValPasswdSize, &AuthPref, &IPv4AddPref, &PCSCFAddPCO,
          &PDPAccCtrlFlag, &PCSCFAddDHCP, &IMCNFlag, &TFTID1, &TFTID2, &PDPCon, &SecFlag, &priID,
          IPv6AddrPref, &UMTSReqQoSsigInd, &UMTSMinQoSsigInd, priDNSIPv6AddPref, secDNSIPv6AddPref,
          &addrAllocPref, &QoSCLSID, &ApnDisFlag, &PDNInactivTim, &APNClass, &extErrCode,
          "Valid test Case - All params specified with valid size" },
    };

    fp = tfopen( "../../wds/test/results/slqsgetprofilesettings3gpp.txt", "w" );
    if ( fp )
    {
        BYTE tCaseNum = 0;
        BYTE ncases   = sizeof( SLQSGetProfileSettings3GPPTestCases ) /
                        sizeof( SLQSGetProfileSettings3GPPTestCases[0] );

        while( tCaseNum < ncases )
        {
            SLQSGetProfileSettings3GPPTestCase_t *pd =
                &SLQSGetProfileSettings3GPPTestCases[tCaseNum];

            pProfile = &(ProfileSetting.curProfile.SlqsProfile3GPP);

            ProfileSettingReq.ProfileType = pd->ProfileType;
            ProfileSettingReq.ProfileID   = pd->ProfileID;

            pProfile->pProfilename        = pd->pProfilename;
            pProfile->pProfilenameSize    = pd->pProfileNameSize;
            pProfile->pPDPtype            = pd->pPDPtype;
            pProfile->pPdpHdrCompType     = pd->pPdpHdrCompType;
            pProfile->pPdpDataCompType    = pd->pPdpDataCompType;
            pProfile->pAPNName            = pd->pAPNName;
            pProfile->pAPNnameSize        = pd->pAPNNameSize;
            pProfile->pPriDNSIPv4AddPref  = pd->pPriDNSIPv4AddPref;
            pProfile->pSecDNSIPv4AddPref  = pd->pSecDNSIPv4AddPref;
            pProfile->pUMTSReqQoS         = pd->pUMTSReqQoS;
            pProfile->pUMTSMinQoS         = pd->pUMTSMinQoS;
            pProfile->pGPRSRequestedQos   = pd->pGPRSRequestedQos;
            pProfile->pGPRSMinimumQoS     = pd->pGPRSMinimumQoS;
            pProfile->pUsername           = pd->pUsername;
            pProfile->pUsernameSize       = pd->pUserNameSize;
            pProfile->pPassword           = pd->pPassword;
            pProfile->pPasswordSize       = pd->pPasswordSize;
            pProfile->pAuthenticationPref = pd->pAuthenticationPref;
            pProfile->pIPv4AddrPref       = pd->pIPv4AddrPref;
            pProfile->pPcscfAddrUsingPCO  = pd->pPcscfAddrUsingPCO;
            pProfile->pPdpAccessConFlag   = pd->pPdpAccessConFlag;
            pProfile->pPcscfAddrUsingDhcp = pd->pPcscfAddrUsingDhcp;
            pProfile->pImCnFlag           = pd->pImCnFlag;
            pProfile->pTFTID1Params       = pd->pTFTID1Params;
            pProfile->pTFTID2Params       = pd->pTFTID2Params;
            pProfile->pPdpContext         = pd->pPdpContext;
            pProfile->pSecondaryFlag      = pd->pSecondaryFlag;
            pProfile->pPrimaryID          = pd->pPrimaryID;
            pProfile->pIPv6AddPref        = pd->pIPv6AddPref;
            pProfile->pUMTSReqQoSSigInd   = pd->pUMTSReqQoSSigInd;
            pProfile->pUMTSMinQosSigInd   = pd->pUMTSMinQosSigInd;
            pProfile->pPriDNSIPv6addpref  = pd->pPriDNSIPv6addpref;
            pProfile->pSecDNSIPv6addpref  = pd->pSecDNSIPv6addpref;
            pProfile->pAddrAllocPref      = pd->pAddrAllocPref;
            pProfile->pQosClassID         = pd->pQosClassID;
            pProfile->pAPNDisabledFlag    = pd->pAPNDisabledFlag;
            pProfile->pPDNInactivTimeout  = pd->pPDNInactivTimeout;
            pProfile->pAPNClass           = pd->pAPNClass;

            ProfileSetting.pExtErrCode        = pd->pExtErrCode;
            if( pProfile->pTFTID1Params )
            {
                pProfile->pTFTID1Params->pSourceIP = SourceIPTFTID1;
            }
            if( pProfile->pTFTID2Params )
            {
                pProfile->pTFTID2Params->pSourceIP = SourceIPTFTID2;
            }

            fprintf( fp,"\n\nTest Case %d / %d : %s\n",
                     (tCaseNum + 1),
                     ncases,
                     pd->desc );

            fprintf( fp, "Parameter Set:\n" );
            fprintf( fp, "Profile Type : %d\n", pd->ProfileType );
            fprintf( fp, "Profile ID : %d\n",   pd->ProfileID );
            nRet = SLQSGetProfileSettings( &ProfileSettingReq,
                                           &ProfileSetting );

            /* Display result code and text */
            doprintreason( fp, nRet );
            if (!nRet)
            {
                fprintf( fp,"Details for Profile \n" );

                if( pProfile->pProfilename )
                {
                    fprintf( fp, "\nProfile Name                 : %s",      pProfile->pProfilename );
                }
                IFPRINTF( fp, "\nProfile Name Size               : 0x%x",    pProfile->pProfilenameSize );
                IFPRINTF( fp, "\nPDP Type                        : 0x%x",    pProfile->pPDPtype );
                IFPRINTF( fp, "\nPDP Header Compression Type     : 0x%X",    pProfile->pPdpHdrCompType );
                IFPRINTF( fp, "\nPDP data Compression Type       : 0x%X",    pProfile->pPdpDataCompType );
                if( pProfile->pAPNName )
                {
                    fprintf( fp, "\nAPN Name                     : %s",      pProfile->pAPNName );
                }
                IFPRINTF( fp, "\nAPN Name Size                   : 0x%x",    pProfile->pAPNnameSize );
                IFPRINTF( fp, "\nPrimary DNS IPv4 address Pref.  : 0x%lX",   pProfile->pPriDNSIPv4AddPref );
                IFPRINTF( fp, "\nSecondary DNS IPv4 address Pref.: 0x%lX",   pProfile->pSecDNSIPv4AddPref );

                if( pProfile->pUMTSReqQoS )
                {
                    struct UMTSQoS *pUMTSQoS = pProfile->pUMTSReqQoS;

                    fprintf( fp,"\n\n UMTS Requested QoS" );
                    fprintf( fp, "\nTraffic Class                : 0x%X",    pUMTSQoS->trafficClass );
                    fprintf( fp, "\nMaximum Uplink Bit Rate      : 0x%lX",   pUMTSQoS->maxUplinkBitrate );
                    fprintf( fp, "\nMaximum Downlink Bit Rate    : 0x%lX",   pUMTSQoS->maxDownlinkBitrate );
                    fprintf( fp, "\nGuaranteed Uplink Bit Rate   : 0x%lX",   pUMTSQoS->grntUplinkBitrate );
                    fprintf( fp, "\nGuaranteed Downlink Bit Rate : 0x%lX",   pUMTSQoS->grntDownlinkBitrate );
                    fprintf( fp, "\nQoS Delivery Order           : 0x%X",    pUMTSQoS->qosDeliveryOrder );
                    fprintf( fp, "\nMaximum SDU Size             : 0x%lX",   pUMTSQoS->maxSDUSize );
                    fprintf( fp, "\nSDU Error Ratio              : 0x%X",    pUMTSQoS->sduErrorRatio );
                    fprintf( fp, "\nResidual Bit Error Ratio     : 0x%X",    pUMTSQoS->resBerRatio );
                    fprintf( fp, "\nDelivery Erroneous SDU's     : 0x%X",    pUMTSQoS->deliveryErrSDU );
                    fprintf( fp, "\nTransfer Delay               : 0x%lX",   pUMTSQoS->transferDelay );
                    fprintf( fp, "\nTraffic Handling Priority    : 0x%lX\n", pUMTSQoS->trafficPriority );
                }

                if( pProfile->pUMTSMinQoS )
                {
                    struct UMTSQoS *pUMTSQoS = pProfile->pUMTSMinQoS;

                    fprintf( fp,"\n\n UMTS Minimum QoS" );
                    fprintf( fp, "\nTraffic Class                : 0x%X",    pUMTSQoS->trafficClass );
                    fprintf( fp, "\nMaximum Uplink Bit Rate      : 0x%lX",   pUMTSQoS->maxUplinkBitrate );
                    fprintf( fp, "\nMaximum Downlink Bit Rate    : 0x%lX",   pUMTSQoS->maxDownlinkBitrate );
                    fprintf( fp, "\nGuaranteed Uplink Bit Rate   : 0x%lX",   pUMTSQoS->grntUplinkBitrate );
                    fprintf( fp, "\nGuaranteed Downlink Bit Rate : 0x%lX",   pUMTSQoS->grntDownlinkBitrate );
                    fprintf( fp, "\nQoS Delivery Order           : 0x%X",    pUMTSQoS->qosDeliveryOrder );
                    fprintf( fp, "\nMaximum SDU Size             : 0x%lX",   pUMTSQoS->maxSDUSize );
                    fprintf( fp, "\nSDU Error Ratio              : 0x%X",    pUMTSQoS->sduErrorRatio );
                    fprintf( fp, "\nResidual Bit Error Ratio     : 0x%X",    pUMTSQoS->resBerRatio );
                    fprintf( fp, "\nDelivery Erroneous SDU's     : 0x%X",    pUMTSQoS->deliveryErrSDU );
                    fprintf( fp, "\nTransfer Delay               : 0x%lX",   pUMTSQoS->transferDelay );
                    fprintf( fp, "\nTraffic Handling Priority    : 0x%lX\n", pUMTSQoS->trafficPriority );
                }

                if( pProfile->pGPRSRequestedQos )
                {
                    struct GPRSRequestedQoS *pGPRSQoS = pProfile->pGPRSRequestedQos;

                    fprintf( fp,"\n\n GPRS Requested QoS" );
                    fprintf( fp, "\nPrecedence Class             : 0x%lX",   pGPRSQoS->precedenceClass );
                    fprintf( fp, "\nDelay Class                  : 0x%lX",   pGPRSQoS->delayClass );
                    fprintf( fp, "\nReliability Class            : 0x%lX",   pGPRSQoS->reliabilityClass );
                    fprintf( fp, "\nPeak Throughput Class        : 0x%lX",   pGPRSQoS->peakThroughputClass );
                    fprintf( fp, "\nMean Throughput Class        : 0x%lX\n", pGPRSQoS->meanThroughputClass );
                }

                if( pProfile->pGPRSMinimumQoS )
                {
                    struct GPRSRequestedQoS *pGPRSQoS = pProfile->pGPRSMinimumQoS;

                    fprintf( fp,"\n\n GPRS Minimum QoS" );
                    fprintf( fp, "\nPrecedence Class             : 0x%lX",   pGPRSQoS->precedenceClass );
                    fprintf( fp, "\nDelay Class                  : 0x%lX",   pGPRSQoS->delayClass );
                    fprintf( fp, "\nReliability Class            : 0x%lX",   pGPRSQoS->reliabilityClass );
                    fprintf( fp, "\nPeak Throughput Class        : 0x%lX",   pGPRSQoS->peakThroughputClass );
                    fprintf( fp, "\nMean Throughput Class        : 0x%lX\n", pGPRSQoS->meanThroughputClass );
                }

                if( pProfile->pUsername )
                {
                    fprintf( fp, "\nUser Name                    : %s",      pProfile->pUsername );
                }
                IFPRINTF( fp, "\nUser Name Size                  : 0x%x",    pProfile->pUsernameSize );
                if( pProfile->pPassword )
                {
                    VFPRINTF( fp, "\nPassword                    : %s",      pProfile->pPassword );
                }
                IFPRINTF( fp, "\nPassword Size                   : 0x%x",    pProfile->pPasswordSize );
                IFPRINTF( fp, "\nAuthentication Preference       : 0x%X",    pProfile->pAuthenticationPref );
                IFPRINTF( fp, "\nIPv4 Address Preference         : 0x%lX",   pProfile->pIPv4AddrPref );
                IFPRINTF( fp, "\nPCSCF Address using PCO         : 0x%X",    pProfile->pPcscfAddrUsingPCO );
                IFPRINTF( fp, "\nPDP Access Control Flag         : 0x%X",    pProfile->pPdpAccessConFlag );
                IFPRINTF( fp, "\nPCSCF Address using DHCP        : 0x%X",    pProfile->pPcscfAddrUsingDhcp );
                IFPRINTF( fp, "\nIM CN Flag                      : 0x%X",    pProfile->pImCnFlag );

                if( pProfile->pTFTID1Params )
                {
                    struct TFTIDParams *pTFTIDParams = pProfile->pTFTID1Params;

                    fprintf( fp, "\n\n Traffic Flow Template ID1 Parameters" );
                    fprintf( fp, "\nFilter ID                    : 0x%X",    pTFTIDParams->filterId );
                    fprintf( fp, "\nEvaluation Precedence ID     : 0x%X",    pTFTIDParams->eValid );
                    fprintf( fp, "\nIP Version                   : 0x%X",    pTFTIDParams->ipVersion );

                    fprintf( fp, "\nSource IP                   : ");
                    if( IPv6 == pTFTIDParams->ipVersion )
                    {
                        for( idx = 0; idx < 8; idx++ )
                        {
                            fprintf( fp, "%X", pTFTIDParams->pSourceIP[idx] );
                            if (7 != idx)
                            {
                                fprintf( fp, ":" );
                            }
                        }
                    }
                    else
                    {
                        fprintf( fp, "0x");
                        for( idx = 0; idx < 2; idx++ )
                        {
                            fprintf( fp, "%X", pTFTIDParams->pSourceIP[idx] );
                        }
                    }
                    fprintf( fp, "\nSource IP mask               : 0x%X",    pTFTIDParams->sourceIPMask );
                    fprintf( fp, "\nNext Header                  : 0x%X",    pTFTIDParams->nextHeader );
                    fprintf( fp, "\nDestination Port Range Start : 0x%X",    pTFTIDParams->destPortRangeStart );
                    fprintf( fp, "\nDestination Port Range End   : 0x%X",    pTFTIDParams->destPortRangeEnd );
                    fprintf( fp, "\nSource Port Range Start      : 0x%X",    pTFTIDParams->srcPortRangeStart );
                    fprintf( fp, "\nSource Port Range End        : 0x%X",    pTFTIDParams->srcPortRangeEnd );
                    fprintf( fp, "\nIPSec SPI                    : 0x%lX",   pTFTIDParams->IPSECSPI );
                    fprintf( fp, "\nTOS Mask                     : 0x%X",    pTFTIDParams->tosMask );
                    fprintf( fp, "\nFlow Label                   : 0x%lX\n", pTFTIDParams->flowLabel );
                }

                if( pProfile->pTFTID2Params )
                {
                    struct TFTIDParams *pTFTIDParams = pProfile->pTFTID2Params;

                    fprintf( fp, "\n\n Traffic Flow Template ID2 Parameters" );
                    fprintf( fp, "\nFilter ID                    : 0x%X",    pTFTIDParams->filterId );
                    fprintf( fp, "\nEvaluation Precedence ID     : 0x%X",    pTFTIDParams->eValid );
                    fprintf( fp, "\nIP Version                   : 0x%X",    pTFTIDParams->ipVersion );

                    fprintf( fp, "\nSource IP                   : ");
                    if( IPv6 == pTFTIDParams->ipVersion )
                    {
                        for( idx = 0; idx < 8; idx++ )
                        {
                            fprintf( fp, "%X", pTFTIDParams->pSourceIP[idx] );
                            if (7 != idx)
                            {
                                fprintf( fp, ":" );
                            }
                        }
                    }
                    else
                    {
                        fprintf( fp, "0x");
                        for( idx = 0; idx < 2; idx++ )
                        {
                            fprintf( fp, "%X", pTFTIDParams->pSourceIP[idx] );
                        }
                    }

                    fprintf( fp, "\nSource IP mask               : 0x%X",    pTFTIDParams->sourceIPMask );
                    fprintf( fp, "\nNext Header                  : 0x%X",    pTFTIDParams->nextHeader );
                    fprintf( fp, "\nDestination Port Range Start : 0x%X",    pTFTIDParams->destPortRangeStart );
                    fprintf( fp, "\nDestination Port Range End   : 0x%X",    pTFTIDParams->destPortRangeEnd );
                    fprintf( fp, "\nSource Port Range Start      : 0x%X",    pTFTIDParams->srcPortRangeStart );
                    fprintf( fp, "\nSource Port Range End        : 0x%X",    pTFTIDParams->srcPortRangeEnd );
                    fprintf( fp, "\nIPSec SPI                    : 0x%lX",   pTFTIDParams->IPSECSPI );
                    fprintf( fp, "\nTOS Mask                     : 0x%X",    pTFTIDParams->tosMask );
                    fprintf( fp, "\nFlow Label                   : 0x%lX\n",   pTFTIDParams->flowLabel );
                }

                IFPRINTF( fp, "\nPDP Context Number              : 0x%X",    pProfile->pPdpContext );
                IFPRINTF( fp, "\nSecondary Flag                  : 0x%X",    pProfile->pSecondaryFlag );
                IFPRINTF( fp, "\nPDP Context Primary ID          : 0x%X",  pProfile->pPrimaryID );
                if( pProfile->pIPv6AddPref )
                {
                    fprintf( fp, "\nIPV6 address Preference: ");
                    for( idx = 0; idx < 8; idx++ )
                    {
                        fprintf( fp, "%X", pProfile->pIPv6AddPref[idx] );
                        if (7 != idx)
                        {
                            fprintf( fp, ":" );
                        }
                    }
                }

                if( pProfile->pUMTSReqQoSSigInd )
                {
                    struct UMTSQoS *pUMTSQoS = &(pProfile->pUMTSReqQoSSigInd->UMTSReqQoS);

                    fprintf( fp, "\n\n UMTS Requested QoS with Signalling Indication" );
                    fprintf( fp, "\nTraffic Class                : 0x%X",    pUMTSQoS->trafficClass );
                    fprintf( fp, "\nMaximum Uplink Bit Rate      : 0x%lX",   pUMTSQoS->maxUplinkBitrate );
                    fprintf( fp, "\nMaximum Downlink Bit Rate    : 0x%lX",   pUMTSQoS->maxDownlinkBitrate );
                    fprintf( fp, "\nGuaranteed Uplink Bit Rate   : 0x%lX",   pUMTSQoS->grntUplinkBitrate );
                    fprintf( fp, "\nGuaranteed Downlink Bit Rate : 0x%lX",   pUMTSQoS->grntDownlinkBitrate );
                    fprintf( fp, "\nQoS Delivery Order           : 0x%X",    pUMTSQoS->qosDeliveryOrder );
                    fprintf( fp, "\nMaximum SDU Size             : 0x%lX",   pUMTSQoS->maxSDUSize );
                    fprintf( fp, "\nSDU Error Ratio              : 0x%X",    pUMTSQoS->sduErrorRatio );
                    fprintf( fp, "\nResidual Bit Error Ratio     : 0x%X",    pUMTSQoS->resBerRatio );
                    fprintf( fp, "\nDelivery Erroneous SDU's     : 0x%X",    pUMTSQoS->deliveryErrSDU );
                    fprintf( fp, "\nTransfer Delay               : 0x%lX",   pUMTSQoS->transferDelay );
                    fprintf( fp, "\nTraffic Handling Priority    : 0x%lX",   pUMTSQoS->trafficPriority );
                    fprintf( fp, "\nSignalling Indication Flag   : 0x%X\n",  pProfile->pUMTSReqQoSSigInd->SigInd );
                }

                if( pProfile->pUMTSMinQosSigInd )
                {
                    struct UMTSQoS *pUMTSQoS = &(pProfile->pUMTSMinQosSigInd->UMTSReqQoS);

                    fprintf( fp,"\n\n UMTS Minimum QoS with Signalling Indication" );
                    fprintf( fp, "\nTraffic Class                : 0x%X",    pUMTSQoS->trafficClass );
                    fprintf( fp, "\nMaximum Uplink Bit Rate      : 0x%lX",   pUMTSQoS->maxUplinkBitrate );
                    fprintf( fp, "\nMaximum Downlink Bit Rate    : 0x%lX",   pUMTSQoS->maxDownlinkBitrate );
                    fprintf( fp, "\nGuaranteed Uplink Bit Rate   : 0x%lX",   pUMTSQoS->grntUplinkBitrate );
                    fprintf( fp, "\nGuaranteed Downlink Bit Rate : 0x%lX",   pUMTSQoS->grntDownlinkBitrate );
                    fprintf( fp, "\nQoS Delivery Order           : 0x%X",    pUMTSQoS->qosDeliveryOrder );
                    fprintf( fp, "\nMaximum SDU Size             : 0x%lX",   pUMTSQoS->maxSDUSize );
                    fprintf( fp, "\nSDU Error Ratio              : 0x%X",    pUMTSQoS->sduErrorRatio );
                    fprintf( fp, "\nResidual Bit Error Ratio     : 0x%X",    pUMTSQoS->resBerRatio );
                    fprintf( fp, "\nDelivery Erroneous SDU's     : 0x%X",    pUMTSQoS->deliveryErrSDU );
                    fprintf( fp, "\nTransfer Delay               : 0x%lX",   pUMTSQoS->transferDelay );
                    fprintf( fp, "\nTraffic Handling Priority    : 0x%lX",   pUMTSQoS->trafficPriority );
                    fprintf( fp, "\nSignalling Indication Flag   : 0x%X\n",  pProfile->pUMTSMinQosSigInd->SigInd );
                }

                if( pProfile->pPriDNSIPv6addpref )
                {
                    fprintf( fp, "\nPrimary DNS IPV6 address Preference: ");
                    for( idx = 0; idx < 8; idx++ )
                    {
                       fprintf( fp, "%X", pProfile->pPriDNSIPv6addpref[idx] );
                       if (7 != idx)
                       {
                           fprintf( fp, ":" );
                       }
                    }
                }

                if( pProfile->pSecDNSIPv6addpref )
                {
                    fprintf( fp, "\nSecondary DNS IPV6 address Preference: ");
                    for( idx = 0; idx < 8; idx++ )
                    {
                        fprintf( fp, "%X", pProfile->pSecDNSIPv6addpref[idx] );
                        if (7 != idx)
                        {
                            fprintf( fp, ":" );
                        }
                    }
                }

                IFPRINTF( fp, "\nAddress Allocation Preference   : 0x%X",    pProfile->pAddrAllocPref );

                if( pProfile->pQosClassID )
                {
                    struct QosClassID *pQosClassID = pProfile->pQosClassID;

                    fprintf( fp, "\n\n 3GPP LTE QoS Parameters" );
                    fprintf( fp, "\nQoS Class identifier         : 0x%X",    pQosClassID->QCI );
                    fprintf( fp, "\nGuaranteed DL Bit Rate       : 0x%lX",   pQosClassID->gDlBitRate );
                    fprintf( fp, "\nMaximum DL Bit Rate          : 0x%lX",   pQosClassID->maxDlBitRate );
                    fprintf( fp, "\nGuaranteed UL Bit Rate       : 0x%lX",   pQosClassID->gUlBitRate );
                    fprintf( fp, "\nMaximum UL Bit Rate          : 0x%lX\n", pQosClassID->maxUlBitRate );
                }
                IFPRINTF( fp, "\nAPN disabled flag               : 0x%X",    pProfile->pAPNDisabledFlag );
                IFPRINTF( fp, "\nPDN Inactivity Timeout          : 0x%lX",   pProfile->pPDNInactivTimeout );
                IFPRINTF( fp, "\nAPN class                       : 0x%X",    pProfile->pAPNClass );
            }
            if( eQCWWAN_ERR_QMI_EXTENDED_INTERNAL == nRet)
            {
                IFPRINTF(fp, "Extended Error Code                : %d\n",    ProfileSetting.pExtErrCode);
            }
            tCaseNum++;
        }
    }

    if (fp)
        tfclose(fp);
}

/*
 * Name:     doSLQSGetProfileSettings3GPP2
 *
 * Purpose:  Perform the tests that call the SLQSGetProfileSettings() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetProfileSettings3GPP2( void )
{
    ULONG                nRet;
    FILE                 *fp = NULL;
    GetProfileSettingIn  ProfileSettingReq;
    GetProfileSettingOut ProfileSetting;
    struct Profile3GPP2  *pProfile;
    BYTE                 idx                = 0;

    /* Define the test cases to be executed */
    SLQSGetProfileSettings3GPP2TestCase_t SLQSGetProfileSettings3GPP2TestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, -1, -1, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, "Invalid Test Case - Invalid params" },

        { eQCWWAN_ERR_NONE, 2, profType3GPP2, profIndex3GPP2, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, "Valid test Case - No out parameter" },

        { eQCWWAN_ERR_NONE, 3, profType3GPP2, profIndex3GPP2, &NegoDNSServPref, &PPPSessCloTimD0,
          &PPPSessCloTim1X, &AllowLinger, &LcpAckTimeout, &IpcpAckTimeout, &AuthTimeout, &LcpCreqRetCt,
          &IpCpCReqRetCt, &authRetryCt, &authProt, userID, &InValuserIDSize, authPwd, &ValauthPwdSize, &DataRate,
          &AppType, &DataMode, &appPrio, ApnStr, &ValApnStrSize, &PdnType, &isPcscfAdd, &PriDNSIPv4Add, &SecDNSIPv4Add,
          PriDNSIPv6Add, SecDNSIPv6Add, &RatType, &ApnEnable3GPP2, &PDNInactivTim, &APNClass, &extErrCode,
          "InValid test Case - All params specified - Invalid User ID size" },

        { eQCWWAN_ERR_NONE, 4, profType3GPP2, profIndex3GPP2, &NegoDNSServPref, &PPPSessCloTimD0,
          &PPPSessCloTim1X, &AllowLinger, &LcpAckTimeout, &IpcpAckTimeout, &AuthTimeout, &LcpCreqRetCt,
          &IpCpCReqRetCt, &authRetryCt, &authProt, userID, &ValuserIDSize, authPwd, &InValauthPwdSize, &DataRate,
          &AppType, &DataMode, &appPrio, ApnStr, &ValApnStrSize, &PdnType, &isPcscfAdd, &PriDNSIPv4Add, &SecDNSIPv4Add,
          PriDNSIPv6Add, SecDNSIPv6Add, &RatType, &ApnEnable3GPP2, &PDNInactivTim, &APNClass, &extErrCode,
          "InValid test Case - All params specified - Invalid Auth Password size" },

        { eQCWWAN_ERR_NONE, 5, profType3GPP2, profIndex3GPP2, &NegoDNSServPref, &PPPSessCloTimD0,
          &PPPSessCloTim1X, &AllowLinger, &LcpAckTimeout, &IpcpAckTimeout, &AuthTimeout, &LcpCreqRetCt,
          &IpCpCReqRetCt, &authRetryCt, &authProt, userID, &ValuserIDSize, authPwd, &ValauthPwdSize, &DataRate,
          &AppType, &DataMode, &appPrio, ApnStr, &InValApnStrSize, &PdnType, &isPcscfAdd, &PriDNSIPv4Add, &SecDNSIPv4Add,
          PriDNSIPv6Add, SecDNSIPv6Add, &RatType, &ApnEnable3GPP2, &PDNInactivTim, &APNClass, &extErrCode,
          "InValid test Case - All params specified - Invalid APN string size" },

        { eQCWWAN_ERR_NONE, 6, profType3GPP2, profIndex3GPP2, &NegoDNSServPref, &PPPSessCloTimD0,
          &PPPSessCloTim1X, &AllowLinger, &LcpAckTimeout, &IpcpAckTimeout, &AuthTimeout, &LcpCreqRetCt,
          &IpCpCReqRetCt, &authRetryCt, &authProt, userID, &ValuserIDSize, authPwd, &ValauthPwdSize, &DataRate,
          &AppType, &DataMode, &appPrio, ApnStr, &ValApnStrSize, &PdnType, &isPcscfAdd, &PriDNSIPv4Add, &SecDNSIPv4Add,
          PriDNSIPv6Add, SecDNSIPv6Add, &RatType, &ApnEnable3GPP2, &PDNInactivTim, &APNClass, &extErrCode,
          "Valid test Case - All params specified - valid sizes" },

    };

    fp = tfopen( "../../wds/test/results/slqsgetprofilesettings3gpp2.txt", "w" );
    if ( fp )
    {
        BYTE tCaseNum = 0;
        BYTE ncases   = sizeof( SLQSGetProfileSettings3GPP2TestCases ) /
                        sizeof( SLQSGetProfileSettings3GPP2TestCases[0] );

        while( tCaseNum < ncases )
        {
            SLQSGetProfileSettings3GPP2TestCase_t *pd =
                &SLQSGetProfileSettings3GPP2TestCases[tCaseNum];

            pProfile = &(ProfileSetting.curProfile.SlqsProfile3GPP2);

            ProfileSettingReq.ProfileType  = pd->ProfileType;
            ProfileSettingReq.ProfileID    = pd->ProfileID;

            pProfile->pNegoDnsSrvrPref        = pd->pNegoDnsSrvrPref;
            pProfile->pPppSessCloseTimerDO    = pd->pPppSessCloseTimerDO;
            pProfile->pPppSessCloseTimer1x    = pd->pPppSessCloseTimer1x;
            pProfile->pAllowLinger            = pd->pAllowLinger;
            pProfile->pLcpAckTimeout          = pd->pLcpAckTimeout;
            pProfile->pIpcpAckTimeout         = pd->pIpcpAckTimeout;
            pProfile->pAuthTimeout            = pd->pAuthTimeout;
            pProfile->pLcpCreqRetryCount      = pd->pLcpCreqRetryCount;
            pProfile->pIpcpCreqRetryCount     = pd->pIpcpCreqRetryCount;
            pProfile->pAuthRetryCount         = pd->pAuthRetryCount;
            pProfile->pAuthProtocol           = pd->pAuthProtocol;
            pProfile->pUserId                 = pd->pUserId;
            pProfile->pUserIdSize             = pd->pUserIdSize;
            pProfile->pAuthPassword           = pd->pAuthPassword;
            pProfile->pAuthPasswordSize       = pd->pAuthPwdSize;
            pProfile->pDataRate               = pd->pDataRate;
            pProfile->pAppType                = pd->pAppType;
            pProfile->pDataMode               = pd->pDataMode;
            pProfile->pAppPriority            = pd->pAppPriority;
            pProfile->pApnString              = pd->pApnString;
            pProfile->pApnStringSize          = pd->pApnStrSize;
            pProfile->pPdnType                = pd->pPdnType;
            pProfile->pIsPcscfAddressNedded   = pd->pIsPcscfAddressNedded;
            pProfile->pPrimaryV4DnsAddress    = pd->pPrimaryV4DnsAddress;
            pProfile->pSecondaryV4DnsAddress  = pd->pSecondaryV4DnsAddress;
            pProfile->pPriV6DnsAddress        = pd->pPriV6DnsAddress;
            pProfile->pSecV6DnsAddress        = pd->pSecV6DnsAddress;
            pProfile->pRATType                = pd->pRATType;
            pProfile->pAPNEnabled3GPP2        = pd->pAPNEnabled3GPP2;
            pProfile->pPDNInactivTimeout3GPP2 = pd->pPDNInactivTimeout3GPP2;
            pProfile->pAPNClass3GPP2          = pd->pAPNClass3GPP2;

            ProfileSetting.pExtErrCode        = pd->pExtErrCode;

            fprintf( fp,"\n\nTest Case %d / %d : %s\n",
                     (tCaseNum + 1),
                     ncases,
                     pd->desc );

            fprintf( fp, "Parameter Set:\n" );
            fprintf( fp, "Profile Type : %d\n", pd->ProfileType );
            fprintf( fp, "Profile ID : %d\n",   pd->ProfileID );
            nRet = SLQSGetProfileSettings( &ProfileSettingReq,
                                           &ProfileSetting );

            /* Display result code and text */
            doprintreason( fp, nRet );
            if (!nRet)
            {
                fprintf( fp,"Details for Profile \n" );

                IFPRINTF( fp, "\nNegotiate DNS Server Preference : 0x%X",  pProfile->pNegoDnsSrvrPref );
                IFPRINTF( fp, "\nPPP Session close Timer for DO  : 0x%lX", pProfile->pPppSessCloseTimerDO );
                IFPRINTF( fp, "\nPPP Session close Timer for 1X  : 0x%lX", pProfile->pPppSessCloseTimer1x );
                IFPRINTF( fp, "\nAllow Linger                    : 0x%X",  pProfile->pAllowLinger );
                IFPRINTF( fp, "\nLCP ACK Timeout                 : 0x%X",  pProfile->pLcpAckTimeout );
                IFPRINTF( fp, "\nIPCP ACK Timeout                : 0x%X",  pProfile->pIpcpAckTimeout );
                IFPRINTF( fp, "\nAuth Timeout                    : 0x%X",  pProfile->pAuthTimeout );
                IFPRINTF( fp, "\nLCP Config. Request Retry Count : 0x%X",  pProfile->pLcpCreqRetryCount );
                IFPRINTF( fp, "\nIPCP Config. Request Retry Count: 0x%X",  pProfile->pIpcpCreqRetryCount );
                IFPRINTF( fp, "\nAuthentication Retry Count      : 0x%X",  pProfile->pAuthRetryCount );
                IFPRINTF( fp, "\nAuthentication Protocol         : 0x%X",  pProfile->pAuthProtocol );
                if( pProfile->pUserId )
                {
                    fprintf( fp, "\nUser ID                      : %s",    pProfile->pUserId );
                }
                IFPRINTF( fp, "\nUser ID Size                    : 0x%X",  pProfile->pUserIdSize );
                if( pProfile->pAuthPassword )
                {
                    fprintf( fp, "\nAuthentication Password      : %s",    pProfile->pAuthPassword );
                }
                IFPRINTF( fp, "\nAuthentication Password Size    : 0x%X",  pProfile->pAuthPasswordSize );
                IFPRINTF( fp, "\nData Rate                       : 0x%X",  pProfile->pDataRate );
                IFPRINTF( fp, "\nApplication Type                : 0x%lX", pProfile->pAppType );
                IFPRINTF( fp, "\nData Mode                       : 0x%X",  pProfile->pDataMode );
                IFPRINTF( fp, "\nApplication Priority            : 0x%X",  pProfile->pAppPriority );
                if( pProfile->pApnString )
                {
                    fprintf( fp, "\nAPN String                   : %s",    pProfile->pApnString );
                }
                IFPRINTF( fp, "\nAPN String Size                 : 0x%X",  pProfile->pApnStringSize );
                IFPRINTF( fp, "\nPDN Type                        : 0x%X",  pProfile->pPdnType );
                IFPRINTF( fp, "\nIs Pcscf Address Needed         : 0x%X",  pProfile->pIsPcscfAddressNedded );
                IFPRINTF( fp, "\nIPv4 Primary DNS Address        : 0x%lX", pProfile->pPrimaryV4DnsAddress );
                IFPRINTF( fp, "\nIPv4 Secondary DNS Address      : 0x%lX", pProfile->pSecondaryV4DnsAddress );
                if( pProfile->pPriV6DnsAddress )
                {
                    fprintf( fp, "\nPrimary DNS IPV6 address     : ");
                    for( idx = 0; idx < 8; idx++ )
                    {
                       fprintf( fp, "%X", pProfile->pPriV6DnsAddress[idx] );
                       if (7 != idx)
                       {
                           fprintf( fp, ":" );
                       }
                    }
                }

                if( pProfile->pSecV6DnsAddress )
                {
                    fprintf( fp, "\nSecondary DNS IPV6 address   : ");
                    for( idx = 0; idx < 8; idx++ )
                    {
                        fprintf( fp, "%X", pProfile->pSecV6DnsAddress[idx] );
                        if (7 != idx)
                        {
                            fprintf( fp, ":" );
                        }
                    }
                }
                IFPRINTF( fp, "\nRAT Type                        : 0x%X",  pProfile->pRATType );
                IFPRINTF( fp, "\nAPN Enabled                     : 0x%X",  pProfile->pAPNEnabled3GPP2 );
                IFPRINTF( fp, "\nPDN Inactivity Timeout          : 0x%lX", pProfile->pPDNInactivTimeout3GPP2 );
                IFPRINTF( fp, "\nAPN class                       : 0x%X",  pProfile->pAPNClass3GPP2 );

            }
            if( eQCWWAN_ERR_QMI_EXTENDED_INTERNAL == nRet)
            {
                IFPRINTF(fp, "Extended Error Code: %d\n", ProfileSetting.pExtErrCode);
            }
            tCaseNum++;
        }
    }

    if (fp)
        tfclose(fp);
}

/*
 * Name:     doSLQSGetProfileSettings
 *
 * Purpose:  Executes doSLQSGetProfileSettings3GPP and doSLQSGetProfileSettings3GPP2
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetProfileSettings( void )
{
    doSLQSGetProfileSettings3GPP();
    doSLQSGetProfileSettings3GPP2();
}

/*
 * Name:     doSetMobileIPProfile
 *
 * Purpose:  Perform the tests that call the SetMobileIPProfile() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSetMobileIPProfile( void )
{
    ULONG nRet;
    FILE  *fp = NULL;
    WORD  lCount;
    CHAR  *pSPC = "577551";
    BYTE  index = 1;
    BYTE  Enabled = 1;
    ULONG Address = 0xFFFFFFFF;
    ULONG PrimaryHA  = 0xFFFFFFFF;
    ULONG SecondaryHA = 0xFFFFFFFF;
    BYTE  RevTunneling = 1;
    CHAR  *pNAI = "www.sierrawireless.com";
    ULONG HASPI = 0xFFFFFFFF;
    ULONG AAASPI = 0xFFFFFFFF;;
    CHAR  *pMNHA = "test";
    CHAR  *pMNAAA = "test";

    /* Define the test cases to be executed */
    SetMobileIPProfileTestCaseStep_t SetMobileIPProfileTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, NULL, index, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, "Invalid Test Case - All params NULL"},

        { eQCWWAN_ERR_INVALID_ARG, 1, pSPC, index, &Enabled, &Address, &PrimaryHA, &SecondaryHA, &RevTunneling,
           pNAI, &HASPI, &AAASPI, pMNHA, pMNAAA, "Valid Test Case"},
    };

    fp = tfopen( "TestReport/setmobileIPprofile.txt", "w" );
    if ( fp )
    {
        BYTE tCaseNum     = 0;
        BYTE ncases = sizeof( SetMobileIPProfileTestCases ) /
                      sizeof( SetMobileIPProfileTestCases[0] );

        while( tCaseNum < ncases )
        {
            SetMobileIPProfileTestCaseStep_t *pd =
                &SetMobileIPProfileTestCases[tCaseNum];

            fprintf( fp,"\n\nTest Case %d / %d : %s\n",
                     (tCaseNum + 1),
                     ncases,
                     pd->desc );
            fprintf( fp, "SPC : ");
            if (pd->pSPC)
            {
                for ( lCount = 0; pd->pSPC[lCount]!=EOS ; lCount++ )
                {
                    fprintf(fp, "%c",pd->pSPC[lCount]);
                }
            }
            fprintf( fp, "\nIndex : %d", pd->index );
            IFPRINTF( fp, "\nAddress : %lx", pd->pAddress );
            IFPRINTF( fp, "\nPrimary HA : %lx", pd->pPrimaryHA );
            IFPRINTF( fp, "\nSecondary HA : %lx", pd->pSecondaryHA );
            IFPRINTF( fp, "\nRev Tunneling : %d", pd->pRevTunneling );
            fprintf( fp, "\nNAI : ");
            if (pd->pNAI)
            {
                for ( lCount = 0; pd->pNAI[lCount]!=EOS ; lCount++ )
                {
                    fprintf(fp, "%c",pd->pNAI[lCount]);
                }
            }
            IFPRINTF( fp, "\nHASPI : %lx", pd->pHASPI );
            IFPRINTF( fp, "\nAAASPI : %lx", pd->pAAASPI );
            fprintf( fp, "\nMNHA : ");
            if (pd->pMNHA)
            {
                for ( lCount = 0; pd->pMNHA[lCount]!=EOS ; lCount++ )
                {
                    fprintf(fp, "%c",pd->pMNHA[lCount]);
                }
            }
            fprintf( fp, "\nMNAA : ");
            if (pd->pMNAAA)
            {
                for ( lCount = 0; pd->pMNAAA[lCount]!=EOS ; lCount++ )
                {
                    fprintf(fp, "%c",pd->pMNAAA[lCount]);
                }
            }
            fprintf( fp,"\n");

            nRet = SetMobileIPProfile( pd->pSPC,
                                       pd->index,
                                       pd->pEnabled,
                                       pd->pAddress,
                                       pd->pPrimaryHA,
                                       pd->pSecondaryHA,
                                       pd->pRevTunneling,
                                       pd->pNAI,
                                       pd->pHASPI,
                                       pd->pAAASPI,
                                       pd->pMNHA,
                                       pd->pMNAAA );

            /* Display result code and text */
            doprintreason( fp, nRet );
            tCaseNum++;
        }
    }

    if (fp)
        tfclose(fp);
}

/*
 * Name:    doSLQSWdsSetEventReport
 *
 * Purpose: Perform the tests that call the SLQSWdsSetEventReport() API
 *
 * Return:  none
 *
 * Notes:   none
 *
 */
void doSLQSWdsSetEventReport( void )
{
    /* Test Cases */
    SLQSWdsSetEventReportTestCase_t SLQSWdsSetEventReportTestCases[]=
    {
        { eQCWWAN_ERR_NONE, 1, &unSetInd, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, "All Params NULL" },

        { eQCWWAN_ERR_NONE, 2, &setInd, &transferStatInd, &setInd, &setInd,
        &setInd, &setInd, &setInd, &setInd, &setInd, &setInd,
        "All Params Set" },
    };

    FILE *fp = tfopen("../../wds/test/results/slqswdsseteventreport.txt", "w");
    wdsSetEventReportReq req;

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSWdsSetEventReportTestCases )/
                      sizeof( SLQSWdsSetEventReportTestCases[0] );
        while ( tc < ncases )
        {
            SLQSWdsSetEventReportTestCase_t *pd = &SLQSWdsSetEventReportTestCases[tc++];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tc,
                        ncases,
                        pd->desc);

            req.pCurrChannelRateInd        = pd->pCurrChannelRateInd;
            req.pTransferStatInd           = pd->pTransferStatInd;
            req.pDataBearerTechInd         = pd->pDataBearerTechInd;
            req.pDormancyStatusInd         = pd->pDormancyStatusInd;
            req.pMIPStatusInd              = pd->pMIPStatusInd;
            req.pCurrDataBearerTechInd     = pd->pCurrDataBearerTechInd;
            req.pDataCallStatusChangeInd   = pd->pDataCallStatusChangeInd;
            req.pCurrPrefDataSysInd        = pd->pCurrPrefDataSysInd;
            req.pEVDOPageMonPerChangeInd   = pd->pEVDOPageMonPerChangeInd;
            req.pDataSystemStatusChangeInd = pd->pDataSystemStatusChangeInd;

            IFPRINTF( fp, "Curr Channel Rate Ind    : %x\n",req.pCurrChannelRateInd )
            if (req.pTransferStatInd)
            {
                fprintf( fp, "Stats Period : %x\n",req.pTransferStatInd->statsPeriod );
                fprintf( fp, "Stats Mask   : %lx\n",req.pTransferStatInd->statsMask );
            }
            IFPRINTF( fp, "Data Bearer Ind          : %x\n",req.pDataBearerTechInd )
            IFPRINTF( fp, "Dormancy Status Ind      : %x\n",req.pDormancyStatusInd )
            IFPRINTF( fp, "MIP Status Ind           : %x\n",req.pMIPStatusInd )
            IFPRINTF( fp, "Curr Dormancy Status Ind : %x\n",req.pCurrDataBearerTechInd )
            IFPRINTF( fp, "Data Call Status Ind     : %x\n",req.pDataCallStatusChangeInd )
            IFPRINTF( fp, "Curr Pref Data Sys Ind   : %x\n",req.pCurrPrefDataSysInd )
            IFPRINTF( fp, "EVDO Page Monito Ind     : %x\n",req.pEVDOPageMonPerChangeInd )
            IFPRINTF( fp, "Data Sys Status Ind      : %x\n",req.pDataSystemStatusChangeInd )

            ULONG nRet = SLQSWdsSetEventReport( &req );
            doprintreason( fp, nRet );

            if ( eQCWWAN_ERR_NONE == nRet )
            {
                fprintf( fp, "SLQSWdsSetEventReport Successful\n");
                continue;
            }

            fprintf( fp, "SLQSWdsSetEventReport Unsuccessful\n");
        }
    }
    if (fp)
        tfclose(fp);
}

/*
 * Name:     doSLQSWdsSwiPDPRuntimeSettings
 *
 * Purpose:  Perform the tests that call the SLQSWdsSwiPDPRuntimeSettings() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSWdsSwiPDPRuntimeSettings( void )
{
    local BYTE                   contextId;
    local BYTE                   bearerId;
    local CHAR                   APNName[nMaxStrLen];
    local ULONG                  IPv4Address;
    local ULONG                  IPv4GWAddress;
    local ULONG                  PrDNSIPv4Address;
    local ULONG                  SeDNSIPv4Address;
    local struct IPV6AddressInfo IPv6Address;
    local struct IPV6AddressInfo IPv6GWAddress;
    local WORD                   PrDNSIPv6Address[IPV6_ADDRESS_ARRAY_SIZE];
    local WORD                   SeDNSIPv6Address[IPV6_ADDRESS_ARRAY_SIZE];
    local ULONG                  PrPCSCFIPv4Address;
    local ULONG                  SePCSCFIPv4Address;
    local WORD                   PrPCSCFIPv6Address[IPV6_ADDRESS_ARRAY_SIZE];
    local WORD                   SePCSCFIPv6Address[IPV6_ADDRESS_ARRAY_SIZE];

    /* Define the test cases to be executed */
    SLQSWdsSwiPDPRuntimeSettingsTestCase_t SLQSWdsSwiPDPRuntimeSettingsTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, 0x01, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        "Valid Test Case - Passing NULL"},

        { eQCWWAN_ERR_NONE, 2, 0x01,
        &contextId,
        &bearerId,
        APNName,
        &IPv4Address,
        &IPv4GWAddress,
        &PrDNSIPv4Address,
        &SeDNSIPv4Address,
        &IPv6Address,
        &IPv6GWAddress,
        PrDNSIPv6Address,
        SeDNSIPv6Address,
        &PrPCSCFIPv4Address,
        &SePCSCFIPv4Address,
        PrPCSCFIPv6Address,
        SePCSCFIPv6Address,
        "Valid Test Case - Fetching All parameters "},

    };

    FILE *fp = tfopen( "../../nas/test/results/slqswdsswipdpruntimesettings.txt", "w" );
    swiPDPRuntimeSettingsReq  req;
    swiPDPRuntimeSettingsResp resp;
    WORD                      iC;

    if ( fp )
    {
        BYTE tC     = 0;
        BYTE ncases = sizeof( SLQSWdsSwiPDPRuntimeSettingsTestCases ) /
                      sizeof( SLQSWdsSwiPDPRuntimeSettingsTestCases[0] );

        while( tC < ncases )
        {
            SLQSWdsSwiPDPRuntimeSettingsTestCase_t *pd =
                       &SLQSWdsSwiPDPRuntimeSettingsTestCases[tC++];

            /* print test data */
            fprintf( fp,"\n\nTest Case %d / %d : %s\n",
                     tC,
                     ncases,
                     pd->desc );

            req.contextId = pd->contextId;
            fprintf( fp,"Context Id : %x \n\n",req.contextId );

            resp.pContextId          = pd->pContextId;
            resp.pBearerId           = pd->pBearerId;
            resp.pAPNName            = pd->pAPNName;
            resp.pIPv4Address        = pd->pIPv4Address;
            resp.pIPv4GWAddress      = pd->pIPv4GWAddress;
            resp.pPrDNSIPv4Address   = pd->pPrDNSIPv4Address;
            resp.pSeDNSIPv4Address   = pd->pSeDNSIPv4Address;
            resp.pIPv6Address        = pd->pIPv6Address;
            resp.pIPv6GWAddress      = pd->pIPv6GWAddress;
            resp.pPrDNSIPv6Address   = pd->pPrDNSIPv6Address;
            resp.pSeDNSIPv6Address   = pd->pSeDNSIPv6Address;
            resp.pPrPCSCFIPv4Address = pd->pPrPCSCFIPv4Address;
            resp.pSePCSCFIPv4Address = pd->pSePCSCFIPv4Address;
            resp.pPrPCSCFIPv6Address = pd->pPrPCSCFIPv6Address;
            resp.pSePCSCFIPv6Address = pd->pSePCSCFIPv6Address;

            ULONG nRet = SLQSWdsSwiPDPRuntimeSettings( &req, &resp );

            /* Display result code and text */
            doprintreason( fp, nRet );

            if ( eQCWWAN_ERR_NONE == nRet )
            {
                fprintf( fp,"SLQSWdsSwiPDPRuntimeSettings Successful \n" );

                IFPRINTF( fp, "Context Id      : %x \n",resp.pContextId);
                IFPRINTF( fp, "Bearer Id       : %x \n",resp.pBearerId);
                if ( resp.pAPNName )
                {
                    fprintf( fp, "APN Name        : %s \n",resp.pAPNName);
                }
                IFPRINTF( fp, "IPv4 Address    : %lx \n",resp.pIPv4Address);
                IFPRINTF( fp, "IPv4 GW Address : %lx \n",resp.pIPv4GWAddress);
                IFPRINTF( fp, "Primary DNS IPv4 Address   : %lx \n",resp.pPrDNSIPv4Address);
                IFPRINTF( fp, "Secondary DNS IPv4 Address : %lx \n",resp.pSeDNSIPv4Address);

                if ( resp.pIPv6Address )
                {
                    fprintf( fp,"IPv6Address : ");
                    for ( iC = 0 ; iC < IPV6_ADDRESS_ARRAY_SIZE ; iC++ )
                    {
                        fprintf( fp,"%x ",resp.pIPv6Address->IPAddressV6[iC]);
                    }
                    fprintf( fp,"\n");
                    fprintf( fp,"IPV6PrefixLen : %d \n",resp.pIPv6Address->IPV6PrefixLen );
                }
                if ( resp.pIPv6GWAddress )
                {
                    fprintf( fp,"IPv6GWAddress : ");
                    for ( iC = 0 ; iC < IPV6_ADDRESS_ARRAY_SIZE ; iC++ )
                    {
                        fprintf( fp,"%x ",resp.pIPv6GWAddress->IPAddressV6[iC]);
                    }
                    fprintf( fp,"\n");
                    fprintf( fp,"IPV6GWPrefixLen : %d \n",resp.pIPv6GWAddress->IPV6PrefixLen );
                }
                if ( resp.pPrDNSIPv6Address )
                {
                    fprintf( fp,"Primary DNS IPv6 Address   : ");
                    for ( iC = 0 ; iC < IPV6_ADDRESS_ARRAY_SIZE ; iC++ )
                    {
                        fprintf( fp,"%x ",resp.pPrDNSIPv6Address[iC]);
                    }
                    fprintf( fp,"\n");
                }
                if ( resp.pSeDNSIPv6Address )
                {
                    fprintf( fp,"Secondary DNS IPv6 Address : ");
                    for ( iC = 0 ; iC < IPV6_ADDRESS_ARRAY_SIZE ; iC++ )
                    {
                        fprintf( fp,"%x ",resp.pSeDNSIPv6Address[iC]);
                    }
                    fprintf( fp,"\n");
                }
                IFPRINTF( fp, "Primary PCSCF IPv4 Address   : %lx \n",resp.pPrPCSCFIPv4Address);
                IFPRINTF( fp, "Secondary PCSCF IPv4 Address : %lx \n",resp.pSePCSCFIPv4Address);
                if ( resp.pPrPCSCFIPv6Address )
                {
                    fprintf( fp,"Primary PCSCF IPv6 Address   : ");
                    for ( iC = 0 ; iC < IPV6_ADDRESS_ARRAY_SIZE ; iC++ )
                    {
                        fprintf( fp,"%x ",resp.pPrPCSCFIPv6Address[iC]);
                    }
                    fprintf( fp,"\n");
                }
                if ( resp.pSePCSCFIPv6Address )
                {
                    fprintf( fp,"Secondary PCSCF IPv6 Address : ");
                    for ( iC = 0 ; iC < IPV6_ADDRESS_ARRAY_SIZE ; iC++ )
                    {
                        fprintf( fp,"%x ",resp.pSePCSCFIPv6Address[iC]);
                    }
                    fprintf( fp,"\n");
                }
            }
        }
    }
    if (fp)
        tfclose(fp);
}

void doSLQSGetConnectionRate( void )
{

    struct WdsConnectionRate lConnectionRate;
    struct WdsConnectionRateElmnts v4Elmnt;
    struct WdsConnectionRateElmnts v6Elmnt;
    ULONG CurrentChannelRXRateV4;
    ULONG CurrentChannelTXRateV4;
    ULONG CurrentChannelRXRateV6;
    ULONG CurrentChannelTXRateV6;
    ULONG MaxChannelTXRateV4;
    ULONG MaxChannelRXRateV4;
    ULONG MaxChannelRXRateV6;
    ULONG MaxChannelTXRateV6;

    v4Elmnt.pCurrentChannelRXRate = &CurrentChannelRXRateV4;
    v4Elmnt.pCurrentChannelTXRate = &CurrentChannelTXRateV4;
    v4Elmnt.pMaxChannelRXRate     = &MaxChannelRXRateV4;
    v4Elmnt.pMaxChannelTXRate     = &MaxChannelTXRateV4;

    v6Elmnt.pCurrentChannelRXRate = &CurrentChannelRXRateV6;
    v6Elmnt.pCurrentChannelTXRate = &CurrentChannelTXRateV6;
    v6Elmnt.pMaxChannelRXRate     = &MaxChannelRXRateV6;
    v6Elmnt.pMaxChannelTXRate     = &MaxChannelTXRateV6;

    FILE   *fp      = NULL;
    BYTE   qmiInst  = 0xFF;
    BYTE   sessType = 0xFF;

  
    SLQSGetConnectionRateTestCaseStep_t SLQSGetConnectionRateTestCases[] =
    {
        {eQCWWAN_ERR_NONE, 1, &v4Elmnt, &v6Elmnt, "All Valid instance = 0"},
        {eQCWWAN_ERR_NONE, 2, &v4Elmnt, &v6Elmnt, "All Valid instance = 1"},
        {eQCWWAN_ERR_NONE, 3, &v4Elmnt, &v6Elmnt, "All Valid instance = 1"},
        {eQCWWAN_ERR_NONE, 4, &v4Elmnt, &v6Elmnt, "All Valid instance = 2"},
    };
    fp = tfopen("../../wds/test/results/getconnectionrate.txt","w");

    if (fp)
    {
        BYTE tc = 0;
        BYTE  ncases = sizeof( SLQSGetConnectionRateTestCases )/
                       sizeof( SLQSGetConnectionRateTestCases[0]);
        while ( tc < ncases )
        {
            SLQSGetConnectionRateTestCaseStep_t *pd = &SLQSGetConnectionRateTestCases[tc++];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tc,
                        ncases,
                        pd->desc);
            qmiInst  = (tc-1)/3;
            sessType = (tc-1)%3;

            fprintf( fp, "\nQMI Instance: %x, Session Type: %x \n",
                                 qmiInst,
                                 sessType );

            /* Initialize session ID to NULL */
            lConnectionRate.pV4sessionId = NULL;
            lConnectionRate.pV6sessionId = NULL;
            lConnectionRate.ConnRateElmntsV4=*(pd->vp4Elemnt);
            lConnectionRate.ConnRateElmntsV6=*(pd->vp6Elemnt);
            /* IPv4 */
            if ( 0 == sessType )
            {
                if( ZERO_SESSION_ID != s4[qmiInst].v4sessionId )
                {
                    lConnectionRate.pV4sessionId = &s4[qmiInst].v4sessionId;
                }
                lConnectionRate.pV6sessionId = NULL;
            }

            /* IPv6 */
            if ( 1 == sessType )
            {
                lConnectionRate.pV4sessionId = NULL;
                if( ZERO_SESSION_ID != s6[qmiInst].v6sessionId )
                {
                    lConnectionRate.pV6sessionId = &s6[qmiInst].v6sessionId;
                }
            }

            /* IPv4v6 */
            if ( 2 == sessType )
            {
                if( ZERO_SESSION_ID != s46[qmiInst].v4sessionId )
                {
                    lConnectionRate.pV4sessionId = &s46[qmiInst].v4sessionId;
                }
                if( ZERO_SESSION_ID != s46[qmiInst].v6sessionId )
                {
                    lConnectionRate.pV6sessionId = &s46[qmiInst].v6sessionId;
                }
            }

            ULONG nRet = SLQSGetConnectionRate( &lConnectionRate );

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
            {
                IFPRINTF( fp, "Current Channel TX rate v4 : %lx \n",lConnectionRate.ConnRateElmntsV4.pCurrentChannelTXRate);
                IFPRINTF( fp, "Current Channel RX rate v4 : %lx \n",lConnectionRate.ConnRateElmntsV4.pCurrentChannelRXRate);
                IFPRINTF( fp, "Maximum Tx rate v4         : %lx \n",lConnectionRate.ConnRateElmntsV4.pMaxChannelTXRate);
                IFPRINTF( fp, "Maximum Rx rate v4         : %lx \n",lConnectionRate.ConnRateElmntsV4.pMaxChannelRXRate);

                IFPRINTF( fp, "Current Channel TX rate v6 : %lx \n",lConnectionRate.ConnRateElmntsV6.pCurrentChannelTXRate);
                IFPRINTF( fp, "Current Channel RX rate v6 : %lx \n",lConnectionRate.ConnRateElmntsV6.pCurrentChannelRXRate);
                IFPRINTF( fp, "Maximum Tx rate v6         : %lx \n",lConnectionRate.ConnRateElmntsV6.pMaxChannelTXRate);
                IFPRINTF( fp, "Maximum Rx rate v6         : %lx \n",lConnectionRate.ConnRateElmntsV6.pMaxChannelRXRate);            
            }
        }
   }

   if (fp)
       tfclose(fp);
}

void doSLQSGetByteTotals( void )
{

    struct WdsByteTotals       lByteTotals;
    struct WdsByteTotalsElmnts v4Elmnt;
    struct WdsByteTotalsElmnts v6Elmnt;
    ULONGLONG                  RXTotalBytesV4;
    ULONGLONG                  TXTotalBytesV4;
    ULONGLONG                  RXTotalBytesV6;
    ULONGLONG                  TXTotalBytesV6;

    v4Elmnt.pRXTotalBytes = &RXTotalBytesV4;
    v4Elmnt.pTXTotalBytes = &TXTotalBytesV4;

    v6Elmnt.pRXTotalBytes = &RXTotalBytesV6;
    v6Elmnt.pTXTotalBytes = &TXTotalBytesV6;

    FILE *fp      = NULL;
    BYTE qmiInst  = 0xFF;
    BYTE sessType = 0xFF;

    SLQSGetByteTotalsTestCaseStep_t SLQSGetByteTotalsTestCases[] =
    {
        {eQCWWAN_ERR_NONE, 1, &v4Elmnt, &v6Elmnt, "All Valid instance "},
        {eQCWWAN_ERR_NONE, 2, &v4Elmnt, &v6Elmnt, "All Valid instance "},
        {eQCWWAN_ERR_NONE, 3, &v4Elmnt, &v6Elmnt, "All Valid instance "},
        {eQCWWAN_ERR_NONE, 4, &v4Elmnt, &v6Elmnt, "All Valid instance "},
        {eQCWWAN_ERR_NONE, 5, &v4Elmnt, &v6Elmnt, "All Valid instance "},
        {eQCWWAN_ERR_NONE, 6, &v4Elmnt, &v6Elmnt, "All Valid instance "},
        {eQCWWAN_ERR_NONE, 7, &v4Elmnt, &v6Elmnt, "All Valid instance "},
        {eQCWWAN_ERR_NONE, 8, &v4Elmnt, &v6Elmnt, "All Valid instance "},
        {eQCWWAN_ERR_NONE, 9, &v4Elmnt, &v6Elmnt, "All Valid instance "},
    };
    fp = tfopen("../../wds/test/results/getByteTotals.txt","w");

    if (fp)
    {
        BYTE tc = 0;
        BYTE  ncases = sizeof( SLQSGetByteTotalsTestCases )/
                       sizeof( SLQSGetByteTotalsTestCases[0]);
        while ( tc < ncases )
        {
            SLQSGetByteTotalsTestCaseStep_t *pd = &SLQSGetByteTotalsTestCases[tc++];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tc,
                        ncases,
                        pd->desc);
            qmiInst  = (tc-1)/3;
            sessType = (tc-1)%3;

            fprintf( fp, "\nQMI Instance: %x, Session Type: %x \n",
                                 qmiInst,
                                 sessType );

            /* Initialize session ID to NULL */
            lByteTotals.pV4sessionId       = NULL;
            lByteTotals.pV6sessionId       = NULL;
            lByteTotals.ByteTotalsElmntsV4 =*(pd->vp4Elemnt);
            lByteTotals.ByteTotalsElmntsV6 =*(pd->vp6Elemnt);
            /* IPv4 */
            if ( 0 == sessType )
            {
                if( ZERO_SESSION_ID != s4[qmiInst].v4sessionId )
                {
                    lByteTotals.pV4sessionId = &s4[qmiInst].v4sessionId;
                }
                lByteTotals.pV6sessionId = NULL;
            }

            /* IPv6 */
            if ( 1 == sessType )
            {
                lByteTotals.pV4sessionId = NULL;
                if( ZERO_SESSION_ID != s6[qmiInst].v6sessionId )
                {
                    lByteTotals.pV6sessionId = &s6[qmiInst].v6sessionId;
                }
            }

            /* IPv4v6 */
            if ( 2 == sessType )
            {
                if( ZERO_SESSION_ID != s46[qmiInst].v4sessionId )
                {
                    lByteTotals.pV4sessionId = &s46[qmiInst].v4sessionId;
                }
                if( ZERO_SESSION_ID != s46[qmiInst].v6sessionId )
                {
                    lByteTotals.pV6sessionId = &s46[qmiInst].v6sessionId;
                }
            }

            ULONG nRet = SLQSGetByteTotals( &lByteTotals );

            /* Display result code and text */
            doprintreason (fp, nRet);

            if (!nRet)
            {
                IFPRINTF( fp, "Bytes total RX (v4) : %llx \n",lByteTotals.ByteTotalsElmntsV4.pRXTotalBytes );
                IFPRINTF( fp, "Bytes total TX (v4) : %llx \n",lByteTotals.ByteTotalsElmntsV4.pTXTotalBytes );

                IFPRINTF( fp, "Bytes total RX (v6) : %llx \n",lByteTotals.ByteTotalsElmntsV6.pRXTotalBytes);
                IFPRINTF( fp, "Bytes total TX (v6) : %llx \n",lByteTotals.ByteTotalsElmntsV6.pTXTotalBytes);
            }
        }
   }

   if (fp)
       tfclose(fp);
}
