/*
 * \ingroup audio
 *
 * \file   qaAudioSLQSSetAudioProfile.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_AUDIO_SET_PROFILE_MSG message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"
#include "qaGobiApiAudio.h"
#include "qaAudioSLQSSetAudioProfile.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetAudioProfile Profile Number field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvProfileNum(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetProfileReq *pReq =
                        (struct QmiAudioSLQSSetProfileReq *)pParam;
    SetAudioProfileReq *tReq = pReq->pSetAudioProfileReq;

    /* Insert the profile number into the QMI message */
    return PutByte( pBuf, tReq->Profile );
}

/*
 * This function packs the SLQSSetAudioProfile Ear Mute field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvEarMute(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetProfileReq *pReq =
                        (struct QmiAudioSLQSSetProfileReq *)pParam;
    SetAudioProfileReq *tReq = pReq->pSetAudioProfileReq;

    /* Insert the ear mute into the QMI message */
    return PutByte( pBuf, tReq->EarMute );
}

/*
 * This function packs the SLQSSetAudioProfile Mic Mute field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvMicMute(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetProfileReq *pReq =
                        (struct QmiAudioSLQSSetProfileReq *)pParam;
    SetAudioProfileReq *tReq = pReq->pSetAudioProfileReq;

    /* Insert the MIC mute into the QMI message */
    return PutByte( pBuf, tReq->MicMute );
}

/*
 * This function packs the SLQSSetAudioProfile Audio generator field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAudGenerator(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetProfileReq *pReq =
                        (struct QmiAudioSLQSSetProfileReq *)pParam;
    SetAudioProfileReq *tReq = pReq->pSetAudioProfileReq;

    /* Insert the Generator into the QMI message */
    return PutByte( pBuf, tReq->Generator );
}

/*
 * This function packs the SLQSSetAudioProfile Audio volume field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAudVolume(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetProfileReq *pReq =
                        (struct QmiAudioSLQSSetProfileReq *)pParam;
    SetAudioProfileReq *tReq = pReq->pSetAudioProfileReq;

    /* Insert the Audio volume into the QMI message */
    return PutByte( pBuf, tReq->Volume );
}

/*
 * This function packs the SLQSSetAudioProfile parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \param  pSetAudioProfileReq [IN] - Request parameters for packing.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSSetAudioProfile(
    WORD               *pMlength,
    BYTE               *pParamField,
    SetAudioProfileReq *pSetAudioProfileReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_AUDIO_PROFILE_NUM, &BuildTlvProfileNum },
        { eTLV_EAR_MUTE,          &BuildTlvEarMute },
        { eTLV_MIC_MUTE,          &BuildTlvMicMute },
        { eTLV_AUD_GENERATOR,     &BuildTlvAudGenerator },
        { eTLV_AUDIO_VOLUME,      &BuildTlvAudVolume },
        { eTLV_TYPE_INVALID,      NULL } /* Important. Sentinel.
                                          * Signifies last item in map
                                          */
    };

    struct QmiAudioSLQSSetProfileReq req;
    enum   eQCWWANError              eRCode;

    /* Intialize the request structure */
    slmemset( (char *)&req, 0, sizeof(struct QmiAudioSLQSSetProfileReq) );
    req.pSetAudioProfileReq   = pSetAudioProfileReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_AUDIO_SET_PROFILE,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSSetAudioProfile response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSSetAudioProfile(
    BYTE                              *pMdmResp,
    struct QmiAudioSLQSSetProfileResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,  &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map.
                                     */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_AUDIO_SET_PROFILE );
    return eRCode;
}
