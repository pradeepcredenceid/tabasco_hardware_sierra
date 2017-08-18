/*
 * \ingroup audio
 *
 * \file qaAudioSLQSGetAudioProfile.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSGetAudioProfile.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_GET_AUDIO_PROFILE_H__
#define __AUDIO_SLQS_GET_AUDIO_PROFILE_H__

/* enum declarations */
/*
 *
 * An enumeration of eQMI_AUDIO_SLQS_GET_AUDIO_PROFILE request TLV IDs
 *
 */
enum eQMI_AUDIO_SLQS_GET_AUDIO_PROFILE_REQ
{
    eTLV_AUDIO_GENERATOR = 0x13
};

/*
 * An enumeration of eQMI_AUDIO_SLQS_GET_AUDIO_PROFILE response TLV IDs
 */
enum eQMI_AUDIO_SLQS_GET_AUDIO_PROFILE_RESP
{
    eTLV_AUDIO_PROFILE    = 0x10,
    eTLV_EAR_MUTE_SETTING = 0x11,
    eTLV_MIC_MUTE_SETTING = 0x12,
    eTLV_AUDIO_VOL_LEVEL  = 0x14,
};

/*
 * This structure contains the SLQSGetAudioProfile request parameters.
 *
 * \sa qaGobiApiAudio.h for parameter descriptions
 *
 */
struct QmiAudioSLQSGetProfileReq
{
    GetAudioProfileReq *pGetAudioProfileReq;
};

/*
 * This structure contains the SLQSGetAudioProfile response parameters.
 *
 */
struct QmiAudioSLQSGetProfileResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSGetAudioProfile response parameters */
    GetAudioProfileResp *pGetAudioProfileResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiAudioSLQSGetAudioProfile (
    WORD               *pMlength,
    BYTE               *pParamField,
    GetAudioProfileReq *pGetAudioProfileReq );

extern enum eQCWWANError UpkQmiAudioSLQSGetAudioProfile (
    BYTE                              *pMdmResp,
    struct QmiAudioSLQSGetProfileResp *pGetAudioProfileResp );

#endif /* __AUDIO_SLQS_GET_AUDIO_PROFILE_H__ */
