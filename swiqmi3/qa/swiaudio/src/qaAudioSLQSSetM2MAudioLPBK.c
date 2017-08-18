/*
 * \ingroup swiaudio
 *
 * \file   qaAudioSLQSSetM2MAudioLPBK.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_M2M_AUDIO_SET_LPBK_MSG message.
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
#include "qaAudioSLQSSetM2MAudioLPBK.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetM2MAudioLPBK Enable field to the
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
enum eQCWWANError BuildTlvSetEnable( BYTE *pBuf, BYTE *pParam )
{
    struct QmiAudioSLQSSetM2MAudioLPBKReq *pReq =
                     (struct QmiAudioSLQSSetM2MAudioLPBKReq *)pParam;

    SetM2MAudioLPBKReq *lReq =
                     (SetM2MAudioLPBKReq *)pReq->pSetM2MAudioLPBKReq;

    /* Add TLV data */
    return PutByte( pBuf, lReq->Enable );
}

/*
 * This function packs the SLQSSetM2MAudioLPBK parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSSetM2MAudioLPBK(
    WORD               *pMlength,
    BYTE               *pParamField,
    SetM2MAudioLPBKReq *pSetM2MAudioLPBKReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_SET_M2M_AUDIO_ENABLE, &BuildTlvSetEnable },
        { eTLV_TYPE_INVALID,         NULL } /* Important. Sentinel.
                                             * Signifies last item in map
                                             */
    };

    enum   eQCWWANError                   eRCode;
    struct QmiAudioSLQSSetM2MAudioLPBKReq req;

    req.pSetM2MAudioLPBKReq = pSetM2MAudioLPBKReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_SWIAUDIO_SET_LPBK,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSSetM2MAudioLPBK response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSSetM2MAudioLPBK(
    BYTE                                   *pMdmResp,
    struct QmiAudioSLQSSetM2MAudioLPBKResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_SET_M2M_AUDIO_LPBK_EC, &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID,          NULL } /* Important. Sentinel.
                                              * Signifies last item in map.
                                              */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_SWIAUDIO_SET_LPBK );
    return eRCode;
}
