/*
 * \ingroup swiaudio
 *
 * \file qaAudioSLQSGetM2MAudioProfile.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaAudioSLQSGetM2MAudioProfile.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __AUDIO_SLQS_GET_M2M_AUDIO_PROFILE_H__
#define __AUDIO_SLQS_GET_M2M_AUDIO_PROFILE_H__

/* enum declarations */

/* An enumeration of eQMI_AUDIO_SLQS_GET_M2M_AUDIO_PROFILE request TLV IDs
 */
enum eVOICE_SLQS_GET_M2M_AUDIO_PROFILE_REQ
{
    eTLV_GET_M2M_AUDIO_GEN = 0x10,
};

/*
 * An enumeration of eQMI_AUDIO_SLQS_GET_M2M_AUDIO_PROFILE response TLV IDs
 */
enum eQMI_AUDIO_SLQS_GET_M2M_AUDIO_PROFILE_RESP
{
    eTLV_GET_M2M_AUDIO_PROFILE   = 0x1,
    eTLV_GET_M2M_AUDIO_EARMUTE   = 0x2,
    eTLV_GET_M2M_AUDIO_MICMUTE   = 0x3,
    eTLV_GET_M2M_AUDIO_GENERATOR = 0x4,
    eTLV_GET_M2M_AUDIO_VOLUME    = 0x5,
    eTLV_GET_M2M_AUDIO_CWTMUTE   = 0x6,
    eTLV_GET_M2M_AUDIO_RESCODE   = 0x7,
};

/*
 * This structure contains the SLQSGetM2MAudioProfile response parameters.
 *
 */
struct QmiAudioSLQSGetM2MAudioProfileResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSGetM2MAudioProfile response parameters */
    GetM2MAudioProfileResp *pGetM2MAudioProfileResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiAudioSLQSGetM2MAudioProfile (
    WORD                  *pMlength,
    BYTE                  *pParamField,
    GetM2MAudioProfileReq *pGetM2MAudioProfileReq );

extern enum eQCWWANError UpkQmiAudioSLQSGetM2MAudioProfile (
    BYTE                                      *pMdmResp,
    struct QmiAudioSLQSGetM2MAudioProfileResp *pGetAudioProfileResp );

#endif /* __AUDIO_SLQS_GET_M2M_AUDIO_PROFILE_H__ */
