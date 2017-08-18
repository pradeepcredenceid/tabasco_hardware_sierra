/*
 * \ingroup swiaudio
 *
 * \file qaAudioSLQSSetM2MAudioProfile.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSSetM2MAudioProfile.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_SET_M2M_AUDIO_PROFILE_H__
#define __AUDIO_SLQS_SET_M2M_AUDIO_PROFILE_H__

#include "qaGobiApiSwiAudio.h"

/* enum declarations */

/*
 * An enumeration of eQMI_AUDIO_SLQS_SET_M2M_AUDIO_PROFILE request TLV IDs
 */
enum eQMI_AUDIO_SLQS_SET_M2M_AUDIO_PROFILE_REQ
{
    eTLV_SET_M2M_AUDIO_PROFILE         = 0x1,
    eTLV_SET_M2M_AUDIO_PROFILE_EARMUTE = 0x10,
    eTLV_SET_M2M_AUDIO_PROFILE_MICMUTE = 0x11,
    eTLV_SET_M2M_AUDIO_PROFILE_GENRT   = 0x12,
    eTLV_SET_M2M_AUDIO_PROFILE_VOL     = 0x13,
    eTLV_SET_M2M_AUDIO_PROFILE_CWTMUT  = 0x14,
};

/*
 * An enumeration of eQMI_AUDIO_SLQS_SET_M2M_AUDIO_PROFILE response TLV IDs
 */
enum eQMI_AUDIO_SLQS_SET_M2M_AUDIO_PROFILE_RESP
{
    eTLV_SET_M2M_AUDIO_PROFILE_EC = 0x1,
};
/*
 * This structure contains the SLQSSetM2MAudioProfile request parameters.
 *
 */
struct QmiAudioSLQSSetM2MAudioProfileReq
{
    SetM2MAudioProfileReq *setM2MAudioProfileReq;
};

/*
 * This structure contains the SLQSSetM2MAudioProfile response parameters.
 *
 */
struct QmiAudioSLQSSetM2MAudioProfileResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiAudioSLQSSetM2MAudioProfile (
    WORD                  *pMlength,
    BYTE                  *pParamField,
    SetM2MAudioProfileReq *setM2MAudioProfileReq );

extern enum eQCWWANError UpkQmiAudioSLQSSetM2MAudioProfile (
    BYTE                                      *pMdmResp,
    struct QmiAudioSLQSSetM2MAudioProfileResp *pSetAudioProfileResp );

#endif /* __AUDIO_SLQS_SET_M2M_AUDIO_PROFILE_H__ */
