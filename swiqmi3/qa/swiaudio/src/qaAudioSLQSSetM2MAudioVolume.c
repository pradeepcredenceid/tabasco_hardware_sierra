/*
 * \ingroup swiaudio
 *
 * \file   qaAudioSLQSSetM2MAudioVolume.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_M2M_AUDIO_SET_VOLUME_MSG message.
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
#include "qaAudioSLQSSetM2MAudioVolume.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetM2MAudioVolume Profile field to the
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
enum eQCWWANError BuildTlvSetVolumeProfile( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAudioVolumeReq *pReq =
                     (struct QmiAudioSLQSSetM2MAudioVolumeReq *)pParam;

    SetM2MAudioVolumeReq *lReq =
                     (SetM2MAudioVolumeReq *)pReq->setM2MAudioVolumeReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->Profile );
}

/*
 * This function packs the SLQSSetM2MAudioVolume Generator field to the
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
enum eQCWWANError BuildTlvSetGenerator( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAudioVolumeReq *pReq =
                     (struct QmiAudioSLQSSetM2MAudioVolumeReq *)pParam;

    SetM2MAudioVolumeReq *lReq =
                     (SetM2MAudioVolumeReq *)pReq->setM2MAudioVolumeReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->Generator );
}

/*
 * This function packs the SLQSSetM2MAudioVolume Level field to the
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
enum eQCWWANError BuildTlvSetLevel( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAudioVolumeReq *pReq =
                     (struct QmiAudioSLQSSetM2MAudioVolumeReq *)pParam;

    SetM2MAudioVolumeReq *lReq =
                     (SetM2MAudioVolumeReq *)pReq->setM2MAudioVolumeReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->Level );
}

/*
 * This function packs the SLQSSetM2MAudioVolume parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSSetM2MAudioVolume(
    WORD                  *pMlength,
    BYTE                  *pParamField,
    SetM2MAudioVolumeReq  *setM2MAudioVolumeReq)
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_SET_M2M_AUDIO_VOLUME_PROFILE,   &BuildTlvSetVolumeProfile },
        { eTLV_SET_M2M_AUDIO_VOLUME_GENERATOR, &BuildTlvSetGenerator },
        { eTLV_SET_M2M_AUDIO_VOLUME_LEVEL,     &BuildTlvSetLevel },
        { eTLV_TYPE_INVALID,                   NULL } /* Important. Sentinel.
                                                       * Signifies last item in map
                                                       */
    };

    enum   eQCWWANError                      eRCode;
    struct QmiAudioSLQSSetM2MAudioVolumeReq req;

    req.setM2MAudioVolumeReq = setM2MAudioVolumeReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_SWIAUDIO_SET_VOLUME,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSSetM2MAudioVolume response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSSetM2MAudioVolume(
    BYTE                                     *pMdmResp,
    struct QmiAudioSLQSSetM2MAudioVolumeResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_SET_M2M_AUDIO_VOLUME_EC,  &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID,             NULL } /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_SWIAUDIO_SET_VOLUME );
    return eRCode;
}
