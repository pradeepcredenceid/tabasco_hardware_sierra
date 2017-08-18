/*
 * \ingroup swiaudio
 *
 * \file qaAudioSLQSSetM2MSpkrGain.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSGetM2MSpkrGain.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_SET_M2M_SPKRGAIN_H__
#define __AUDIO_SLQS_SET_M2M_SPKRGAIN_H__

#include "qaGobiApiSwiAudio.h"

/* enum declarations */

/*
 * An enumeration of eQMI_AUDIO_SLQS_SET_M2M_SPKRGAIN request TLV IDs
 */
enum eQMI_AUDIO_SLQS_SET_M2M_SPKRGAIN_REQ
{
    eTLV_SET_M2M_SPKRGAIN_PROFILE = 0x1,
    eTLV_SET_M2M_SPKRGAIN_VALUE   = 0x2,
};

/*
 * An enumeration of eQMI_AUDIO_SLQS_SET_M2M_SPKRGAIN response TLV IDs
 */
enum eQMI_AUDIO_SLQS_SET_M2M_SPKRGAIN_RESP
{
    eTLV_SET_M2M_SPKRGAIN_EC = 0x1,
};

/*
 * This structure contains the SLQSSetM2MSpkrGain request parameters.
 *
 */
struct QmiAudioSLQSSetM2MSpkrGainReq
{
    SetM2MSpkrGainReq *pSpkrGainReq;
};

/*
 * This structure contains the SLQSSetM2MSpkrGain response parameters.
 *
 */
struct QmiAudioSLQSSetM2MSpkrGainResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
enum eQCWWANError PkQmiAudioSLQSSetM2MSpkrGain(
    WORD               *pMlength,
    BYTE               *pParamField,
    SetM2MSpkrGainReq  *pSetM2MSpkrGainReq);

enum eQCWWANError UpkQmiAudioSLQSSetM2MSpkrGain(
    BYTE                                  *pMdmResp,
    struct QmiAudioSLQSSetM2MSpkrGainResp *pApiResp);

#endif /* __AUDIO_SLQS_SET_M2M_SPKRGAIN_H__ */
