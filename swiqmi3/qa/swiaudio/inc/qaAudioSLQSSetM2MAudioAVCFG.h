/*
 * \ingroup swiaudio
 *
 * \file qaAudioSLQSSetM2MAudioAVCFG.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSSetM2MAudioAVCFG.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_SET_M2M_AUDIO_AVCFG_H__
#define __AUDIO_SLQS_SET_M2M_AUDIO_AVCFG_H__

#include "qaGobiApiSwiAudio.h"

/* enum declarations */

/*
 * An enumeration of eQMI_AUDIO_SLQS_SET_M2M_AUDIO_AVCFG request TLV IDs
 */
enum eQMI_AUDIO_SLQS_SET_M2M_AUDIO_AVCFG_REQ
{
    eTLV_SET_M2M_AUDIO_AVCFG_PROFILE    = 0x1,
    eTLV_SET_M2M_AUDIO_AVCFG_DEVICE     = 0x2,
    eTLV_SET_M2M_AUDIO_AVCFG_PIFACEID   = 0x3,
    eTLV_SET_M2M_AUDIO_AVCFG_PCM_PARAMS = 0x10,
};

/*
 * An enumeration of eQMI_AUDIO_SLQS_SET_M2M_AUDIO_AVCFG response TLV IDs
 */
enum eQMI_AUDIO_SLQS_SET_M2M_AUDIO_AVCFG_RESP
{
    eTLV_SET_M2M_AUDIO_AVCFG_EC = 0x1,
};
/*
 * This structure contains the SLQSSetM2MAudioAVCFG request parameters.
 *
 */
struct QmiAudioSLQSSetM2MAudioAVCFGReq
{
    SetM2MAudioAVCFGReq *pSetM2MAudioAVCFGReq;
};

/*
 * This structure contains the SLQSSetM2MAudioAVCFG response parameters.
 *
 */
struct QmiAudioSLQSSetM2MAudioAVCFGResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiAudioSLQSSetM2MAudioAVCFG (
    WORD                *pMlength,
    BYTE                *pParamField,
    SetM2MAudioAVCFGReq *setM2MAudioAVCFGReq );

extern enum eQCWWANError UpkQmiAudioSLQSSetM2MAudioAVCFG (
    BYTE                                    *pMdmResp,
    struct QmiAudioSLQSSetM2MAudioAVCFGResp *pSetAudioAVCFGResp );

#endif /* __AUDIO_SLQS_SET_M2M_AUDIO_AVCFG_H__ */
