/*
 * \ingroup swiaudio
 *
 * \file   qaAudioSLQSGetM2MSpkrGain.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_M2M_AUDIO_SET_SPKRGAIN_MSG message.
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
#include "qaAudioSLQSSetM2MSpkrGain.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetM2MSpkrGain Profile field to the
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
enum eQCWWANError BuildTlvSetSpkrGainProfile( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MSpkrGainReq *pReq =
                 (struct QmiAudioSLQSSetM2MSpkrGainReq *)pParam;

    SetM2MSpkrGainReq *lReq =
                     (SetM2MSpkrGainReq *)pReq->pSpkrGainReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->Profile );
}

/*
 * This function packs the SLQSSetM2MSpkrGain Value field to the
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
enum eQCWWANError BuildTlvSetSpkrgainValue( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MSpkrGainReq *pReq =
                 (struct QmiAudioSLQSSetM2MSpkrGainReq *)pParam;

    SetM2MSpkrGainReq *lReq =
                     (SetM2MSpkrGainReq *)pReq->pSpkrGainReq;

    /* Add TLV data */
    return PutWord( pBuf, lReq->Value );
}

/*
 * This function packs the SLQSSetM2MSpkrGain parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSSetM2MSpkrGain(
    WORD               *pMlength,
    BYTE               *pParamField,
    SetM2MSpkrGainReq  *pSpkrGainReq)
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_SET_M2M_SPKRGAIN_PROFILE, &BuildTlvSetSpkrGainProfile },
        { eTLV_SET_M2M_SPKRGAIN_VALUE,   &BuildTlvSetSpkrgainValue },
        { eTLV_TYPE_INVALID,             NULL } /* Important. Sentinel.
                                                 * Signifies last item in map
                                                 */
    };

    enum   eQCWWANError                  eRCode;
    struct QmiAudioSLQSSetM2MSpkrGainReq req;

    req.pSpkrGainReq = pSpkrGainReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_SWIAUDIO_SET_SPKRGAIN,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSSetM2MSpkrGain response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSSetM2MSpkrGain(
    BYTE                                  *pMdmResp,
    struct QmiAudioSLQSSetM2MSpkrGainResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_SET_M2M_SPKRGAIN_EC, &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID,        NULL } /* Important. Sentinel.
                                            * Signifies last item in map.
                                            */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_SWIAUDIO_SET_SPKRGAIN );
    return eRCode;
}
