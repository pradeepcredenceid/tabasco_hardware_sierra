/*
 * \ingroup swiaudio
 *
 * \file   qaAudioSLQSSetM2MAudioProfile.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_M2M_AUDIO_SET_PROFILE_MSG message.
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
#include "qaAudioSLQSSetM2MAudioProfile.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetM2MAudioProfile Profile field to the
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
enum eQCWWANError BuildTlvSetProfile( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAudioProfileReq *pReq =
                     (struct QmiAudioSLQSSetM2MAudioProfileReq *)pParam;

    SetM2MAudioProfileReq *lReq =
                     (SetM2MAudioProfileReq *)pReq->setM2MAudioProfileReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->Profile );
}

/*
 * This function packs the SLQSSetM2MAudioProfile Ear Mute field to the
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
enum eQCWWANError BuildTlvSetProfileEM( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAudioProfileReq *pReq =
                     (struct QmiAudioSLQSSetM2MAudioProfileReq *)pParam;

    SetM2MAudioProfileReq *lReq =
                     (SetM2MAudioProfileReq *)pReq->setM2MAudioProfileReq;

    if( NULL == lReq->pEarMute )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Add TLV data */
    return PutByte( pBuf, *(lReq->pEarMute) );
}

/*
 * This function packs the SLQSSetM2MAudioProfile Mic Mute field to the
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
enum eQCWWANError BuildTlvSetProfileMM( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAudioProfileReq *pReq =
                     (struct QmiAudioSLQSSetM2MAudioProfileReq *)pParam;

    SetM2MAudioProfileReq *lReq =
                     (SetM2MAudioProfileReq *)pReq->setM2MAudioProfileReq;

    if( NULL == lReq->pMicMute )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Add TLV data */
    return PutByte( pBuf, *(lReq->pMicMute) );
}

/*
 * This function packs the SLQSSetM2MAudioProfile Generator field to the
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
enum eQCWWANError BuildTlvSetProfileGnrt( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAudioProfileReq *pReq =
                     (struct QmiAudioSLQSSetM2MAudioProfileReq *)pParam;

    SetM2MAudioProfileReq *lReq =
                     (SetM2MAudioProfileReq *)pReq->setM2MAudioProfileReq;

    if( NULL == lReq->pGenerator )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Add TLV data */
    return PutByte( pBuf, *(lReq->pGenerator) );
}

/*
 * This function packs the SLQSSetM2MAudioProfile Volume field to the
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
enum eQCWWANError BuildTlvSetProfileVol( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAudioProfileReq *pReq =
                     (struct QmiAudioSLQSSetM2MAudioProfileReq *)pParam;

    SetM2MAudioProfileReq *lReq =
                     (SetM2MAudioProfileReq *)pReq->setM2MAudioProfileReq;

    if( NULL == lReq->pVolume )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Add TLV data */
    return PutByte( pBuf, *(lReq->pVolume) );
}

/*
 * This function packs the SLQSSetM2MAudioProfile CWT Mute field to the
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
enum eQCWWANError BuildTlvSetProfileCWM( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAudioProfileReq *pReq =
                     (struct QmiAudioSLQSSetM2MAudioProfileReq *)pParam;

    SetM2MAudioProfileReq *lReq =
                     (SetM2MAudioProfileReq *)pReq->setM2MAudioProfileReq;

    if( NULL == lReq->pCwtMute )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Add TLV data */
    return PutByte( pBuf, *(lReq->pCwtMute) );
}

/*
 * This function packs the SLQSSetM2MAudioProfile parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSSetM2MAudioProfile(
    WORD                  *pMlength,
    BYTE                  *pParamField,
    SetM2MAudioProfileReq *setM2MAudioProfileReq)
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_SET_M2M_AUDIO_PROFILE,         &BuildTlvSetProfile },
        { eTLV_SET_M2M_AUDIO_PROFILE_EARMUTE, &BuildTlvSetProfileEM },
        { eTLV_SET_M2M_AUDIO_PROFILE_MICMUTE, &BuildTlvSetProfileMM },
        { eTLV_SET_M2M_AUDIO_PROFILE_GENRT,   &BuildTlvSetProfileGnrt },
        { eTLV_SET_M2M_AUDIO_PROFILE_VOL,     &BuildTlvSetProfileVol },
        { eTLV_SET_M2M_AUDIO_PROFILE_CWTMUT,  &BuildTlvSetProfileCWM },
        { eTLV_TYPE_INVALID,                  NULL } /* Important. Sentinel.
                                                      * Signifies last item in map
                                                      */
    };

    enum   eQCWWANError                      eRCode;
    struct QmiAudioSLQSSetM2MAudioProfileReq req;

    req.setM2MAudioProfileReq = setM2MAudioProfileReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_SWIAUDIO_SET_PROFILE,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

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
enum eQCWWANError UpkQmiAudioSLQSSetM2MAudioProfile(
    BYTE                                      *pMdmResp,
    struct QmiAudioSLQSSetM2MAudioProfileResp *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_SET_M2M_AUDIO_PROFILE_EC,  &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID,              NULL } /* Important. Sentinel.
                                                  * Signifies last item in map.
                                                  */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_SWIAUDIO_SET_PROFILE );
    return eRCode;
}
