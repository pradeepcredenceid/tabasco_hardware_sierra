/*
 * \ingroup audio
 *
 * \file qaAudioSLQSSetAudioPathConfig.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSSetAudioPathConfig.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_SET_AUDIO_PATH_CONFIG_H__
#define __AUDIO_SLQS_SET_AUDIO_PATH_CONFIG_H__

/* enum declarations */
/*
 *
 * An enumeration of eQMI_AUDIO_SLQS_SET_AUDIO_PATH_CONFIG request TLV IDs
 *
 * Note: Most of the request TLV are common to the
 *       eQMI_AUDIO_SLQS_GET_AUDIO_PATH_CONFIG response TLV IDs, hence they
 *       are not re-declared again here. Will be used directly from
 *       eQMI_AUDIO_SLQS_GET_SET_AUDIO_PATH_COMMON
 */
enum eQMI_AUDIO_SLQS_SET_AUDIO_PATH_CONFIG_REQ
{
    eTLV_AUDIO_PATH_PROF = 0x10,
};

/*
 * This structure contains the SLQSSetAudioPathConfig request parameters.
 *
 * \sa qaGobiApiAudio.h for parameter descriptions
 *
 */
struct QmiAudioSLQSSetAudioPathConfigReq
{
    SetAudioPathConfigReq *pSetAudioPathConfigReq;
};

/*
 * This structure contains the SLQSSetAudioPathConfig response parameters.
 *
 */
struct QmiAudioSLQSSetAudioPathConfigResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiAudioSLQSSetAudioPathConfig (
    WORD                  *pMlength,
    BYTE                  *pParamField,
    SetAudioPathConfigReq *pSetAudioPathConfigReq );

extern enum eQCWWANError UpkQmiAudioSLQSSetAudioPathConfig (
    BYTE                                      *pMdmResp,
    struct QmiAudioSLQSSetAudioPathConfigResp *pSetAudioPathConfigResp );

#endif /* __AUDIO_SLQS_SET_AUDIO_PATH_CONFIG_H__ */
