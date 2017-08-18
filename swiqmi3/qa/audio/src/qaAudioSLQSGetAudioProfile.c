/*
 * \ingroup audio
 *
 * \file   qaAudioSLQSGetAudioProfile.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_AUDIO_GET_PROFILE_MSG message.
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
#include "qaAudioSLQSGetAudioProfile.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSGetAudioProfile audio generator field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAudioGenerator(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSGetProfileReq *pReq =
                        (struct QmiAudioSLQSGetProfileReq *)pParam;
    GetAudioProfileReq *tReq = pReq->pGetAudioProfileReq;

    /* Insert the generator into the QMI message */
    return PutByte( pBuf, tReq->Generator);
}

/*
 * This function packs the SLQSGetAudioProfile parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \param  pGetAudioProfileReq [IN] - Request parameters for packing.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSGetAudioProfile(
    WORD               *pMlength,
    BYTE               *pParamField,
    GetAudioProfileReq *pGetAudioProfileReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_AUDIO_GENERATOR, &BuildTlvAudioGenerator },
        { eTLV_TYPE_INVALID,    NULL } /* Important. Sentinel.
                                        * Signifies last item in map
                                        */
    };

    struct QmiAudioSLQSGetProfileReq req;
    enum   eQCWWANError              eRCode;

    /* Intialize the request structure */
    slmemset( (char *)&req, 0, sizeof(struct QmiAudioSLQSGetProfileReq) );
    req.pGetAudioProfileReq   = pGetAudioProfileReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_AUDIO_GET_PROFILE,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSGetAudioProfile Audio profile from the
 * QMI response message to a user provided response structure. This is
 * part of the mandatory TLV.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvAudioProfile(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetProfileResp *lResp =
        (struct QmiAudioSLQSGetProfileResp *)pResp;
    GetAudioProfileResp *tResp = lResp->pGetAudioProfileResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    /* Get Audio Profile in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    tResp->Profile = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioProfile Ear Mute Setting from the
 * QMI response message to a user provided response structure. This is
 * part of the mandatory TLV.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvEarMute(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetProfileResp *lResp =
        (struct QmiAudioSLQSGetProfileResp *)pResp;
    GetAudioProfileResp *tResp = lResp->pGetAudioProfileResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    /* Get Ear Mute in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    tResp->EarMute = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioProfile Mic Mute Setting from the
 * QMI response message to a user provided response structure. This is
 * part of the mandatory TLV.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvMicMute(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetProfileResp *lResp =
        (struct QmiAudioSLQSGetProfileResp *)pResp;
    GetAudioProfileResp *tResp = lResp->pGetAudioProfileResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    /* Get MIC Mute in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    tResp->MicMute = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioProfile Audio Volume level from the
 * QMI response message to a user provided response structure. This is
 * part of the mandatory TLV.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvAudioVol(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetProfileResp *lResp =
        (struct QmiAudioSLQSGetProfileResp *)pResp;
    GetAudioProfileResp *tResp = lResp->pGetAudioProfileResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    /* Get Audio volume level in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    tResp->Volume = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioProfile response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSGetAudioProfile(
    BYTE                              *pMdmResp,
    struct QmiAudioSLQSGetProfileResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,      &qmUnpackTlvResultCode },
        { eTLV_AUDIO_PROFILE,    &UnpackTlvAudioProfile },
        { eTLV_EAR_MUTE_SETTING, &UnpackTlvEarMute },
        { eTLV_MIC_MUTE_SETTING, &UnpackTlvMicMute },
        { eTLV_AUDIO_VOL_LEVEL,  &UnpackTlvAudioVol },
        { eTLV_TYPE_INVALID,     NULL } /* Important. Sentinel.
                                         * Signifies last item in map.
                                         */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_AUDIO_GET_PROFILE );
    return eRCode;
}
