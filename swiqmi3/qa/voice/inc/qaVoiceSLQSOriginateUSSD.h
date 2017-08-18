/*
 * \ingroup voice
 *
 * \file    qaVoiceSLQSOriginateUSSD.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaVoiceSLQSOriginateUSSD.c
 *
 * Copyright: � 2011 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __VOICE_SLQS_ORIGINATE_USSD_H__
#define __VOICE_SLQS_ORIGINATE_USSD_H__

#include "qaGobiApiVoice.h"

/* enum declarations */

enum eQMI_VOICE_SLQS_ORIG_USSD_RESP
{
	eTLV_FAIL_CAUSE       = 0x10,
	eTLV_ALPHA_IDNTFR     = 0x11,
	eTLV_USSD_DATA        = 0x12,
	eTLV_CC_RESULT        = 0x13,
	eTLV_CALLER_ID          = 0x14,
	eTLV_CC_SUPP_SERVICE  = 0x15,
};

/*
 * This structure contains the VoiceOriginateUSSD response parameters.
 */
struct QmiVoiceSLQSOriginateUSSDResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* Optional TLVs */
    struct USSResp *pUSSResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiVoiceSLQSOriginateUSSD(
    WORD           *pMlength,
    BYTE           *pBuffer,
    struct USSInfo *pUSSInfo );

extern enum eQCWWANError UpkQmiVoiceSLQSOriginateUSSD(
    BYTE                             *pMdmResp,
    struct QmiVoiceSLQSOriginateUSSDResp *pApiResp );

#endif /* __VOICE_SLQS_ORIGINATE_USSD_H__ */
