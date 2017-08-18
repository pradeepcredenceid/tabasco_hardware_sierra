/**************
 *
 *  Filename: qaGobiApiImsTest.c
 *
 *  Purpose:  Contains functions implementing specific tests for
 *            Ims Service called by the main() routine in qatest.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc., all rights reserved
 *
 **************/

/* Linux definitions */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* include files */
#include "SwiDataTypes.h"
#include "qmerrno.h"

#include "qaGobiApiIms.h"
#include "qaGobiApiImsTest.h"
#include "qatesthelper.h"
#include "qatestproto.h"

/*******************
    TEST DATA
 *******************/

/* Constants */

/* Global Variables */

/* Indication Register Parameters */
local BYTE SIPConfigEvent = 0x01;
local BYTE RegMgrCfgEvent = 0x01;
local BYTE SmsCfgEvent    = 0x01;
local BYTE UserCfgEvent   = 0x01;
local BYTE VoipCfgEvent   = 0x01;

/* Forward declaration */


/*******************
  TEST FUNCTIONS
 *******************/
/*
 * Name:     doSLQSSetSIPConfig
 *
 * Purpose:  Perform the tests that call the SLQSSetSIPConfig() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSSetSIPConfig( void )
{
    WORD SIPPortNum   = 0x00;
    ULONG SIPRegTime  = 0x1000;
    ULONG SubscrTimer = 0x1000;
    ULONG TimerT1     = 0x1000;
    ULONG TimerT2     = 0x1000;
    ULONG TimerTf     = 0x1000;
    BYTE sigCompEn    = 0x01;
    BYTE sigCompDs    = 0x00;
    BYTE sigCompInv   = 0x02;
    BYTE settingsResp = 0xFF;

    SetSIPConfigResp tempResp;

    SLQSSetSIPConfigTestCase_t SLQSSetSIPConfigTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, &SIPPortNum, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, "Valid - SIP Port Number" },

        { eQCWWAN_ERR_NONE, 2, NULL, &SIPRegTime, &SubscrTimer, NULL,
          NULL, NULL, NULL, &tempResp,
          "Valid - SIP Registration timer, response" },

        { eQCWWAN_ERR_NONE, 3, NULL, NULL, &SubscrTimer, NULL,
          NULL, NULL, NULL, &tempResp,
          "Valid - Subscription timer, response" },

        { eQCWWAN_ERR_NONE, 4, NULL, NULL, NULL, &TimerT1,
          NULL, NULL, NULL, &tempResp,
          "Valid - Timer T1, response" },

        { eQCWWAN_ERR_NONE, 5, NULL, NULL, NULL, NULL,
          &TimerT2, NULL, NULL, &tempResp,
          "Valid - Timer T2, response" },

        { eQCWWAN_ERR_NONE, 6, NULL, NULL, NULL, NULL,
          NULL, &TimerTf, NULL, &tempResp,
          "Valid - Timer Tf, response" },

        { eQCWWAN_ERR_NONE, 7, NULL, NULL, NULL, NULL,
          NULL, NULL, &sigCompEn, &tempResp,
          "Valid - SigComp - Enabled, response" },

        { eQCWWAN_ERR_NONE, 8, NULL, NULL, NULL, NULL,
          NULL, NULL, &sigCompDs, &tempResp,
          "Valid - SigComp - Disabled, response" },

        { eQCWWAN_ERR_NONE, 9, NULL, NULL, NULL, NULL,
          NULL, NULL, &sigCompInv, &tempResp,
          "Invalid - SigComp - Invalid, response" },

        { eQCWWAN_ERR_NONE, 10, &SIPPortNum, &SIPRegTime, &SubscrTimer,
          &TimerT1, &TimerT2, &TimerTf, &sigCompEn, &tempResp,
          "All Valid" },
    };

    ULONG nRet;
    SetSIPConfigReq  req;
    SetSIPConfigResp *pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../ims/test/results/slqssetsipconfig.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSSetSIPConfigTestCases )/
                      sizeof( SLQSSetSIPConfigTestCases[0] );

        while ( tc < ncases )
        {
            SLQSSetSIPConfigTestCase_t *pd =
                                     &SLQSSetSIPConfigTestCases[tc];
            /* Initialize request parameters */
            req.pSIPLocalPort   = pd->pSIPLocalPort;
            req.pTimerSIPReg    = pd->pTimerSIPReg;
            req.pSubscribeTimer = pd->pSubscribeTimer;
            req.pTimerT1        = pd->pTimerT1;
            req.pTimerT2        = pd->pTimerT2;
            req.pTimerTf        = pd->pTimerTf;
            req.pSigCompEnabled = pd->pSigCompEnabled;


            pResp               = pd->pSetSIPConfigResp;
            /* Initialize response parameters */
            if( NULL != pResp )
            {
                pResp->pSettingResp = &settingsResp;
            }
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "Parameters Set:\n" );
            IFPRINTF( fp, "SIP Local Port         :0x%x\n", req.pSIPLocalPort );
            IFPRINTF( fp, "Timer SIP Registration :0x%lx\n", req.pTimerSIPReg );
            IFPRINTF( fp, "Subscribe Timer        :0x%lx\n", req.pSubscribeTimer );
            IFPRINTF( fp, "Timer T1               :0x%lx\n", req.pTimerT1 );
            IFPRINTF( fp, "Timer T2               :0x%lx\n", req.pTimerT2 );
            IFPRINTF( fp, "Timer Tf               :0x%lx\n", req.pTimerTf );
            IFPRINTF( fp, "Sig Comp Enabled       :0x%x\n", req.pSigCompEnabled );

            nRet = SLQSSetSIPConfig( &req, pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "IMS SIP configuration parameters successfully set\n" );
                IFPRINTF( fp, "Settings Response   :0x%x\n", pResp->pSettingResp );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

/*
 * Name:     doSLQSSetRegMgrConfig
 *
 * Purpose:  Perform the tests that call the SLQSSetRegMgrConfig() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSSetRegMgrConfig( void )
{
    WORD PriCSCFPort      = 0x00;
    BYTE CSCFPortNameLen  = 0x06;
    BYTE CSCFPortName[10] = { 'S', 'i', 'e', 'r', 'r', 'a' };
    BYTE IMSTestEn        = 0x01;
    BYTE IMSTestDis       = 0x00;
    BYTE IMSTestInv       = 0x05;
    BYTE settingsResp     = 0xFF;

    SetRegMgrConfigResp tempResp;

    SLQSSetRegMgrConfigTestCase_t SLQSSetRegMgrConfigTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, &PriCSCFPort, NULL, NULL, NULL,
          NULL, "Valid - Pri CSCF Port" },

        { eQCWWAN_ERR_NONE, 2, NULL, &CSCFPortNameLen, NULL, NULL,
          &tempResp, "Valid - CSCF Port Name Length, response" },

        { eQCWWAN_ERR_NONE, 3, NULL, NULL, CSCFPortName, NULL,
          &tempResp, "Valid - CSCF Port Name, response" },

        { eQCWWAN_ERR_NONE, 4, NULL, &CSCFPortNameLen, CSCFPortName,
          NULL, &tempResp, "Valid - CSCF Port Length and Name, response" },

        { eQCWWAN_ERR_NONE, 5, NULL, NULL, NULL, &IMSTestEn,
          &tempResp, "Valid - IMS Test Enable, response" },

        { eQCWWAN_ERR_NONE, 6, NULL, NULL, NULL, &IMSTestDis,
          &tempResp, "Valid - IMS Test Disable, response" },

        { eQCWWAN_ERR_NONE, 7, NULL, NULL, NULL, &IMSTestInv,
          &tempResp, "Invalid - IMS Test, response" },

        { eQCWWAN_ERR_NONE, 8, &PriCSCFPort, &CSCFPortNameLen, CSCFPortName,
          &IMSTestEn, &tempResp, "All Valid" },
    };

    ULONG nRet;
    SetRegMgrConfigReq  req;
    SetRegMgrConfigResp *pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../ims/test/results/slqssetregmgrconfig.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSSetRegMgrConfigTestCases )/
                      sizeof( SLQSSetRegMgrConfigTestCases[0] );

        while ( tc < ncases )
        {
            SLQSSetRegMgrConfigTestCase_t *pd =
                                     &SLQSSetRegMgrConfigTestCases[tc];

            /* Initialize request parameters */
            req.pPriCSCFPort     = pd->pPriCSCFPort;
            req.pCSCFPortNameLen = pd->pCSCFPortNameLen;
            req.pCSCFPortName    = pd->pCSCFPortName;
            req.pIMSTestMode     = pd->pIMSTestMode;

            pResp                = pd->pSetRegMgrConfigResp;

            /* Initialize response parameters */
            if( NULL != pResp )
            {
                pResp->pSettingResp = &settingsResp;
            }
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "Parameters Set:\n" );
            IFPRINTF( fp, "Primary CSCF Port :0x%x\n", req.pPriCSCFPort );
            IFPRINTF( fp, "CSCF Port Length  :0x%x\n", req.pCSCFPortNameLen );
            VFPRINTF( fp, "CSCF Port Name    :%s\n", req.pCSCFPortName );
            IFPRINTF( fp, "IMS Test Enabled  :0x%x\n", req.pIMSTestMode );

            nRet = SLQSSetRegMgrConfig( &req, pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "IMS Registration Manager configuration parameters successfully set\n" );
                IFPRINTF( fp, "Settings Response :0x%x\n", pResp->pSettingResp );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

/*
 * Name:     doSLQSSetIMSSMSConfig
 *
 * Purpose:  Perform the tests that call the SLQSSetIMSSMSConfig() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSSetIMSSMSConfig( void )
{
    BYTE SMSFormat3GPP     = 0x00;
    BYTE SMSFormat3GPP2    = 0x01;
    BYTE SMSFormat3GPPInv  = 0x02;

    BYTE PhCtxtURILen  = 0x06;
    BYTE PhCtxtURI[10] = { 'S', 'i', 'e', 'r', 'r', 'a' };
    BYTE SMSIPNw       = 0x01;
    BYTE SMSIPNwDis    = 0x00;
    BYTE SMSIPNwInv    = 0x05;
    BYTE settingsResp  = 0xFF;

    SetIMSSMSConfigResp tempResp;

    SLQSSetIMSSMSConfigTestCase_t SLQSSetIMSSMSConfigTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, &SMSFormat3GPP, NULL, NULL, NULL,
          NULL, "Valid - SMS Format 3GPP" },

        { eQCWWAN_ERR_NONE, 2, &SMSFormat3GPP, NULL, NULL, NULL,
          &tempResp, "Valid - SMS Format 3GPP, response" },

        { eQCWWAN_ERR_NONE, 3, &SMSFormat3GPP2, NULL, NULL, NULL,
          &tempResp, "Valid - SMS Format 3GPP2, response" },

        { eQCWWAN_ERR_INVALID_ARG, 4, &SMSFormat3GPPInv, NULL, NULL, NULL,
          &tempResp, "InValid - SMS Format, Valid - response" },

        { eQCWWAN_ERR_NONE, 5, NULL, &SMSIPNw, NULL, NULL,
          &tempResp, "Valid - SMS Over IP Network Indication(TRUE), response" },

        { eQCWWAN_ERR_NONE, 6, NULL, &SMSIPNwDis, NULL, NULL,
          &tempResp, "Valid - SMS Over IP Network Indication(FALSE), response" },

        { eQCWWAN_ERR_INVALID_ARG, 7, NULL, &SMSIPNwInv, NULL, NULL,
          &tempResp, "InValid - SMS Over IP Network Indication, Valid - response" },

        { eQCWWAN_ERR_NONE, 8, NULL, NULL, &PhCtxtURILen, NULL,
          &tempResp, "Valid - Phone Context URI Length, response" },

        { eQCWWAN_ERR_NONE, 9, NULL, NULL, NULL, PhCtxtURI,
          &tempResp, "Valid - Phone Context URI, response" },

        { eQCWWAN_ERR_NONE, 10, NULL, NULL, &PhCtxtURILen, PhCtxtURI,
           &tempResp, "Valid - Phone Context Length and Name, response" },

        { eQCWWAN_ERR_NONE, 11, &SMSFormat3GPP, &SMSIPNw, &PhCtxtURILen,
          PhCtxtURI, &tempResp, "All Valid" },
    };

    ULONG nRet;
    SetIMSSMSConfigReq  req;
    SetIMSSMSConfigResp *pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../ims/test/results/slqssetimssmsconfig.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSSetIMSSMSConfigTestCases )/
                      sizeof( SLQSSetIMSSMSConfigTestCases[0] );

        while ( tc < ncases )
        {
            SLQSSetIMSSMSConfigTestCase_t *pd =
                                     &SLQSSetIMSSMSConfigTestCases[tc];

            /* Initialize request parameters */
            req.pSMSFormat       = pd->pSMSFormat;
            req.pSMSOverIPNwInd  = pd->pSMSOverIPNwInd;
            req.pPhoneCtxtURILen = pd->pPhoneCtxtURILen;
            req.pPhoneCtxtURI    = pd->pPhoneCtxtURI;

            pResp                = pd->pSetIMSSMSConfigResp;

            /* Initialize response parameters */
            if( NULL != pResp )
            {
                pResp->pSettingResp = &settingsResp;
            }
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "Parameters Set:\n" );
            IFPRINTF( fp, "SMS Format :0x%x\n", req.pSMSFormat );
            IFPRINTF( fp, "SMS Over IP Network Indication :0x%x\n", req.pSMSOverIPNwInd );
            IFPRINTF( fp, "Phone Context URI length  :0x%x\n", req.pPhoneCtxtURILen );
            VFPRINTF( fp, "Phone Context URI    :%s\n", req.pPhoneCtxtURI );

            nRet = SLQSSetIMSSMSConfig( &req, pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "IMS SMS configuration parameters successfully set\n" );
                IFPRINTF( fp, "Settings Response :0x%x\n", pResp->pSettingResp );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

/*
 * Name:     doSLQSSetIMSUserConfig
 *
 * Purpose:  Perform the tests that call the SLQSSetIMSUserConfig() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSSetIMSUserConfig( void )
{
    BYTE IMSDomainNameLen  = 0x06;
    BYTE IMSDomainName[10] = { 'S', 'i', 'e', 'r', 'r', 'a' };

    BYTE settingsResp  = 0xFF;

    SetIMSUserConfigResp tempResp;

    SLQSSetIMSUserConfigTestCase_t SLQSSetIMSUserConfigTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, NULL, NULL, NULL,
          "All parameters NULL" },

        { eQCWWAN_ERR_NONE, 2, &IMSDomainNameLen, NULL,
          &tempResp, "Valid Domain Length, Domain Name NULL" },

        { eQCWWAN_ERR_NONE, 3, &IMSDomainNameLen, IMSDomainName,
          &tempResp, "All Valid" },
    };

    ULONG nRet;
    SetIMSUserConfigReq  req;
    SetIMSUserConfigResp *pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../ims/test/results/slqssetimsuserconfig.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSSetIMSUserConfigTestCases )/
                      sizeof( SLQSSetIMSUserConfigTestCases[0] );

        while ( tc < ncases )
        {
            SLQSSetIMSUserConfigTestCase_t *pd =
                                     &SLQSSetIMSUserConfigTestCases[tc];

            /* Initialize request parameters */
            req.pIMSDomainLen = pd->pIMSDomainNameLen;
            req.pIMSDomain    = pd->pIMSDomainName;

            pResp             = pd->pSetIMSUserConfigResp;

            /* Initialize response parameters */
            if( NULL != pResp )
            {
                pResp->pSettingResp = &settingsResp;
            }
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "Parameters Set:\n" );
            IFPRINTF( fp, "IMS Domain Name length :0x%x\n", req.pIMSDomainLen );
            VFPRINTF( fp, "IMS Domain Name        :%s\n", req.pIMSDomain );

            nRet = SLQSSetIMSUserConfig( &req, pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "IMS User configuration parameters successfully set\n" );
                IFPRINTF( fp, "Settings Response :0x%x\n", pResp->pSettingResp );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

/*
 * Name:     doSLQSSetIMSVoIPConfig
 *
 * Purpose:  Perform the tests that call the SLQSSetIMSVoIPConfig() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSSetIMSVoIPConfig( void )
{
    WORD SessionExpiryTimer = 0x100;
    WORD MinSessionExpiryTimer = 0x10;
    BYTE AmrWbEnable = 0x01;
    BYTE ScrAmrEnable = 0x01;
    BYTE ScrAmrWbEnable = 0x01;
    BYTE AmrMode = 0xFF;
    WORD AmrWBMode = 0xFF;
    BYTE AmrOctetAligned = 0x01;
    BYTE AmrWBOctetAligned = 0x01;
    WORD RingingTimer = 0x40;
    WORD RingBackTimer = 0x40;
    WORD RTPRTCPInactTimer = 0x40;

    BYTE settingsResp  = 0xFF;

    SetIMSVoIPConfigResp tempResp;

    SLQSSetIMSVoIPConfigTestCase_t SLQSSetIMSVoIPConfigTestCases[] =
    {
        { eQCWWAN_ERR_INVALID_ARG, 1, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, NULL, "All parameters NULL" },

        { eQCWWAN_ERR_NONE, 2, &SessionExpiryTimer, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, &tempResp,
          "Valid Session Expiry Timer, Response - Rest NULL" },

        { eQCWWAN_ERR_NONE, 3, NULL, &MinSessionExpiryTimer, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, &tempResp,
          "Valid Min Session Expiry Timer, Response - Rest NULL" },

        { eQCWWAN_ERR_NONE, 4, NULL, NULL, &AmrWbEnable, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, &tempResp,
          "Valid Min Session Expiry Timer, Response - Rest NULL" },

        { eQCWWAN_ERR_NONE, 5, NULL, NULL, NULL, &ScrAmrEnable, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, &tempResp,
          "Valid SCR AMR Enable, Response - Rest NULL" },

        { eQCWWAN_ERR_NONE, 6, NULL, NULL, NULL, NULL, &ScrAmrWbEnable, NULL,
          NULL, NULL, NULL, NULL, NULL, NULL, &tempResp,
          "Valid SCR AMR WB Enable, Response - Rest NULL" },

        { eQCWWAN_ERR_NONE, 7, NULL, NULL, NULL, NULL, NULL, &AmrMode,
          NULL, NULL, NULL, NULL, NULL, NULL, &tempResp,
          "Valid AMR Mode, Response - Rest NULL" },

        { eQCWWAN_ERR_NONE, 8, NULL, NULL, NULL, NULL, NULL, NULL,
          &AmrWBMode, NULL, NULL, NULL, NULL, NULL, &tempResp,
          "Valid AMR WB Mode, Response - Rest NULL" },

        { eQCWWAN_ERR_NONE, 9, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, &AmrOctetAligned, NULL, NULL, NULL, NULL, &tempResp,
          "Valid AMR Octet Aligned, Response - Rest NULL" },

        { eQCWWAN_ERR_NONE, 10, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, &AmrWBOctetAligned, NULL, NULL, NULL, &tempResp,
          "Valid AMR WB Octet Aligned, Response - Rest NULL" },

        { eQCWWAN_ERR_NONE, 11, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, &RingingTimer, NULL, NULL, &tempResp,
          "Valid Ringing Timer, Response - Rest NULL" },

        { eQCWWAN_ERR_NONE, 12, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, &RingBackTimer, NULL, &tempResp,
          "Valid Ring Back Timer, Response - Rest NULL" },

        { eQCWWAN_ERR_NONE, 13, NULL, NULL, NULL, NULL, NULL, NULL,
          NULL, NULL, NULL, NULL, NULL, &RTPRTCPInactTimer, &tempResp,
          "Valid RTP RTCP Inactivity Timer, Response - Rest NULL" },

        { eQCWWAN_ERR_NONE, 14, &SessionExpiryTimer, &MinSessionExpiryTimer,
          &AmrWbEnable, &ScrAmrEnable, &ScrAmrWbEnable, &AmrMode, &AmrWBMode,
          &AmrOctetAligned, &AmrWBOctetAligned, &RingingTimer, &RingBackTimer,
          &RTPRTCPInactTimer, &tempResp,
          "Valid RTP RTCP Inactivity Timer, Response - Rest NULL" },
    };

    ULONG nRet;
    SetIMSVoIPConfigReq  req;
    SetIMSVoIPConfigResp *pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../ims/test/results/slqssetimsvoipconfig.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSSetIMSVoIPConfigTestCases )/
                      sizeof( SLQSSetIMSVoIPConfigTestCases[0] );

        while ( tc < ncases )
        {
            SLQSSetIMSVoIPConfigTestCase_t *pd =
                                     &SLQSSetIMSVoIPConfigTestCases[tc];

            /* Initialize request parameters */
            req.pSessionExpiryTimer    = pd->pSessionExpiryTimer;
            req.pMinSessionExpiryTimer = pd->pMinSessionExpiryTimer;
            req.pAmrWbEnable           = pd->pAmrWbEnable;
            req.pScrAmrEnable          = pd->pScrAmrEnable;
            req.pScrAmrWbEnable        = pd->pScrAmrWbEnable;
            req.pAmrMode               = pd->pAmrMode;
            req.pAmrWBMode             = pd->pAmrWBMode;
            req.pAmrOctetAligned       = pd->pAmrOctetAligned;
            req.pAmrWBOctetAligned     = pd->pAmrWBOctetAligned;
            req.pRingingTimer          = pd->pRingingTimer;
            req.pRingBackTimer         = pd->pRingBackTimer;
            req.pRTPRTCPInactTimer     = pd->pRTPRTCPInactTimer;

            pResp = pd->pSetIMSVoIPConfigResp;

            /* Initialize response parameters */
            if( NULL != pResp )
            {
                pResp->pSettingResp = &settingsResp;
            }
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "Parameters Set:\n" );
            IFPRINTF( fp, "Session Expiry Timer :0x%x\n", req.pSessionExpiryTimer );
            IFPRINTF( fp, "Min. Session Expiry  :0x%x\n", req.pMinSessionExpiryTimer );
            IFPRINTF( fp, "AMR WB Enable  :0x%x\n", req.pAmrWbEnable );
            IFPRINTF( fp, "SCR AMR Enable  :0x%x\n", req.pScrAmrEnable );
            IFPRINTF( fp, "SCR AMR WB Enable  :0x%x\n", req.pScrAmrWbEnable );
            IFPRINTF( fp, "AMR Mode  :0x%x\n", req.pAmrMode );
            IFPRINTF( fp, "AMR WB Mode  :0x%x\n", req.pAmrWBMode );
            IFPRINTF( fp, "AMR Octet Aligned  :0x%x\n", req.pAmrOctetAligned );
            IFPRINTF( fp, "AMR WB Octet Aligned  :0x%x\n", req.pAmrWBOctetAligned );
            IFPRINTF( fp, "Ringing Timer  :0x%x\n", req.pRingingTimer );
            IFPRINTF( fp, "Ring Back Timer  :0x%x\n", req.pRingBackTimer );
            IFPRINTF( fp, "RTP/RTCP Inactivity Timer  :0x%x\n", req.pRTPRTCPInactTimer );

            nRet = SLQSSetIMSVoIPConfig( &req, pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "IMS VoIP configuration parameters successfully set\n" );
                IFPRINTF( fp, "Settings Response :0x%x\n", pResp->pSettingResp );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

/*
 * Name:     doSLQSGetSIPConfig
 *
 * Purpose:  Perform the tests that call the SLQSGetSIPConfig() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetSIPConfig( void )
{
    BYTE  settingsResp   = 0xFF;
    WORD  SIPLocalPort   = 0xFFFF;
    ULONG TimerSIPReg    = 0xFFFFFFFF;
    ULONG SubscribeTimer = 0xFFFFFFFF;
    ULONG TimerT1        = 0xFFFFFFFF;
    ULONG TimerT2        = 0xFFFFFFFF;
    ULONG TimerTf        = 0xFFFFFFFF;
    BYTE  SigCompEnabled = 0xFF;

    GetSIPConfigResp tempResp;

    SLQSGetSIPConfigTestCase_t SLQSGetSIPConfigTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, &tempResp, "Valid Test Case" },
    };

    ULONG nRet;
    GetSIPConfigResp *pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../ims/test/results/slqsgetsipconfig.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSGetSIPConfigTestCases )/
                      sizeof( SLQSGetSIPConfigTestCases[0] );

        while ( tc < ncases )
        {
            SLQSGetSIPConfigTestCase_t *pd =
                                     &SLQSGetSIPConfigTestCases[tc];

            pResp = pd->pGetSIPConfigResp;

            /* Initialize response parameters */
            if( NULL != pResp )
            {
                pResp->pSettingResp    = &settingsResp;
                pResp->pSIPLocalPort   = &SIPLocalPort;
                pResp->pTimerSIPReg    = &TimerSIPReg;
                pResp->pSubscribeTimer = &SubscribeTimer;
                pResp->pTimerT1        = &TimerT1;
                pResp->pTimerT2        = &TimerT2;
                pResp->pTimerTf        = &TimerTf;
                pResp->pSigCompEnabled = &SigCompEnabled;
            }
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            nRet = SLQSGetSIPConfig( pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "IMS SIP configuration parameters successfully retrieved\n" );
                IFPRINTF( fp, "Settings Response :0x%x\n", pResp->pSettingResp );
                IFPRINTF( fp, "SIP Local Port    :0x%x\n", pResp->pSIPLocalPort );
                IFPRINTF( fp, "Timer SIP Regn.   :0x%lx\n", pResp->pTimerSIPReg );
                IFPRINTF( fp, "Subscribe Timer   :0x%lx\n", pResp->pSubscribeTimer );
                IFPRINTF( fp, "Timer T1          :0x%lx\n", pResp->pTimerT1 );
                IFPRINTF( fp, "Timer T2          :0x%lx\n", pResp->pTimerT2 );
                IFPRINTF( fp, "Timer Tf          :0x%lx\n", pResp->pTimerTf );
                IFPRINTF( fp, "SigComp Status    :0x%x\n", pResp->pSigCompEnabled );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

/*
 * Name:     doSLQSGetRegMgrConfig
 *
 * Purpose:  Perform the tests that call the SLQSGetRegMgrConfig() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetRegMgrConfig( void )
{
    BYTE settingsResp = 0xFF;
    WORD ProxyCSCF    = 0xFFFF;
    BYTE PriCSCFLen   = 0xFF;
    BYTE PriCSCFName[255];
    BYTE IMSTestMode = 0xFF;

    GetRegMgrConfigParams tempResp;

    SLQSGetRegMgrConfigTestCase_t SLQSGetRegMgrConfigTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, &tempResp, "Valid Test Case" },
    };

    ULONG nRet;
    GetRegMgrConfigParams *pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../ims/test/results/slqsgetregmgrconfig.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSGetRegMgrConfigTestCases )/
                      sizeof( SLQSGetRegMgrConfigTestCases[0] );

        while ( tc < ncases )
        {
            SLQSGetRegMgrConfigTestCase_t *pd =
                                     &SLQSGetRegMgrConfigTestCases[tc];

            pResp = pd->pGetRegMgrConfigParams;

            /* Initialize response parameters */
            if( NULL != pResp )
            {
                pResp->pSettingResp        = &settingsResp;
                pResp->pPCSCFPort          = &ProxyCSCF;
                pResp->pPriCSCFPortNameLen = &PriCSCFLen;
                pResp->pPriCSCFPortName    = PriCSCFName;
                pResp->pIMSTestMode        = &IMSTestMode;
            }
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "Parameters Set:\n" );
            IFPRINTF( fp, "Primary CSCF Port Name length :0x%x\n",
                      pResp->pPriCSCFPortNameLen );

            nRet = SLQSGetRegMgrConfig( pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "Registration Manager configuration parameters successfully retrieved\n" );
                IFPRINTF( fp, "Settings Response    :0x%x\n", pResp->pSettingResp );
                IFPRINTF( fp, "Proxy CSCF port      :0x%x\n", pResp->pPCSCFPort );
                VFPRINTF( fp, "Prim. CSCF port name :%s\n", pResp->pPriCSCFPortName );
                IFPRINTF( fp, "IMS Test Mode        :0x%x\n", pResp->pIMSTestMode );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

/*
 * Name:     doSLQSGetIMSSMSConfig
 *
 * Purpose:  Perform the tests that call the SLQSGetIMSSMSConfig() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetIMSSMSConfig( void )
{
    BYTE settingsResp = 0xFF;
    BYTE SMSFmt       = 0xFF;
    BYTE SMSoIPNIF    = 0xFF;
    BYTE PhCtxtURILen = 0xFF;
    BYTE PhCtxtURI[255];

    GetIMSSMSConfigParams tempResp;

    SLQSGetIMSSMSConfigTestCase_t SLQSGetIMSSMSConfigTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, &tempResp, "Valid Test Case" },
    };

    ULONG nRet;
    GetIMSSMSConfigParams *pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../ims/test/results/slqsgetimssmsconfig.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSGetIMSSMSConfigTestCases )/
                      sizeof( SLQSGetIMSSMSConfigTestCases[0] );

        while ( tc < ncases )
        {
            SLQSGetIMSSMSConfigTestCase_t *pd =
                                     &SLQSGetIMSSMSConfigTestCases[tc];

            pResp = pd->pGetIMSSMSConfigParams;

            /* Initialize response parameters */
            if( NULL != pResp )
            {
                pResp->pSettingResp     = &settingsResp;
                pResp->pSMSFormat       = &SMSFmt;
                pResp->pSMSOverIPNwInd  = &SMSoIPNIF;
                pResp->pPhoneCtxtURILen = &PhCtxtURILen;
                pResp->pPhoneCtxtURI    = PhCtxtURI;

            }
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "Parameters Set:\n" );
            IFPRINTF( fp, "Phone Context URI length :0x%x\n",
                      pResp->pPhoneCtxtURILen );

            nRet = SLQSGetIMSSMSConfig( pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "IMS SMS configuration parameters successfully retrieved\n" );
                IFPRINTF( fp, "Settings Response    :0x%x\n", pResp->pSettingResp );
                IFPRINTF( fp, "SMS Format           :0x%x\n", pResp->pSMSFormat );
                IFPRINTF( fp, "SMS over NW IP Ind.  :0x%x\n", pResp->pSMSOverIPNwInd );
                VFPRINTF( fp, "Phone Context URI    :%s\n", pResp->pPhoneCtxtURI );

            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

/*
 * Name:     doSLQSGetIMSUserConfig
 *
 * Purpose:  Perform the tests that call the SLQSGetIMSUserConfig() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetIMSUserConfig( void )
{
    BYTE settingsResp = 0xFF;
    BYTE IMSDomainLen = 0xFF;
    BYTE IMSDomain[255];

    GetIMSUserConfigParams tempResp;

    SLQSGetIMSUserConfigTestCase_t SLQSGetIMSUserConfigTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, &tempResp, "Valid Test Case" },
    };

    ULONG nRet;
    GetIMSUserConfigParams *pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../ims/test/results/slqsgetimsuserconfig.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSGetIMSUserConfigTestCases )/
                      sizeof( SLQSGetIMSUserConfigTestCases[0] );

        while ( tc < ncases )
        {
            SLQSGetIMSUserConfigTestCase_t *pd =
                                     &SLQSGetIMSUserConfigTestCases[tc];

            pResp = pd->pGetIMSUserConfigParams;

            /* Initialize response parameters */
            if( NULL != pResp )
            {
                pResp->pSettingResp  = &settingsResp;
                pResp->pIMSDomainLen = &IMSDomainLen;
                pResp->pIMSDomain    = IMSDomain;
            }
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            fprintf( fp, "Parameters Set:\n" );
            IFPRINTF( fp, "IMS Domain Name length :0x%x\n",
                      pResp->pIMSDomainLen );

            nRet = SLQSGetIMSUserConfig( pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "IMS User configuration parameters successfully retrieved\n" );
                IFPRINTF( fp, "Settings Response :0x%x\n", pResp->pSettingResp );
                VFPRINTF( fp, "IMS Domain Name   :%s\n", pResp->pIMSDomain );

            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

/*
 * Name:     doSLQSGetIMSVoIPConfig
 *
 * Purpose:  Perform the tests that call the SLQSGetIMSVoIPConfig() API
 *
 * Return:   none
 *
 * Notes:    none
 *
 */
void doSLQSGetIMSVoIPConfig( void )
{
    BYTE settingsResp    = 0xFF;
    WORD SessExpTimer    = 0xFFFF;
    WORD MinSessExpTmr   = 0xFFFF;
    BYTE AmrWbEnable     = 0xFF;
    BYTE ScrAmrEnable    = 0xFF;
    BYTE ScrAmrWbEnable  = 0xFF;
    BYTE AmrMode         = 0xFF;
    WORD AmrWbMode       = 0xFF;
    BYTE AmrOctetAlign   = 0xFF;
    BYTE AmrWbOctetAlign = 0xFF;
    WORD RingTmr         = 0xFFFF;
    WORD RingBkTmr       = 0xFFFF;
    WORD RTPRTCPInactTmr = 0xFFFF;

    GetIMSVoIPConfigResp tempResp;

    SLQSGetIMSVoIPConfigTestCase_t SLQSGetIMSVoIPConfigTestCases[] =
    {
        { eQCWWAN_ERR_NONE, 1, &tempResp, "Valid Test Case" },
    };

    ULONG nRet;
    GetIMSVoIPConfigResp *pResp;
    FILE  *fp = NULL;

    fp = tfopen( "../../ims/test/results/slqsgetimsvoipconfig.txt", "w" );

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSGetIMSVoIPConfigTestCases )/
                      sizeof( SLQSGetIMSVoIPConfigTestCases[0] );

        while ( tc < ncases )
        {
            SLQSGetIMSVoIPConfigTestCase_t *pd =
                                     &SLQSGetIMSVoIPConfigTestCases[tc];

            pResp = pd->pGetIMSVoIPConfigResp;

            /* Initialize response parameters */
            if( NULL != pResp )
            {
                pResp->pSettingResp           = &settingsResp;
                pResp->pSessionExpiryTimer    = &SessExpTimer;
                pResp->pMinSessionExpiryTimer = &MinSessExpTmr;
                pResp->pAmrWbEnable           = &AmrWbEnable;
                pResp->pScrAmrEnable          = &ScrAmrEnable;
                pResp->pScrAmrWbEnable        = &ScrAmrWbEnable;
                pResp->pAmrMode               = &AmrMode;
                pResp->pAmrWBMode             = &AmrWbMode;
                pResp->pAmrOctetAligned       = &AmrOctetAlign;
                pResp->pAmrWBOctetAligned     = &AmrWbOctetAlign;
                pResp->pRingingTimer          = &RingTmr;
                pResp->pRingBackTimer         = &RingBkTmr;
                pResp->pRTPRTCPInactTimer     = &RTPRTCPInactTmr;
            }
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        (tc + 1),
                        ncases,
                        pd->desc);

            nRet = SLQSGetIMSVoIPConfig( pResp );

            /* Display result code and text */
            doprintreason ( fp, nRet );

            if ( !nRet )
            {
                fprintf( fp, "IMS VOIP configuration parameters successfully retrieved\n" );
                IFPRINTF( fp, "Settings Response         :0x%x\n", pResp->pSettingResp );
                IFPRINTF( fp, "Session Expiry Timer      :0x%x\n", pResp->pSessionExpiryTimer );
                IFPRINTF( fp, "Min. Session Expiry Timer :0x%x\n", pResp->pMinSessionExpiryTimer );
                IFPRINTF( fp, "Enable AMR WB             :0x%x\n", pResp->pAmrWbEnable );
                IFPRINTF( fp, "SCR AMR Enable            :0x%x\n", pResp->pScrAmrEnable );
                IFPRINTF( fp, "SCR AMR WB Enable         :0x%x\n", pResp->pScrAmrWbEnable );
                IFPRINTF( fp, "AMR Mode                  :0x%x\n", pResp->pAmrMode );
                IFPRINTF( fp, "AMR WB Mode               :0x%x\n", pResp->pAmrWBMode );
                IFPRINTF( fp, "AMR Octet Aligned         :0x%x\n", pResp->pAmrOctetAligned );
                IFPRINTF( fp, "AMR WB Octet Aligned      :0x%x\n", pResp->pAmrWBOctetAligned );
                IFPRINTF( fp, "Ringing Timer             :0x%x\n", pResp->pRingingTimer );
                IFPRINTF( fp, "Ring Back Timer           :0x%x\n", pResp->pRingBackTimer );
                IFPRINTF( fp, "RTP/RTCP Inactivity Timer :0x%x\n", pResp->pRTPRTCPInactTimer );
            }
            tc++;
        }
    }
    if ( fp )
        tfclose( fp );
}

/*
 * Name:    doSLQSImsConfigIndicationRegister
 *
 * Purpose: Perform the tests that call the SLQSImsConfigIndicationRegister() API
 *
 * Return:  none
 *
 * Notes:   none
 *
 */
void doSLQSImsConfigIndicationRegister( void )
{
    /* Test Cases */
    SLQSIMSCfgIndRegisterTestCase_t SLQSIMSCfgIndRegisterTestCases[]=
    {
        { eQCWWAN_ERR_NONE, 1, NULL, NULL, NULL, NULL, NULL,
          "Error: At Least One Optional Parameter to be provided" },

        { eQCWWAN_ERR_NONE, 2, &SIPConfigEvent, NULL, NULL, NULL, NULL,
          "Set SIP Config Event" },

        { eQCWWAN_ERR_NONE, 3, NULL, &RegMgrCfgEvent, NULL, NULL, NULL,
          "Set Reg Mgr Config Event" },

        { eQCWWAN_ERR_NONE, 4, NULL, NULL, &SmsCfgEvent, NULL, NULL,
          "Set SMS Config Event" },

        { eQCWWAN_ERR_NONE, 5, NULL, NULL, NULL, &UserCfgEvent, NULL,
          "Set Reg Mgr Config Event" },

        { eQCWWAN_ERR_NONE, 6, NULL, NULL, NULL, NULL, &VoipCfgEvent,
          "Set SMS Config Event" },

        { eQCWWAN_ERR_NONE, 7, &SIPConfigEvent, &RegMgrCfgEvent, &SmsCfgEvent,
          &UserCfgEvent, &VoipCfgEvent, "Set All" },
    };

    FILE  *fp = tfopen("../../ims/test/results/slqsimscfgindregister.txt", "w");
    imsCfgIndRegisterInfo req;

    if (fp)
    {
        BYTE tc = 0;
        BYTE ncases = sizeof( SLQSIMSCfgIndRegisterTestCases )/
                      sizeof( SLQSIMSCfgIndRegisterTestCases[0] );
        while ( tc < ncases )
        {
            SLQSIMSCfgIndRegisterTestCase_t *pd =
                                    &SLQSIMSCfgIndRegisterTestCases[tc++];
            fprintf(fp, "\nTest Case %d / %d : %s\n",
                        tc,
                        ncases,
                        pd->desc);

            req.pSIPConfigEvents    = pd->pSIPConfigEvents;
            req.pRegMgrConfigEvents = pd->pRegMgrConfigEvents;
            req.pSMSConfigEvents    = pd->pSMSConfigEvents;
            req.pUserConfigEvents   = pd->pUserConfigEvents;
            req.pVoIPConfigEvents   = pd->pVoIPConfigEvents;

            IFPRINTF(fp, "Reg SIP Config Event          : %x\n",
                                                 req.pSIPConfigEvents);
            IFPRINTF(fp, "Reg Reg Mgr Config Event      : %x\n",
                                                 req.pRegMgrConfigEvents);
            IFPRINTF(fp, "Reg SMS Config Event          : %x\n",
                                                 req.pSMSConfigEvents);
            IFPRINTF(fp, "Reg User Config Event         : %x\n",
                                                 req.pUserConfigEvents);
            IFPRINTF(fp, "Reg VoIP Config Event         : %x\n",
                                                 req.pVoIPConfigEvents);

            ULONG nRet = SLQSImsConfigIndicationRegister( &req );
            doprintreason( fp, nRet );

            if ( eQCWWAN_ERR_NONE == nRet )
            {
                fprintf(fp, "SLQSImsConfigIndicationRegister Successful\n");
            }
        }
    }
    if (fp)
        tfclose(fp);
}

