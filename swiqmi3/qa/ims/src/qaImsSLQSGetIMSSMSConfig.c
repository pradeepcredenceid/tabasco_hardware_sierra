/*
 * \ingroup ims
 *
 * \file   qaImsSLQSGetIMSSMSConfig.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_IMS_SETTINGS_GET_IMS_SMS_CONFIG message.
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
#include "qaImsSLQSGetIMSSMSConfig.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSGetIMSSMSConfig parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiImsSLQSGetIMSSMSConfig(
    WORD            *pMlength,
    BYTE            *pParamField )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map
                                     */
    };

    enum   eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      NULL,
                      map,
                      eQMI_IMS_SETTINGS_GET_IMS_SMS_CONFIG,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSGetIMSSMSConfig Settings Resp from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvSettngResp(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSSMSConfigResp *lResp =
        (struct QmiImsSLQSGetIMSSMSConfigResp *)pResp;
    GetIMSSMSConfigParams *tResp = lResp->pGetIMSSMSConfigParams;

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
 * This function unpacks the SLQSGetIMSSMSConfig SMS Format from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvSMSFmt(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSSMSConfigResp *lResp =
        (struct QmiImsSLQSGetIMSSMSConfigResp *)pResp;
    GetIMSSMSConfigParams *tResp = lResp->pGetIMSSMSConfigParams;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pSMSFormat )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get SMS format in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pSMSFormat) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSSMSConfig SMS Over IP Network
 * Indication Flag from the QMI response message to a user provided
 * response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvSMSoIPNIF(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSSMSConfigResp *lResp =
        (struct QmiImsSLQSGetIMSSMSConfigResp *)pResp;
    GetIMSSMSConfigParams *tResp = lResp->pGetIMSSMSConfigParams;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pSMSOverIPNwInd )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get SMS over Network Indication Flag in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pSMSOverIPNwInd) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSSMSConfig Phone Ctxt URI from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvPhoneCtxtURI(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSSMSConfigResp *lResp =
        (struct QmiImsSLQSGetIMSSMSConfigResp *)pResp;
    GetIMSSMSConfigParams *tResp = lResp->pGetIMSSMSConfigParams;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if( NULL == tResp->pPhoneCtxtURILen ||
        NULL == tResp->pPhoneCtxtURI )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Phone Context URI */
    eRCode = qmQmiExtractString( pTlvData,
                                 (CHAR *)tResp->pPhoneCtxtURI,
                                 (ULONG)*tResp->pPhoneCtxtURILen );
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSSMSConfig response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiImsSLQSGetIMSSMSConfig(
    BYTE                                 *pMdmResp,
    struct QmiImsSLQSGetIMSSMSConfigResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,    &qmUnpackTlvResultCode },
        { eTLV_SETNG_RESP,     &UnpackTlvSettngResp },
        { eTLV_SMS_FMT,        &UnpackTlvSMSFmt },
        { eTLV_SMS_OVER_IPNIF, &UnpackTlvSMSoIPNIF },
        { eTLV_PH_CTXT_URI,    &UnpackTlvPhoneCtxtURI },
        { eTLV_TYPE_INVALID,   NULL } /* Important. Sentinel.
                                       * Signifies last item in map.
                                       */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_IMS_SETTINGS_GET_IMS_SMS_CONFIG );
    return eRCode;
}



