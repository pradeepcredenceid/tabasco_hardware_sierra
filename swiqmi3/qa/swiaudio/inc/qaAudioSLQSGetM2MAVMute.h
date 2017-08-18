/*
 * \ingroup swiaudio
 *
 * \file qaAudioSLQSGetM2MAVMute.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSGetM2MAVMute.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_GET_M2M_AVMUTE_H__
#define __AUDIO_SLQS_GET_M2M_AVMUTE_H__

#include "qaGobiApiSwiAudio.h"

/* enum declarations */

/*
 * An enumeration of eQMI_AUDIO_SLQS_GET_M2M_AUDIO_VOLUME request TLV IDs
 */
enum eQMI_AUDIO_SLQS_GET_M2M_AVMUTE_REQ
{
    eTLV_GET_M2M_AVMUTE_PROFILE = 0x1,
};

/*
 * An enumeration of eQMI_AUDIO_SLQS_GET_M2M_AUDIO_VOLUME response TLV IDs
 */
enum eQMI_AUDIO_SLQS_GET_M2M_AVMUTE_RESP
{
    eTLV_GET_M2M_AVMUTE_EARMUTE = 0x1,
    eTLV_GET_M2M_AVMUTE_MICMUTE = 0x2,
    eTLV_GET_M2M_AVMUTE_CWTMUTE = 0x3,
    eTLV_GET_M2M_AVMUTE_EC      = 0x4,
};

/*
 * This structure contains the SLQSGetM2MAVMute request parameters.
 *
 */
struct QmiAudioSLQSGetM2MAVMuteReq
{
    GetM2MAVMuteReq *pGetM2MAVMuteReq;
};

/*
 * This structure contains the SLQSSetM2MAVMute response parameters.
 *
 */
struct QmiAudioSLQSGetM2MAVMuteResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSSetM2MAVMute response parameters */
    GetM2MAVMuteResp *pGetM2MAVMuteResp;
};

/*
 * Prototypes
 */
enum eQCWWANError PkQmiAudioSLQSGetM2MAVMute(
    WORD            *pMlength,
    BYTE            *pParamField,
    GetM2MAVMuteReq *pGetM2MAVMuteReq );

enum eQCWWANError UpkQmiAudioSLQSGetM2MAVMute(
    BYTE                                *pMdmResp,
    struct QmiAudioSLQSGetM2MAVMuteResp *pApiResp );

#endif /* __AUDIO_SLQS_GET_M2M_AVMUTE_H__ */
