/*
 * \ingroup ims
 *
 * \file   qaImsSLQSGetRegMgrConfig.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_IMS_SETTINGS_GET_REG_MGR_CONFIG message.
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
#include "qaImsSLQSGetRegMgrConfig.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSGetRegMgrConfig parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiImsSLQSGetRegMgrConfig(
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
                      eQMI_IMS_SETTINGS_GET_REG_MGR_CONFIG,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSGetRegMgrConfig Settings Resp from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvSettResp(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetRegMgrConfigResp *lResp =
        (struct QmiImsSLQSGetRegMgrConfigResp *)pResp;
    GetRegMgrConfigParams *tResp = lResp->pGetRegMgrConfigParams;

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
 * This function unpacks the SLQSGetRegMgrConfig Proxy CSCF from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvProCSCF(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetRegMgrConfigResp *lResp =
        (struct QmiImsSLQSGetRegMgrConfigResp *)pResp;
    GetRegMgrConfigParams *tResp = lResp->pGetRegMgrConfigParams;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    if( NULL == tResp->pPCSCFPort )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Proxy CSCF port number in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pPCSCFPort) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetRegMgrConfig Primary CSCF from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvPriCSCF(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetRegMgrConfigResp *lResp =
        (struct QmiImsSLQSGetRegMgrConfigResp *)pResp;
    GetRegMgrConfigParams *tResp = lResp->pGetRegMgrConfigParams;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if( NULL == tResp->pPriCSCFPortNameLen ||
        NULL == tResp->pPriCSCFPortName )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Primary CSCF name string */
    eRCode = qmQmiExtractString( pTlvData,
                                 (CHAR *)tResp->pPriCSCFPortName,
                                 (ULONG)*tResp->pPriCSCFPortNameLen );
    return eRCode;
}

/*
 * This function unpacks the SLQSGetRegMgrConfig IMS test mode from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvIMSTestMode(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetRegMgrConfigResp *lResp =
        (struct QmiImsSLQSGetRegMgrConfigResp *)pResp;
    GetRegMgrConfigParams *tResp = lResp->pGetRegMgrConfigParams;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pIMSTestMode )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get IMS test mode in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pIMSTestMode) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetRegMgrConfig response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiImsSLQSGetRegMgrConfig(
    BYTE                                 *pMdmResp,
    struct QmiImsSLQSGetRegMgrConfigResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,   &qmUnpackTlvResultCode },
        { eTLV_SETTING_RESP,  &UnpackTlvSettResp },
        { eTLV_PCSCF_PORT,    &UnpackTlvProCSCF },
        { eTLV_PRI_CSCF_PORT, &UnpackTlvPriCSCF },
        { eTLV_IMS_TEST_MODE, &UnpackTlvIMSTestMode },
        { eTLV_TYPE_INVALID,  NULL } /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_IMS_SETTINGS_GET_REG_MGR_CONFIG );
    return eRCode;
}
