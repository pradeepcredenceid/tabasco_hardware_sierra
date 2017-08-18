/*
 * \ingroup swiaudio
 *
 * \file qaAudioSLQSGetM2MAudioVolume.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSGetM2MAudioVolume.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_GET_M2M_AUDIO_VOLUME_H__
#define __AUDIO_SLQS_GET_M2M_AUDIO_VOLUME_H__

#include "qaGobiApiSwiAudio.h"

/* enum declarations */

/*
 * An enumeration of eQMI_AUDIO_SLQS_GET_M2M_AUDIO_VOLUME response TLV IDs
 */
enum eQMI_AUDIO_SLQS_GET_M2M_AUDIO_VOLUME_RESP
{
    eTLV_GET_M2M_AUDIO_RX_VOLUME_LEVEL = 0x1,
};

/*
 * An enumeration of eQMI_AUDIO_SLQS_GET_M2M_AUDIO_VOLUME request TLV IDs
 */
enum eQMI_AUDIO_SLQS_GET_M2M_AUDIO_VOLUME_REQ
{
    eTLV_GET_M2M_AUDIO_VOLUME_PROFILE   = 0x1,
    eTLV_GET_M2M_AUDIO_VOLUME_GENERATOR = 0x2,
};

/*
 * This structure contains the SLQSGetM2MAudioVolume request parameters.
 *
 */
struct QmiAudioSLQSGetM2MAudioVolumeReq
{
    GetM2MAudioVolumeReq *pGetM2MAudioVolumeReq;
};

/*
 * This structure contains the SLQSSetM2MAudioVolume response parameters.
 *
 */
struct QmiAudioSLQSGetM2MAudioVolumeResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSSetM2MAudioVolume response parameters */
    GetM2MAudioVolumeResp *pGetM2MAudioVolumeResp;
};

/*
 * Prototypes
 */
enum eQCWWANError PkQmiAudioSLQSGetM2MAudioVolume(
    WORD                  *pMlength,
    BYTE                  *pParamField,
    GetM2MAudioVolumeReq  *pGetM2MAudioVolumeReq);

enum eQCWWANError UpkQmiAudioSLQSGetM2MAudioVolume(
    BYTE                                     *pMdmResp,
    struct QmiAudioSLQSGetM2MAudioVolumeResp *pApiResp);

#endif /* __AUDIO_SLQS_GET_M2M_AUDIO_VOLUME_H__ */
