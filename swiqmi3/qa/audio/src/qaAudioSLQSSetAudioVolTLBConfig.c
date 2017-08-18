/*
 * \ingroup audio
 *
 * \file   qaAudioSLQSSetAudioVolTLBConfig.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_AUDIO_SET_AUDIO_VOL_TLB_CONFIG message.
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
#include "qaAudioSLQSSetAudioVolTLBConfig.h"
#include "qaAudioSLQSGetAudioVolTLBConfig.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetAudioVolTLBConfig audio profile field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAudioProf(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetVolTLBConfigReq *pReq =
                        (struct QmiAudioSLQSSetVolTLBConfigReq *)pParam;
    SetAudioVolTLBConfigReq *tReq = pReq->pSetAudioVolTLBConfigReq;

    /* Insert the profile into the QMI message */
    return PutByte( pBuf, tReq->Profile );
}

/*
 * This function packs the SLQSSetAudioVolTLBConfig Generator field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAudioGenerate(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetVolTLBConfigReq *pReq =
                        (struct QmiAudioSLQSSetVolTLBConfigReq *)pParam;
    SetAudioVolTLBConfigReq *tReq = pReq->pSetAudioVolTLBConfigReq;

    /* Insert the generator into the QMI message */
    return PutByte( pBuf, tReq->Generator );
}

/*
 * This function packs the SLQSSetAudioVolTLBConfig volume field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAudioVol(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetVolTLBConfigReq *pReq =
                        (struct QmiAudioSLQSSetVolTLBConfigReq *)pParam;
    SetAudioVolTLBConfigReq *tReq = pReq->pSetAudioVolTLBConfigReq;

    /* Insert the volume into the QMI message */
    return PutByte( pBuf, tReq->Volume );
}

/*
 * This function packs the SLQSSetAudioVolTLBConfig item field to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAudioTLBItm(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetVolTLBConfigReq *pReq =
                        (struct QmiAudioSLQSSetVolTLBConfigReq *)pParam;
    SetAudioVolTLBConfigReq *tReq = pReq->pSetAudioVolTLBConfigReq;

    /* Insert the item into the QMI message */
    return PutByte( pBuf, tReq->Item );
}

/*
 * This function packs the SLQSSetAudioVolTLBConfig Volume table value to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will be
 *                         placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAudioVolTable(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiAudioSLQSSetVolTLBConfigReq *pReq =
                        (struct QmiAudioSLQSSetVolTLBConfigReq *)pParam;
    SetAudioVolTLBConfigReq *tReq = pReq->pSetAudioVolTLBConfigReq;

    /* Insert the item into the QMI message */
    return PutWord( pBuf, tReq->VolValue );
}

/*
 * This function packs the SLQSSetAudioVolTLBConfig parameters to the QMI
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
enum eQCWWANError PkQmiAudioSLQSSetAudioVolTLBConfig(
    WORD                    *pMlength,
    BYTE                    *pParamField,
    SetAudioVolTLBConfigReq *pSetAudioVolTLBConfigReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_AUDIO_TLB_PROFILE,   &BuildTlvAudioProf },
        { eTLV_AUDIO_TLB_GENERATOR, &BuildTlvAudioGenerate },
        { eTLV_AUDIO_TLB_VOLUME,    &BuildTlvAudioVol },
        { eTLV_AUDIO_TLB_ITEM,      &BuildTlvAudioTLBItm },
        { eTLV_AUDIO_TLB_VOL_TABLE, &BuildTlvAudioVolTable },
        { eTLV_TYPE_INVALID,        NULL } /* Important. Sentinel.
                                            * Signifies last item in map
                                            */
    };

    struct QmiAudioSLQSSetVolTLBConfigReq req;
    enum   eQCWWANError                        eRCode;

    /* Intialize the request structure */
    slmemset( (char *)&req, 0, sizeof(struct QmiAudioSLQSSetVolTLBConfigReq) );
    req.pSetAudioVolTLBConfigReq   = pSetAudioVolTLBConfigReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_AUDIO_SET_AUDIO_VOL_TLB_CONFIG,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSSetAudioProfile value from the
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
enum eQCWWANError UnpackTlvAudioTLBVal(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiAudioSLQSSetVolTLBConfigResp *lResp =
        (struct QmiAudioSLQSSetVolTLBConfigResp *)pResp;
    SetAudioVolTLBConfigResp *tResp = lResp->pSetAudioVolTLBConfigResp;

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
 * This function unpacks the SLQSSetAudioVolTLBConfig response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiAudioSLQSSetAudioVolTLBConfig(
    BYTE                                   *pMdmResp,
    struct QmiAudioSLQSSetVolTLBConfigResp *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,          &qmUnpackTlvResultCode },
        { eTLV_AUDIO_TLB_CFG_VALUE,  &UnpackTlvAudioTLBVal },
        { eTLV_TYPE_INVALID,         NULL } /* Important. Sentinel.
                                             * Signifies last item in map.
                                             */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_AUDIO_SET_AUDIO_VOL_TLB_CONFIG );
    return eRCode;
}
