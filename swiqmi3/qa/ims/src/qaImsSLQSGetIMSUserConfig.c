/*
 * \ingroup ims
 *
 * \file   qaImsSLQSGetIMSUserConfig.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_IMS_SETTINGS_GET_IMS_USER_CONFIG message.
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
#include "qaImsSLQSGetIMSUserConfig.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSGetIMSUserConfig parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiImsSLQSGetIMSUserConfig(
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
                      eQMI_IMS_SETTINGS_GET_IMS_USR_CONFIG,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSGetIMSUserConfig Settings Resp from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvSetingResp(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSUserConfigResp *lResp =
        (struct QmiImsSLQSGetIMSUserConfigResp *)pResp;
    GetIMSUserConfigParams *tResp = lResp->pGetIMSUserConfigParams;

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
 * This function unpacks the SLQSGetIMSUserConfig IMS Domain Name from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvIMSDomain(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSUserConfigResp *lResp =
        (struct QmiImsSLQSGetIMSUserConfigResp *)pResp;
    GetIMSUserConfigParams *tResp = lResp->pGetIMSUserConfigParams;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if( NULL == tResp->pIMSDomainLen ||
        NULL == tResp->pIMSDomain )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get IMS Domain */
    eRCode = qmQmiExtractString( pTlvData,
                                 (CHAR *)tResp->pIMSDomain,
                                 (ULONG)*tResp->pIMSDomainLen );
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSUserConfig response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiImsSLQSGetIMSUserConfig(
    BYTE                                  *pMdmResp,
    struct QmiImsSLQSGetIMSUserConfigResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,  &qmUnpackTlvResultCode },
        { eTLV_SETING_RESP,  &UnpackTlvSetingResp },
        { eTLV_IMS_DOMAIN,   &UnpackTlvIMSDomain },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map.
                                     */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_IMS_SETTINGS_GET_IMS_USR_CONFIG );
    return eRCode;
}
