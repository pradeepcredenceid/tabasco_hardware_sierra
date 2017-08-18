/**************
 *
 *  Filename: qaGobiApiAudioTest.h
 *
 *  Purpose:  Contains prototypes for functions implementing specific
 *            tests for Audio Service
 *            called by the main() routine in qatest.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc., all rights reserved
 *
 **************/

#ifndef __GOBI_API_AUDIO_TEST_H__
#define __GOBI_API_AUDIO_TEST_H__

typedef struct _SLQSGetAudioProfileTestCase_t
{
    enum eQCWWANError   expectedReturnValue;
    ULONG               testCaseId;
    BYTE                *pGenerator;
    GetAudioProfileResp *pGetAudioProfileResp;
    CHAR                desc[100];
} SLQSGetAudioProfileTestCase_t;

typedef struct _SLQSSetAudioProfileTestCase_t
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    BYTE              *pProfile;
    BYTE              *pEarMute;
    BYTE              *pMicMute;
    BYTE              *pGenerator;
    BYTE              *pVolume;
    CHAR              desc[100];
} SLQSSetAudioProfileTestCase_t;

typedef struct _SLQSGetAudioPathConfigTestCase_t
{
    enum eQCWWANError       expectedReturnValue;
    ULONG                   testCaseId;
    BYTE                    *pAudioProfile;
    BYTE                    *pItem;
    GetAudioPathConfigResp  *pGetAudioPathConfigResp;
    CHAR                    desc[100];
} SLQSGetAudioPathConfigTestCase_t;

typedef struct _SLQSSetAudioPathConfigTestCase_t
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    BYTE              *pProfile;
    BYTE              *pECMode;
    BYTE              *pNS;
    WORD              *pTXVoiceGain;
    WORD              *pDTMFTXGain;
    WORD              *pCODECSTGain;
    TXPCMIIRFltr      *pTXPCMIIRFLTR;
    RXPCMIIRFltr      *pRXPCMIIRFLTR;
    BYTE              *pRXAVCAGCSwitch;
    BYTE              *pTXAVCSwitch;
    RXAGCList         *pRXAGCList;
    RXAVCList         *pRXAVCList;
    TXAGCList         *pTXAGCList;
    CHAR              desc[100];
} SLQSSetAudioPathConfigTestCase_t;

typedef struct _SLQSGetAudioVolTLBConfigTestCase_t
{
    enum eQCWWANError        expectedReturnValue;
    ULONG                    testCaseId;
    BYTE                     *pAudioProfile;
    BYTE                     *pGenerator;
    BYTE                     *pVolume;
    BYTE                     *pItem;
    GetAudioVolTLBConfigResp *pGetAudioVolTLBConfigResp;
    CHAR                     desc[100];
} SLQSGetAudioVolTLBConfigTestCase_t;

typedef struct _SLQSSetAudioVolTLBConfigTestCase_t
{
    enum eQCWWANError        expectedReturnValue;
    ULONG                    testCaseId;
    BYTE                     *pAudioProfile;
    BYTE                     *pGenerator;
    BYTE                     *pVolume;
    BYTE                     *pItem;
    WORD                     *pVolTableVal;
    SetAudioVolTLBConfigResp *pSetAudioVolTLBConfigResp;
    CHAR                     desc[100];
} SLQSSetAudioVolTLBConfigTestCase_t;

#endif /* __GOBI_API_AUDIO_TEST_H__ */

