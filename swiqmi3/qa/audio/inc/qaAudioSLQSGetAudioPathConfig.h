/*
 * \ingroup audio
 *
 * \file qaAudioSLQSGetAudioPathConfig.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSGetAudioPathConfig.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_GET_AUDIO_PATH_CONFIG_H__
#define __AUDIO_SLQS_GET_AUDIO_PATH_CONFIG_H__

#define STAGESIZECOUNT 20

/* enum declarations */
/*
 *
 * An enumeration of eQMI_AUDIO_SLQS_GET_AUDIO_PATH_CONFIG request TLV IDs
 *
 */
enum eQMI_AUDIO_SLQS_GET_AUDIO_PATH_CONFIG_REQ
{
    eTLV_AUDIO_PATH_PROFILE = 0x10,
    eTLV_AUDIO_ITEM         = 0x20
};

/*
 * An enumeration of eQMI_AUDIO_SLQS_GET_AUDIO_PATH_CONFIG response TLV IDs
 * and eQMI_AUDIO_SLQS_SET_AUDIO_PATH_CONFIG request TLV IDs
 */
enum eQMI_AUDIO_SLQS_GET_SET_AUDIO_PATH_CONFIG_COMMON
{
    eTLV_AV_EC             = 0x21,
    eTLV_AV_NS             = 0x22,
    eTLV_AV_TXVOL          = 0x23,
    eTLV_AV_DTMFTXG        = 0x24,
    eTLV_AV_CODECSTG       = 0x25,
    eTLV_AV_TXPCMIIRFLTR   = 0x26,
    eTLV_AV_RXPCMIIRFLTR   = 0x27,
    eTLV_AV_MICGAIN        = 0x28,
    eTLV_RX_AVC_AGC_SWITCH = 0x29,
    eTLV_TX_AVC_SWITCH     = 0x2A,
    eTLV_RX_AGC_LIST       = 0x2B,
    eTLV_RX_AVC_LIST       = 0x2C,
    eTLV_TX_AGC_LIST       = 0x2D,
};

/*
 * This structure contains the SLQSGetAudioPathConfig request parameters.
 *
 * \sa qaGobiApiAudio.h for parameter descriptions
 *
 */
struct QmiAudioSLQSGetAudioPathConfigReq
{
    GetAudioPathConfigReq *pGetAudioPathConfigReq;
};

/*
 * This structure contains the SLQSGetAudioPathConfig response parameters.
 *
 */
struct QmiAudioSLQSGetAudioPathConfigResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSGetAudioPathConfig response parameters */
    GetAudioPathConfigResp *pGetAudioPathConfigResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiAudioSLQSGetAudioPathConfig (
    WORD                  *pMlength,
    BYTE                  *pParamField,
    GetAudioPathConfigReq *pGetAudioPathConfigReq );

extern enum eQCWWANError UpkQmiAudioSLQSGetAudioPathConfig (
    BYTE                                      *pMdmResp,
    struct QmiAudioSLQSGetAudioPathConfigResp *pGetAudioPathConfigResp );

#endif /* __AUDIO_SLQS_GET_AUDIO_PATH_CONFIG_H__ */
