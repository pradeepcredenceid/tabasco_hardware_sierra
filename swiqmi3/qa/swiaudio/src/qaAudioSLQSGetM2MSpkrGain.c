/*
 * \ingroup swiaudio
 *
 * \file   qaAudioSLQSGetM2MSpkrGain.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_M2M_AUDIO_GET_SPKRGAIN_MSG message.
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
#include "qaAudioSLQSGetM2MSpkrGain.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSGetM2MSpkrGain Profile field to the
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
enum eQCWWANError BuildTlvGetSpkrGainProfile( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSGetM2MSpkrGainReq *pReq =
                 (struct QmiAudioSLQSGetM2MSpkrGainReq *)pParam;

    GetM2MSpkrGainReq *lReq =
                     (GetM2MSpkrGainReq *)pReq->pSpkrGainReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->Profile );
}

/*
 * This function packs the SLQSGetM2MSpkrGain parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSGetM2MSpkrGain(
    WORD               *pMlength,
    BYTE               *pParamField,
    GetM2MSpkrGainReq  *pSpkrGainReq)
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_GET_M2M_SPKRGAIN_PROFILE, &BuildTlvGetSpkrGainProfile },
        { eTLV_TYPE_INVALID,             NULL } /* Important. Sentinel.
                                                 * Signifies last item in map
                                                 */
    };

    enum   eQCWWANError                  eRCode;
    struct QmiAudioSLQSGetM2MSpkrGainReq req;

    req.pSpkrGainReq = pSpkrGainReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_SWIAUDIO_GET_SPKRGAIN,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSGetM2MSpkrGain Value from the
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
enum eQCWWANError UnpackTLVM2MSpkrGainValue(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetM2MSpkrGainResp *lResp =
        (struct QmiAudioSLQSGetM2MSpkrGainResp *)pResp;
    GetM2MSpkrGainResp *tResp = lResp->pSpkrGainResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    /* Get Volume Level in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    tResp->Value = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetM2MSpkrGain response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSGetM2MSpkrGain(
    BYTE                                  *pMdmResp,
    struct QmiAudioSLQSGetM2MSpkrGainResp *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,            &qmUnpackTlvResultCode },
        { eTLV_GET_M2M_SPKRGAIN_VALUE, &UnpackTLVM2MSpkrGainValue},
        { eTLV_TYPE_INVALID,           NULL } /* Important. Sentinel.
                                               * Signifies last item in map.
                                               */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_SWIAUDIO_GET_SPKRGAIN );
    return eRCode;
}
