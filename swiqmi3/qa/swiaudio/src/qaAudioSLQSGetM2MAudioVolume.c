/*
 * \ingroup swiaudio
 *
 * \file   qaAudioSLQSGetM2MAudioVolume.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_M2M_AUDIO_GET_VOLUME_MSG message.
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
#include "qaGobiApiSwiAudio.h"
#include "qaAudioSLQSGetM2MAudioVolume.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSGetM2MAudioVolume Profile field to the
 * QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                  data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE  on success, eQCWWAN_ERR_XXX on error
 *
 */
enum eQCWWANError BuildTlvGetProfile( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSGetM2MAudioVolumeReq *pReq =
                     (struct QmiAudioSLQSGetM2MAudioVolumeReq *)pParam;

    GetM2MAudioVolumeReq *lReq =
                     (GetM2MAudioVolumeReq *)pReq->pGetM2MAudioVolumeReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->Profile );
}

/*
 * This function packs the SLQSGetM2MAudioVolume Generator field to the
 * QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                  data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE  on success, eQCWWAN_ERR_XXX on error
 *
 */
enum eQCWWANError BuildTlvGetGenerator( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSGetM2MAudioVolumeReq *pReq =
                     (struct QmiAudioSLQSGetM2MAudioVolumeReq *)pParam;

    GetM2MAudioVolumeReq *lReq =
                     (GetM2MAudioVolumeReq *)pReq->pGetM2MAudioVolumeReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->Generator );
}

/*
 * This function packs the SLQSGetM2MAudioVolume parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSGetM2MAudioVolume(
    WORD                 *pMlength,
    BYTE                 *pParamField,
    GetM2MAudioVolumeReq *pGetM2MAudioVolumeReq)
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_GET_M2M_AUDIO_VOLUME_PROFILE,   &BuildTlvGetProfile },
        { eTLV_GET_M2M_AUDIO_VOLUME_GENERATOR, &BuildTlvGetGenerator},
        { eTLV_TYPE_INVALID,                   NULL } /* Important. Sentinel.
                                                       * Signifies last item in map
                                                       */
    };

    enum   eQCWWANError                      eRCode;
    struct QmiAudioSLQSGetM2MAudioVolumeReq req;

    req.pGetM2MAudioVolumeReq = pGetM2MAudioVolumeReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_SWIAUDIO_GET_VOLUME,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSGetM2MAudioVolume RX Volume Level from the
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
enum eQCWWANError UnpackTLVM2MAudioVolumeLevel(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetM2MAudioVolumeResp *lResp =
        (struct QmiAudioSLQSGetM2MAudioVolumeResp *)pResp;
    GetM2MAudioVolumeResp *tResp = lResp->pGetM2MAudioVolumeResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    /* Get Volume Level in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    tResp->Level = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSSetM2MAudioProfile response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSGetM2MAudioVolume(
    BYTE                                     *pMdmResp,
    struct QmiAudioSLQSGetM2MAudioVolumeResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,                   &qmUnpackTlvResultCode },
        { eTLV_GET_M2M_AUDIO_RX_VOLUME_LEVEL, &UnpackTLVM2MAudioVolumeLevel},
        { eTLV_TYPE_INVALID,                  NULL } /* Important. Sentinel.
                                                      * Signifies last item in map.
                                                      */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_SWIAUDIO_GET_VOLUME );
    return eRCode;
}
