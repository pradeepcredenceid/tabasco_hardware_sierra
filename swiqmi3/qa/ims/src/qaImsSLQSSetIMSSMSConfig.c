/*
 * \ingroup ims
 *
 * \file   qaImsSLQSSetIMSSMSConfig.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_IMS_SETTINGS_SET_IMS_SMS_CFG message.
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
#include "qaGobiApiIms.h"
#include "qaImsSLQSSetIMSSMSConfig.h"
#include "qaImsSLQSSetSIPConfig.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetIMSSMSConfig SMS Format to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvSMSFormat(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSSMSConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSSMSConfigReq *)pParam;
    SetIMSSMSConfigReq *tReq = pReq->pSetIMSSMSConfigReq;

    if( NULL == tReq->pSMSFormat )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the SMS Format into the QMI message */
    return PutByte( pBuf, *(tReq->pSMSFormat) );
}

/*
 * This function packs the SLQSSetIMSSMSConfig SMS Over IP Network Indication
 * Flag to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvSMSIPNw(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSSMSConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSSMSConfigReq *)pParam;
    SetIMSSMSConfigReq *tReq = pReq->pSetIMSSMSConfigReq;

    if( NULL == tReq->pSMSOverIPNwInd )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the SMS Over IP NW Indication flag into the QMI message */
    return PutByte( pBuf, *(tReq->pSMSOverIPNwInd) );
}

/*
 * This function packs the SLQSSetIMSSMSConfig Phone Context Universal
 * Resource Identifier to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvPhCtxtURI(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSSMSConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSSMSConfigReq *)pParam;
    SetIMSSMSConfigReq *tReq = pReq->pSetIMSSMSConfigReq;

    enum eQCWWANError eRCode  = eQCWWAN_ERR_NONE;
    BYTE              lCtr    = 0;
    BYTE              lStrlen = 0;

    if( NULL == tReq->pPhoneCtxtURILen ||
        NULL == tReq->pPhoneCtxtURI )
    {
        return eQCWWAN_ERR_NONE;
    }

    lStrlen  = *(tReq->pPhoneCtxtURILen);

    /* Insert the Phone Context URI into the QMI message */
    for( lCtr = 0; lCtr < lStrlen; lCtr++ )
    {
        eRCode = PutByte( pBuf, tReq->pPhoneCtxtURI[lCtr] );
        if (eQCWWAN_ERR_NONE != eRCode)
        {
            return eRCode;
        }
    }

    return eRCode;
}

/*
 * This function packs the SLQSSetIMSSMSConfig parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \param  pSetIMSSMSConfigReq [IN] - Request parameters for packing.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiImsSLQSSetIMSSMSConfig(
    WORD               *pMlength,
    BYTE               *pParamField,
    SetIMSSMSConfigReq *pSetIMSSMSConfigReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_SMS_FORMAT,         &BuildTlvSMSFormat },
        { eTLV_SMS_IP_NW_IND_FLAG, &BuildTlvSMSIPNw },
        { eTLV_PHONE_CTXT_URI,     &BuildTlvPhCtxtURI },
        { eTLV_TYPE_INVALID,       NULL } /* Important. Sentinel.
                                           * Signifies last item in map
                                           */
    };

    struct QmiImsSLQSSetIMSSMSConfigReq req;
    enum   eQCWWANError                 eRCode;

    /* Intialize the request structure */
    slmemset( (char *)&req,
              0,
              sizeof(struct QmiImsSLQSSetIMSSMSConfigReq) );
    req.pSetIMSSMSConfigReq = pSetIMSSMSConfigReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_IMS_SETTINGS_SET_IMS_SMS_CFG,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSSetIMSSMSConfig Settings Resp from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvSetResp(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSSetIMSSMSConfigResp *lResp =
        (struct QmiImsSLQSSetIMSSMSConfigResp *)pResp;
    SetIMSSMSConfigResp *tResp = lResp->pSetIMSSMSConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pSettingResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Settings Response in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pSettingResp) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSSetIMSSMSConfig response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiImsSLQSSetIMSSMSConfig(
    BYTE                                 *pMdmResp,
    struct QmiImsSLQSSetIMSSMSConfigResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,   &qmUnpackTlvResultCode },
        { eTLV_SETTINGS_RESP, &UnpackTlvSetResp },
        { eTLV_TYPE_INVALID,  NULL } /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_IMS_SETTINGS_SET_IMS_SMS_CFG );
    return eRCode;
}

