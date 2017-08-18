/*
 * \ingroup swiaudio
 *
 * \file   qaAudioSLQSGetM2MAudioProfile.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_M2M_AUDIO_GET_PROFILE_MSG message.
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
#include "qaAudioSLQSGetM2MAudioProfile.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSGetM2MAudioProfile generator field to the
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
enum eQCWWANError BuildTlvGetM2MAudProfileGen( BYTE *pBuf, BYTE *pParam )
{
    GetM2MAudioProfileReq *lReq =
                     (GetM2MAudioProfileReq *)pParam;

    if ( NULL == lReq->pGenerator )
        return eQCWWAN_ERR_NONE;

    /* Add TLV data */
    return PutByte( pBuf, *(lReq->pGenerator) );
}

/*
 * This function packs the SLQSGetM2MAudioProfile parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSGetM2MAudioProfile(
    WORD *pMlength,
    BYTE *pParamField,
    GetM2MAudioProfileReq *pGetM2MAudioProfileReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_GET_M2M_AUDIO_GEN,  &BuildTlvGetM2MAudProfileGen },
        { eTLV_TYPE_INVALID,       NULL } /* Important. Sentinel.
                                           * Signifies last item in map
                                           */
    };

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pGetM2MAudioProfileReq,
                      map,
                      eQMI_SWIAUDIO_GET_PROFILE,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSGetM2MAudioProfile Audio profile from the
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
enum eQCWWANError UnpackTlvM2MAudioProfile(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetM2MAudioProfileResp *lResp =
        (struct QmiAudioSLQSGetM2MAudioProfileResp *)pResp;

    GetM2MAudioProfileResp *tResp = lResp->pGetM2MAudioProfileResp;

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
 * This function unpacks the SLQSGetM2MAudioProfile Ear Mute from the
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
enum eQCWWANError UnpackTLVM2MAudioEarMute(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetM2MAudioProfileResp *lResp =
        (struct QmiAudioSLQSGetM2MAudioProfileResp *)pResp;

    GetM2MAudioProfileResp *tResp = lResp->pGetM2MAudioProfileResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    /* Get Audio Profile in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    tResp->EarMute = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetM2MAudioProfile Mic Mute from the
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
enum eQCWWANError UnpackTLVM2MAudioMicMute(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetM2MAudioProfileResp *lResp =
        (struct QmiAudioSLQSGetM2MAudioProfileResp *)pResp;

    GetM2MAudioProfileResp *tResp = lResp->pGetM2MAudioProfileResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    /* Get Mic Mute in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    tResp->MicMute = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetM2MAudioProfile Generator from the
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
enum eQCWWANError UnpackTLVM2MAudioGenerator(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetM2MAudioProfileResp *lResp =
        (struct QmiAudioSLQSGetM2MAudioProfileResp *)pResp;

    GetM2MAudioProfileResp *tResp = lResp->pGetM2MAudioProfileResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    /* Get generator in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    tResp->Generator = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetM2MAudioProfile Volume from the
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
enum eQCWWANError UnpackTLVM2MAudioVolume(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetM2MAudioProfileResp *lResp =
        (struct QmiAudioSLQSGetM2MAudioProfileResp *)pResp;

    GetM2MAudioProfileResp *tResp = lResp->pGetM2MAudioProfileResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    /* Get volume in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    tResp->Volume = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetM2MAudioProfile Cwt Mute from the
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
enum eQCWWANError UnpackTLVM2MAudioCwtMute(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetM2MAudioProfileResp *lResp =
        (struct QmiAudioSLQSGetM2MAudioProfileResp *)pResp;

    GetM2MAudioProfileResp *tResp = lResp->pGetM2MAudioProfileResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    /* Get cwt mute in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    tResp->CwtMute = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetM2MAudioProfile response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSGetM2MAudioProfile(
    BYTE                                      *pMdmResp,
    struct QmiAudioSLQSGetM2MAudioProfileResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_GET_M2M_AUDIO_RESCODE,   &qmUnpackTlvResultCode },
        { eTLV_GET_M2M_AUDIO_PROFILE,   &UnpackTlvM2MAudioProfile },
        { eTLV_GET_M2M_AUDIO_EARMUTE,   &UnpackTLVM2MAudioEarMute},
        { eTLV_GET_M2M_AUDIO_MICMUTE,   &UnpackTLVM2MAudioMicMute},
        { eTLV_GET_M2M_AUDIO_GENERATOR, &UnpackTLVM2MAudioGenerator},
        { eTLV_GET_M2M_AUDIO_VOLUME,    &UnpackTLVM2MAudioVolume},
        { eTLV_GET_M2M_AUDIO_CWTMUTE,   &UnpackTLVM2MAudioCwtMute},
        { eTLV_TYPE_INVALID,            NULL } /* Important. Sentinel.
                                                * Signifies last item in map.
                                                */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_SWIAUDIO_GET_PROFILE );
    return eRCode;
}

