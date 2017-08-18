/*
 * \ingroup swiaudio
 *
 * \file qaAudioSLQSSetM2MAudioVolume.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSSetM2MAudioProfile.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_SET_M2M_AUDIO_VOLUME_H__
#define __AUDIO_SLQS_SET_M2M_AUDIO_VOLUME_H__

#include "qaGobiApiSwiAudio.h"

/* enum declarations */

/*
 * An enumeration of eQMI_AUDIO_SLQS_SET_M2M_AUDIO_VOLUME response TLV IDs
 */
enum eQMI_AUDIO_SLQS_SET_M2M_AUDIO_VOLUME_REQ
{
    eTLV_SET_M2M_AUDIO_VOLUME_PROFILE   = 0x1,
    eTLV_SET_M2M_AUDIO_VOLUME_GENERATOR = 0x2,
    eTLV_SET_M2M_AUDIO_VOLUME_LEVEL     = 0x3,
};

/*
 * An enumeration of eQMI_AUDIO_SLQS_SET_M2M_AUDIO_VOLUME response TLV IDs
 */
enum eQMI_AUDIO_SLQS_SET_M2M_AUDIO_VOLUME_RESP
{
    eTLV_SET_M2M_AUDIO_VOLUME_EC = 0x1,
};

/*
 * This structure contains the SLQSSetM2MAudioVolume request parameters.
 *
 */
struct QmiAudioSLQSSetM2MAudioVolumeReq
{
    SetM2MAudioVolumeReq *setM2MAudioVolumeReq;
};

/*
 * This structure contains the SLQSSetM2MAudioVolume response parameters.
 *
 */
struct QmiAudioSLQSSetM2MAudioVolumeResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiAudioSLQSSetM2MAudioVolume (
    WORD                 *pMlength,
    BYTE                 *pParamField,
    SetM2MAudioVolumeReq *pSetM2MAudioVolumeReq );

extern enum eQCWWANError UpkQmiAudioSLQSSetM2MAudioVolume (
    BYTE                                     *pMdmResp,
    struct QmiAudioSLQSSetM2MAudioVolumeResp *pSetAudioVolumeResp );

#endif /* __AUDIO_SLQS_SET_M2M_AUDIO_VOLUME_H__ */
