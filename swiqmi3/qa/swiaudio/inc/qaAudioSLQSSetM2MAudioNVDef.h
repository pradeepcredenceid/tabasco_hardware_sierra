/*
 * \ingroup swiaudio
 *
 * \file qaAudioSLQSSetM2MAudioNVDef.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSSetM2MAudioNVDef.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_SET_M2M_AUDIO_NV_DEF_H__
#define __AUDIO_SLQS_SET_M2M_AUDIO_NV_DEF_H__

#include "qaGobiApiSwiAudio.h"

/*
 * An enumeration of eQMI_AUDIO_SLQS_SET_M2M_AUDIO_NV_DEF response TLV IDs
 */
enum eQMI_AUDIO_SLQS_SET_M2M_AUDIO_NV_DEF_RESP
{
    eTLV_SET_M2M_AUDIO_NVDEF_EC = 0x1,
};

/*
 * This structure contains the SLQSSetM2MAudioNVDef response parameters.
 *
 */
struct QmiAudioSLQSSetM2MAudioNVDefResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiAudioSLQSSetM2MAudioNVDef (
    WORD *pMlength,
    BYTE *pParamField );

extern enum eQCWWANError UpkQmiAudioSLQSSetM2MAudioNVDef (
    BYTE                                    *pMdmResp,
    struct QmiAudioSLQSSetM2MAudioNVDefResp *pSetAudioNVDefResp );

#endif /* __AUDIO_SLQS_SET_M2M_AUDIO_NV_DEF_H__ */
