/*
 * \ingroup swiaudio
 *
 * \file qaAudioSLQSGetM2MSpkrGain.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSGetM2MSpkrGain.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_GET_M2M_SPKRGAIN_H__
#define __AUDIO_SLQS_GET_M2M_SPKRGAIN_H__

#include "qaGobiApiSwiAudio.h"

/* enum declarations */

/*
 * An enumeration of eQMI_AUDIO_SLQS_GET_M2M_SPKRGAIN response TLV IDs
 */
enum eQMI_AUDIO_SLQS_GET_M2M_SPKRGAIN_RESP
{
    eTLV_GET_M2M_SPKRGAIN_VALUE = 0x1,
};

/*
 * An enumeration of eQMI_AUDIO_SLQS_GET_M2M_SPKRGAIN request TLV IDs
 */
enum eQMI_AUDIO_SLQS_GET_M2M_SPKRGAIN_REQ
{
    eTLV_GET_M2M_SPKRGAIN_PROFILE = 0x1,
};

/*
 * This structure contains the SLQSGetM2MSpkrGain request parameters.
 *
 */
struct QmiAudioSLQSGetM2MSpkrGainReq
{
    GetM2MSpkrGainReq *pSpkrGainReq;
};

/*
 * This structure contains the SLQSSetM2MSpkrGain response parameters.
 *
 */
struct QmiAudioSLQSGetM2MSpkrGainResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSSetM2MSpkrGain response parameters */
    GetM2MSpkrGainResp *pSpkrGainResp;
};

/*
 * Prototypes
 */
enum eQCWWANError PkQmiAudioSLQSGetM2MSpkrGain(
    WORD              *pMlength,
    BYTE              *pParamField,
    GetM2MSpkrGainReq *pGetM2MSpkrGainReq );

enum eQCWWANError UpkQmiAudioSLQSGetM2MSpkrGain(
    BYTE                                  *pMdmResp,
    struct QmiAudioSLQSGetM2MSpkrGainResp *pApiResp );

#endif /* __AUDIO_SLQS_GET_M2M_SPKRGAIN_H__ */
