/*
 * \ingroup swiaudio
 *
 * \file qaAudioSLQSSetM2MAVMute.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSSetM2MAVMute.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_SET_M2M_AVMUTE_H__
#define __AUDIO_SLQS_SET_M2M_AVMUTE_H__

#include "qaGobiApiSwiAudio.h"

/* enum declarations */

/*
 * An enumeration of eQMI_AUDIO_SLQS_SET_M2M_AVMUTE request TLV IDs
 */
enum eQMI_AUDIO_SLQS_SET_M2M_AVMUTE_REQ
{
    eTLV_SET_M2M_AVMUTE_PROFILE = 0x1,
    eTLV_SET_M2M_AVMUTE_EARMUTE = 0x2,
    eTLV_SET_M2M_AVMUTE_MICMUTE = 0x3,
    eTLV_SET_M2M_AVMUTE_CWTMUTE = 0x10,
};

/*
 * An enumeration of eQMI_AUDIO_SLQS_SET_M2M_AVMUTE response TLV IDs
 */
enum eQMI_AUDIO_SLQS_SET_M2M_AVMUTE_RESP
{
    eTLV_SET_M2M_AVMUTE_EC = 0x1,
};

/*
 * This structure contains the SLQSSetM2MAVMute request parameters.
 *
 */
struct QmiAudioSLQSSetM2MAVMuteReq
{
    SetM2MAVMuteReq *setM2MAVMuteReq;
};

/*
 * This structure contains the SLQSSetM2MAVMute response parameters.
 *
 */
struct QmiAudioSLQSSetM2MAVMuteResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiAudioSLQSSetM2MAVMute (
    WORD            *pMlength,
    BYTE            *pParamField,
    SetM2MAVMuteReq *pSetM2MAVMuteReq );

extern enum eQCWWANError UpkQmiAudioSLQSSetM2MAVMute (
    BYTE                                *pMdmResp,
    struct QmiAudioSLQSSetM2MAVMuteResp *pSetAVMuteResp );

#endif /* __AUDIO_SLQS_SET_M2M_AVMUTE_H__ */
