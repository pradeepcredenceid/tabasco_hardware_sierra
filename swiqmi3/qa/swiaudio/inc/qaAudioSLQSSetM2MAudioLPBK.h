/*
 * \ingroup swiaudio
 *
 * \file qaAudioSLQSSetM2MAudioLPBK.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSSetM2MAudioLPBK.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_SET_M2M_AUDIO_LPBK_H__
#define __AUDIO_SLQS_SET_M2M_AUDIO_LPBK_H__

#include "qaGobiApiSwiAudio.h"

/* enum declarations */

/*
 * An enumeration of eQMI_AUDIO_SLQS_SET_M2M_AUDIO_LPBK request TLV IDs
 */
enum eQMI_AUDIO_SLQS_SET_M2M_AUDIO_LPBK_REQ
{
    eTLV_SET_M2M_AUDIO_ENABLE = 0x1,
};

/*
 * An enumeration of eQMI_AUDIO_SLQS_SET_M2M_AUDIO_LPBK response TLV IDs
 */
enum eQMI_AUDIO_SLQS_SET_M2M_AUDIO_LPBK_RESP
{
    eTLV_SET_M2M_AUDIO_LPBK_EC = 0x1,
};

/*
 * This structure contains the SLQSSetM2MAudioLPBK request parameters.
 *
 */
struct QmiAudioSLQSSetM2MAudioLPBKReq
{
    SetM2MAudioLPBKReq *pSetM2MAudioLPBKReq;
};

/*
 * This structure contains the SLQSSetM2MAudioLPBK response parameters.
 *
 */
struct QmiAudioSLQSSetM2MAudioLPBKResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiAudioSLQSSetM2MAudioLPBK (
    WORD               *pMlength,
    BYTE               *pParamField,
    SetM2MAudioLPBKReq *setM2MAudioLPBKReq );

extern enum eQCWWANError UpkQmiAudioSLQSSetM2MAudioLPBK (
    BYTE                                   *pMdmResp,
    struct QmiAudioSLQSSetM2MAudioLPBKResp *pSetAudioLPBKResp );

#endif /* __AUDIO_SLQS_SET_M2M_AUDIO_LPBK_H__ */
