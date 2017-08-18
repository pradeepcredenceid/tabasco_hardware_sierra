/*
 * \ingroup audio
 *
 * \file qaAudioSLQSSetAudioVolTLBConfig.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSSetAudioVolTLBConfig.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_SET_AUDIO_VOL_TLB_CONFIG_H__
#define __AUDIO_SLQS_SET_AUDIO_VOL_TLB_CONFIG_H__

/* enum declarations */
/*
 *
 * An enumeration of eQMI_AUDIO_SLQS_SET_AUDIO_VOL_TLB_CONFIG request TLV IDs
 *
 * Note: Most of the request TLV are common to the
 *       eQMI_AUDIO_SLQS_GET_AUDIO_VOL_TLB_CONFIG request TLV IDs, hence they
 *       are not re-declared again here. Will be used directly from
 *       eQMI_AUDIO_SLQS_GET_SET_AUDIO_VOL_TLB_CONFIG
 *
 */
enum eQMI_AUDIO_SLQS_SET_AUDIO_VOL_TLB_CONFIG
{
    eTLV_AUDIO_TLB_VOL_TABLE = 0x21,
};

/*
 * An enumeration of eQMI_AUDIO_SLQS_SET_AUDIO_VOL_TLB_CONFIG response TLV IDs
 */
enum eQMI_AUDIO_SLQS_SET_AUDIO_VOL_TLB_CONFIG_RESP
{
    eTLV_AUDIO_TLB_CFG_VALUE = 0x21
};

/*
 * This structure contains the SLQSSetAudioVolTLBConfig request parameters.
 *
 * \sa qaGobiApiAudio.h for parameter descriptions
 *
 */
struct QmiAudioSLQSSetVolTLBConfigReq
{
    SetAudioVolTLBConfigReq *pSetAudioVolTLBConfigReq;
};

/*
 * This structure contains the SLQSSetAudioVolTLBConfig response parameters.
 *
 */
struct QmiAudioSLQSSetVolTLBConfigResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSSetAudioVolTLBConfig response parameters */
    SetAudioVolTLBConfigResp *pSetAudioVolTLBConfigResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiAudioSLQSSetAudioVolTLBConfig (
    WORD                    *pMlength,
    BYTE                    *pParamField,
    SetAudioVolTLBConfigReq *pSetAudioVolTLBConfigReq );

extern enum eQCWWANError UpkQmiAudioSLQSSetAudioVolTLBConfig (
    BYTE                                   *pMdmResp,
    struct QmiAudioSLQSSetVolTLBConfigResp *pSetAudioVolTLBConfigResp );

#endif /* __AUDIO_SLQS_SET_AUDIO_VOL_TLB_CONFIG_H__ */
