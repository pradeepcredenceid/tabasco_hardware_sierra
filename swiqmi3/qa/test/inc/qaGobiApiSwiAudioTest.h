/**************
 *
 *  Filename: qaGobiApiSwiAudioTest.h
 *
 *  Purpose:  Contains prototypes for functions implementing specific
 *            tests for SWI Audio Service
 *            called by the main() routine in qatest.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc., all rights reserved
 *
 **************/

#ifndef __GOBI_API_SWI_AUDIO_TEST_H__
#define __GOBI_API_SWI_AUDIO_TEST_H__

typedef struct _SLQSGetM2MAudioProfileTestCase_t
{
    enum  eQCWWANError expectedReturnValue;
    ULONG              testCaseId;
    BYTE               *pGenerator;
    CHAR               desc[100];
} SLQSGetM2MAudioProfileTestCase_t;

typedef struct _SLQSSetM2MAudioProfileTestCase_t
{
    enum  eQCWWANError expectedReturnValue;
    ULONG              testCaseId;
    BYTE               Profile;
    BYTE               *pEarMute;
    BYTE               *pMicMute;
    BYTE               *pGenerator;
    BYTE               *pVolume;
    BYTE               *pCwtMute;
    CHAR               desc[100];
} SLQSSetM2MAudioProfileTestCase_t;

typedef struct _SLQSGetM2MAudioVolumeTestCase_t
{
    enum  eQCWWANError expectedReturnValue;
    ULONG              testCaseId;
    BYTE               Profile;
    BYTE               Generator;
    CHAR               desc[100];
} SLQSGetM2MAudioVolumeTestCase_t;

typedef struct _SLQSSetM2MAudioVolumeTestCase_t
{
    enum  eQCWWANError expectedReturnValue;
    ULONG              testCaseId;
    BYTE               Profile;
    BYTE               Generator;
    BYTE               Level;
    CHAR               desc[100];
} SLQSSetM2MAudioVolumeTestCase_t;

typedef struct _SLQSSetM2MAudioAVCFGTestCase_t
{
    enum      eQCWWANError expectedReturnValue;
    ULONG                  testCaseId;
    BYTE                   Profile;
    BYTE                   Device;
    BYTE                   PFACEId;
    PCMparams              *pPCMParams;
    CHAR                   desc[100];
} SLQSSetM2MAudioAVCFGTestCase_t;

typedef struct _SLQSSetM2MAudioLPBKTestCase_t
{
    enum  eQCWWANError expectedReturnValue;
    ULONG              testCaseId;
    BYTE               Enable;
    CHAR               desc[100];
} SLQSSetM2MAudioLPBKTestCase_t;

typedef struct _SLQSSetM2MAudioNVDefTestCase_t
{
    enum eQCWWANError expectedReturnValue;
    ULONG             testCaseId;
    CHAR              desc[100];
} SLQSSetM2MAudioNVDefTestCase_t;

typedef struct _SLQSGetM2MSpkrGainTestCase_t
{
    enum  eQCWWANError expectedReturnValue;
    ULONG              testCaseId;
    BYTE               Profile;
    CHAR               desc[100];
} SLQSGetM2MSpkrGainTestCase_t;

typedef struct _SLQSSetM2MSpkrGainTestCase_t
{
    enum  eQCWWANError expectedReturnValue;
    ULONG              testCaseId;
    BYTE               Profile;
    WORD               Value;
    CHAR               desc[100];
} SLQSSetM2MSpkrGainTestCase_t;

typedef struct _SLQSGetM2MAVMuteTestCase_t
{
    enum  eQCWWANError expectedReturnValue;
    ULONG              testCaseId;
    BYTE               Profile;
    CHAR               desc[100];
} SLQSGetM2MAVMuteTestCase_t;

typedef struct _SLQSSetM2MAVMuteTestCase_t
{
    enum  eQCWWANError expectedReturnValue;
    ULONG              testCaseId;
    BYTE               Profile;
    BYTE               EarMute;
    BYTE               MicMute;
    BYTE               *pCwtMute;
    CHAR               desc[100];
} SLQSSetM2MAVMuteTestCase_t;

#endif /* __GOBI_API_SWI_AUDIO_TEST_H__ */

