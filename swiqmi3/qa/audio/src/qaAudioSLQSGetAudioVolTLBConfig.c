/*
 * \ingroup audio
 *
 * \file   qaAudioSLQSGetAudioVolTLBConfig.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_AUDIO_GET_AUDIO_VOL_TLB_CONFIG message.
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
#include "qaAudioSLQSGetAudioVolTLBConfig.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSGetAudioVolTLBConfig audio profile field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAudioProfile(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSGetVolTLBConfigReq *pReq =
                        (struct QmiAudioSLQSGetVolTLBConfigReq *)pParam;
    GetAudioVolTLBConfigReq *tReq = pReq->pGetAudioVolTLBConfigReq;

    /* Insert the profile into the QMI message */
    return PutByte( pBuf, tReq->Profile);
}

/*
 * This function packs the SLQSGetAudioVolTLBConfig Generator field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAudioGen(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSGetVolTLBConfigReq *pReq =
                        (struct QmiAudioSLQSGetVolTLBConfigReq *)pParam;
    GetAudioVolTLBConfigReq *tReq = pReq->pGetAudioVolTLBConfigReq;

    /* Insert the generator into the QMI message */
    return PutByte( pBuf, tReq->Generator);
}

/*
 * This function packs the SLQSGetAudioVolTLBConfig volume field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAudioVolume(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSGetVolTLBConfigReq *pReq =
                        (struct QmiAudioSLQSGetVolTLBConfigReq *)pParam;
    GetAudioVolTLBConfigReq *tReq = pReq->pGetAudioVolTLBConfigReq;

    /* Insert the volume into the QMI message */
    return PutByte( pBuf, tReq->Volume);
}

/*
 * This function packs the SLQSGetAudioVolTLBConfig item field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAudioTLBItem(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSGetVolTLBConfigReq *pReq =
                        (struct QmiAudioSLQSGetVolTLBConfigReq *)pParam;
    GetAudioVolTLBConfigReq *tReq = pReq->pGetAudioVolTLBConfigReq;

    /* Insert the item into the QMI message */
    return PutByte( pBuf, tReq->Item);
}

/*
 * This function packs the SLQSGetAudioVolTLBConfig parameters to the QMI
 * message SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \param  pGetAudioVolTLBConfigReq [IN] - Request parameters for packing.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiAudioSLQSGetAudioVolTLBConfig(
    WORD                    *pMlength,
    BYTE                    *pParamField,
    GetAudioVolTLBConfigReq *pGetAudioVolTLBConfigReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_AUDIO_TLB_PROFILE,   &BuildTlvAudioProfile },
        { eTLV_AUDIO_TLB_GENERATOR, &BuildTlvAudioGen },
        { eTLV_AUDIO_TLB_VOLUME,    &BuildTlvAudioVolume },
        { eTLV_AUDIO_TLB_ITEM,      &BuildTlvAudioTLBItem },
        { eTLV_TYPE_INVALID,        NULL } /* Important. Sentinel.
                                            * Signifies last item in map
                                            */
    };

    struct QmiAudioSLQSGetVolTLBConfigReq req;
    enum   eQCWWANError                        eRCode;

    /* Intialize the request structure */
    slmemset( (char *)&req, 0, sizeof(struct QmiAudioSLQSGetVolTLBConfigReq) );
    req.pGetAudioVolTLBConfigReq   = pGetAudioVolTLBConfigReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_AUDIO_GET_AUDIO_VOL_TLB_CONFIG,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSGetAudioProfile value from the
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
enum eQCWWANError UnpackTlvAudioTLBValue(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSGetVolTLBConfigResp *lResp =
        (struct QmiAudioSLQSGetVolTLBConfigResp *)pResp;
    GetAudioVolTLBConfigResp *tResp = lResp->pGetAudioVolTLBConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    /* Get value in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    tResp->ResCode = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetAudioVolTLBConfig response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSGetAudioVolTLBConfig(
    BYTE                                   *pMdmResp,
    struct QmiAudioSLQSGetVolTLBConfigResp *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,          &qmUnpackTlvResultCode },
        { eTLV_AUDIO_VOL_TLB_VALUE,  &UnpackTlvAudioTLBValue },
        { eTLV_TYPE_INVALID,         NULL } /* Important. Sentinel.
                                             * Signifies last item in map.
                                             */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_AUDIO_GET_AUDIO_VOL_TLB_CONFIG );
    return eRCode;
}
