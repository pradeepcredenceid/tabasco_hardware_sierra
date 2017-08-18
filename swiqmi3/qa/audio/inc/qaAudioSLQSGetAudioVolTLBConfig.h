/*
 * \ingroup audio
 *
 * \file qaAudioSLQSGetAudioVolTLBConfig.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSGetAudioVolTLBConfig.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_GET_AUDIO_VOL_TLB_CONFIG_H__
#define __AUDIO_SLQS_GET_AUDIO_VOL_TLB_CONFIG_H__

/* enum declarations */
/*
 *
 * An enumeration of eQMI_AUDIO_SLQS_GET_SET_AUDIO_VOL_TLB_CONFIG
 * request TLV IDs
 *
 */
enum eQMI_AUDIO_SLQS_GET_SET_AUDIO_VOL_TLB_CONFIG
{
    eTLV_AUDIO_TLB_PROFILE   = 0x10,
    eTLV_AUDIO_TLB_GENERATOR = 0x13,
    eTLV_AUDIO_TLB_VOLUME    = 0x14,
    eTLV_AUDIO_TLB_ITEM      = 0x20,
};

/*
 * An enumeration of eQMI_AUDIO_SLQS_GET_SET_AUDIO_VOL_TLB_CONFIG
 * response TLV IDs
 */
enum eQMI_AUDIO_SLQS_GET_AUDIO_VOL_TLB_CONFIG_RESP
{
	eTLV_AUDIO_VOL_TLB_VALUE = 0x21
};

/*
 * This structure contains the SLQSGetAudioVolTLBConfig request parameters.
 *
 * \sa qaGobiApiAudio.h for parameter descriptions
 *
 */
struct QmiAudioSLQSGetVolTLBConfigReq
{
    GetAudioVolTLBConfigReq *pGetAudioVolTLBConfigReq;
};

/*
 * This structure contains the SLQSGetAudioVolTLBConfig response parameters.
 *
 */
struct QmiAudioSLQSGetVolTLBConfigResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSGetAudioVolTLBConfig response parameters */
    GetAudioVolTLBConfigResp *pGetAudioVolTLBConfigResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiAudioSLQSGetAudioVolTLBConfig (
    WORD                    *pMlength,
    BYTE                    *pParamField,
    GetAudioVolTLBConfigReq *pGetAudioVolTLBConfigReq );

extern enum eQCWWANError UpkQmiAudioSLQSGetAudioVolTLBConfig (
    BYTE                                   *pMdmResp,
    struct QmiAudioSLQSGetVolTLBConfigResp *pGetAudioVolTLBConfigResp );

#endif /* __AUDIO_SLQS_GET_AUDIO_VOL_TLB_CONFIG_H__ */
