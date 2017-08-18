/*
 * \ingroup swiaudio
 *
 * \file   qaAudioSLQSGetM2MAVMute.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_M2M_AUDIO_GET_AVMUTE_MSG message.
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
#include "qaAudioSLQSGetM2MAVMute.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSGetM2MAVMute Profile field to the
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
enum eQCWWANError BuildTlvGetAVMuteProfile( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSGetM2MAVMuteReq *pReq =
                     (struct QmiAudioSLQSGetM2MAVMuteReq *)pParam;

    GetM2MAVMuteReq *lReq = (GetM2MAVMuteReq *)pReq->pGetM2MAVMuteReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->Profile );
}

/*
 * This function packs the SLQSGetM2MAVMute parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSGetM2MAVMute(
    WORD             *pMlength,
    BYTE             *pParamField,
    GetM2MAVMuteReq  *pGetM2MAVMuteReq)
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_GET_M2M_AVMUTE_PROFILE, &BuildTlvGetAVMuteProfile },
        { eTLV_TYPE_INVALID,           NULL } /* Important. Sentinel.
                                               * Signifies last item in map
                                               */
    };

    enum   eQCWWANError                eRCode;
    struct QmiAudioSLQSGetM2MAVMuteReq req;

    req.pGetM2MAVMuteReq = pGetM2MAVMuteReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_SWIAUDIO_GET_AVMUTE,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSGetM2MAVMute Ear Mute from the
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
enum eQCWWANError UnpackTLVM2MAVMuteEarMute(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetM2MAVMuteResp *lResp =
        (struct QmiAudioSLQSGetM2MAVMuteResp *)pResp;

    GetM2MAVMuteResp *tResp = lResp->pGetM2MAVMuteResp;

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
 * This function unpacks the SLQSGetM2MAVMute Mic Mute from the
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
enum eQCWWANError UnpackTLVM2MAVMuteMicMute(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetM2MAVMuteResp *lResp =
        (struct QmiAudioSLQSGetM2MAVMuteResp *)pResp;

    GetM2MAVMuteResp *tResp = lResp->pGetM2MAVMuteResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    /* Get Audio Profile in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    tResp->MicMute = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetM2MAVMute Cwt Mute from the
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
enum eQCWWANError UnpackTLVM2MAVMuteCwtMute(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetM2MAVMuteResp *lResp =
        (struct QmiAudioSLQSGetM2MAVMuteResp *)pResp;

    GetM2MAVMuteResp *tResp = lResp->pGetM2MAVMuteResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    /* Get Audio Profile in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    tResp->CwtMute = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetM2MAVMute response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSGetM2MAVMute(
    BYTE                                *pMdmResp,
    struct QmiAudioSLQSGetM2MAVMuteResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_GET_M2M_AVMUTE_EC,      &qmUnpackTlvResultCode },
        { eTLV_GET_M2M_AVMUTE_EARMUTE, &UnpackTLVM2MAVMuteEarMute },
        { eTLV_GET_M2M_AVMUTE_MICMUTE, &UnpackTLVM2MAVMuteMicMute },
        { eTLV_GET_M2M_AVMUTE_CWTMUTE, &UnpackTLVM2MAVMuteCwtMute },
        { eTLV_TYPE_INVALID,           NULL } /* Important. Sentinel.
                                               * Signifies last item in map.
                                               */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_SWIAUDIO_GET_AVMUTE );
    return eRCode;
}

