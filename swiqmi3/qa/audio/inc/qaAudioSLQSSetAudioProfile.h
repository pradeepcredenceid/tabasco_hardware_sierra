/*
 * \ingroup audio
 *
 * \file qaAudioSLQSSetAudioProfile.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSSetAudioProfile.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_SET_AUDIO_PROFILE_H__
#define __AUDIO_SLQS_SET_AUDIO_PROFILE_H__

/* enum declarations */
/*
 *
 * An enumeration of eQMI_AUDIO_SLQS_SET_AUDIO_PROFILE request TLV IDs
 *
 */
enum eQMI_AUDIO_SLQS_SET_AUDIO_PROFILE_REQ
{
    eTLV_AUDIO_PROFILE_NUM = 0x10,
    eTLV_EAR_MUTE          = 0x11,
    eTLV_MIC_MUTE          = 0x12,
    eTLV_AUD_GENERATOR     = 0x13,
    eTLV_AUDIO_VOLUME      = 0x14,
};

/*
 * This structure contains the SLQSSetAudioProfile request parameters.
 *
 * \sa qaGobiApiAudio.h for parameter descriptions
 *
 */
struct QmiAudioSLQSSetProfileReq
{
    SetAudioProfileReq *pSetAudioProfileReq;
};

/*
 * This structure contains the SLQSSetAudioProfile response parameters.
 *
 */
struct QmiAudioSLQSSetProfileResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiAudioSLQSSetAudioProfile (
    WORD               *pMlength,
    BYTE               *pParamField,
    SetAudioProfileReq *pSetAudioProfileReq );

extern enum eQCWWANError UpkQmiAudioSLQSSetAudioProfile (
    BYTE                              *pMdmResp,
    struct QmiAudioSLQSSetProfileResp *pSetAudioProfileResp );

#endif /* __AUDIO_SLQS_SET_AUDIO_PROFILE_H__ */
