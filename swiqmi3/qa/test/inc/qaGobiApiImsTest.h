/**************
 *
 *  Filename: qaGobiApiImsTest.h
 *
 *  Purpose:  Contains prototypes for functions implementing specific
 *            tests for Ims Service
 *            called by the main() routine in qatest.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc., all rights reserved
 *
 **************/

#ifndef __GOBI_API_IMS_TEST_H__
#define __GOBI_API_IMS_TEST_H__

typedef struct _SLQSSetSIPConfigTestCase_t
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    WORD              *pSIPLocalPort;
    ULONG             *pTimerSIPReg;
    ULONG             *pSubscribeTimer;
    ULONG             *pTimerT1;
    ULONG             *pTimerT2;
    ULONG             *pTimerTf;
    BYTE              *pSigCompEnabled;
    SetSIPConfigResp  *pSetSIPConfigResp;
    CHAR              desc[100];
} SLQSSetSIPConfigTestCase_t;

typedef struct _SLQSSetRegMgrConfigTestCase_t
{
    enum eQCWWANError   expectedReturnValue;
    ULONG               testCaseId;
    WORD                *pPriCSCFPort;
    BYTE                *pCSCFPortNameLen;
    BYTE                *pCSCFPortName;
    BYTE                *pIMSTestMode;
    SetRegMgrConfigResp *pSetRegMgrConfigResp;
    CHAR                desc[100];
} SLQSSetRegMgrConfigTestCase_t;

typedef struct _SLQSSetIMSSMSConfigTestCase_t
{
    enum eQCWWANError   expectedReturnValue;
    ULONG               testCaseId;
    BYTE                *pSMSFormat;
    BYTE                *pSMSOverIPNwInd;
    BYTE                *pPhoneCtxtURILen;
    BYTE                *pPhoneCtxtURI;
    SetIMSSMSConfigResp *pSetIMSSMSConfigResp;
    CHAR                desc[100];
} SLQSSetIMSSMSConfigTestCase_t;

typedef struct _SLQSSetIMSUserConfigTestCase_t
{
    enum eQCWWANError    expectedReturnValue;
    ULONG                testCaseId;
    BYTE                 *pIMSDomainNameLen;
    BYTE                 *pIMSDomainName;
    SetIMSUserConfigResp *pSetIMSUserConfigResp;
    CHAR                 desc[100];
} SLQSSetIMSUserConfigTestCase_t;

typedef struct _SLQSSetIMSVoIPConfigTestCase_t
{
    enum eQCWWANError    expectedReturnValue;
    ULONG                testCaseId;
    WORD                 *pSessionExpiryTimer;
    WORD                 *pMinSessionExpiryTimer;
    BYTE                 *pAmrWbEnable;
    BYTE                 *pScrAmrEnable;
    BYTE                 *pScrAmrWbEnable;
    BYTE                 *pAmrMode;
    WORD                 *pAmrWBMode;
    BYTE                 *pAmrOctetAligned;
    BYTE                 *pAmrWBOctetAligned;
    WORD                 *pRingingTimer;
    WORD                 *pRingBackTimer;
    WORD                 *pRTPRTCPInactTimer;
    SetIMSVoIPConfigResp *pSetIMSVoIPConfigResp;
    CHAR                 desc[100];
} SLQSSetIMSVoIPConfigTestCase_t;

typedef struct _SLQSGetSIPConfigTestCase_t
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    GetSIPConfigResp  *pGetSIPConfigResp;
    CHAR              desc[100];
} SLQSGetSIPConfigTestCase_t;

typedef struct _SLQSGetRegMgrConfigTestCase_t
{
    enum eQCWWANError     expectedReturnValue;
    ULONG                 testCaseId;
    GetRegMgrConfigParams *pGetRegMgrConfigParams;
    CHAR                  desc[100];
} SLQSGetRegMgrConfigTestCase_t;

typedef struct _SLQSGetIMSSMSConfigTestCase_t
{
    enum eQCWWANError     expectedReturnValue;
    ULONG                 testCaseId;
    GetIMSSMSConfigParams *pGetIMSSMSConfigParams;
    CHAR                  desc[100];
} SLQSGetIMSSMSConfigTestCase_t;

typedef struct _SLQSGetIMSUserConfigTestCase_t
{
    enum eQCWWANError      expectedReturnValue;
    ULONG                  testCaseId;
    GetIMSUserConfigParams *pGetIMSUserConfigParams;
    CHAR                   desc[100];
} SLQSGetIMSUserConfigTestCase_t;

typedef struct _SLQSGetIMSVoIPConfigTestCase_t
{
    enum eQCWWANError    expectedReturnValue;
    ULONG                testCaseId;
    GetIMSVoIPConfigResp *pGetIMSVoIPConfigResp;
    CHAR                 desc[100];
} SLQSGetIMSVoIPConfigTestCase_t;

typedef struct _SLQSIMSCfgIndRegisterTestCase_t
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    BYTE              *pSIPConfigEvents;
    BYTE              *pRegMgrConfigEvents;
    BYTE              *pSMSConfigEvents;
    BYTE              *pUserConfigEvents;
    BYTE              *pVoIPConfigEvents;
    CHAR              desc[100];
} SLQSIMSCfgIndRegisterTestCase_t;

#endif /* __GOBI_API_IMS_TEST_H__ */
