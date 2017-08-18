/*
 * \ingroup ims
 *
 * \file   qaImsSLQSSetIMSUserConfig.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_IMS_SETTINGS_SET_IMS_USER_CFG message.
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
#include "qaImsSLQSSetIMSUserConfig.h"
#include "qaImsSLQSSetSIPConfig.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetIMSUserConfig IMS domain name to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvDomainName(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSUserConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSUserConfigReq *)pParam;
    SetIMSUserConfigReq *tReq = pReq->pSetIMSUserConfigReq;

    enum eQCWWANError eRCode  = eQCWWAN_ERR_NONE;
    BYTE              lCtr    = 0;
    BYTE              lStrlen = 0;

    if( NULL == tReq->pIMSDomain ||
        NULL == tReq->pIMSDomainLen )
    {
        return eQCWWAN_ERR_NONE;
    }

    lStrlen = *(tReq->pIMSDomainLen);
    for( lCtr = 0; lCtr < lStrlen; lCtr++ )
    {
        /* Insert the Domain Name into the QMI message */
        eRCode = PutByte( pBuf, tReq->pIMSDomain[lCtr] );
        if (eQCWWAN_ERR_NONE != eRCode)
        {
            return eRCode;
        }
    }

    return eRCode;
}

/*
 * This function packs the SLQSSetIMSUserConfig parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \param  pSetIMSUserConfigReq [IN] - Request parameters for packing.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiImsSLQSSetIMSUserConfig(
    WORD                *pMlength,
    BYTE                *pParamField,
    SetIMSUserConfigReq *pSetIMSUserConfigReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_IMS_DOMAIN_NAME,    &BuildTlvDomainName },
        { eTLV_TYPE_INVALID,       NULL } /* Important. Sentinel.
                                           * Signifies last item in map
                                           */
    };

    struct QmiImsSLQSSetIMSUserConfigReq req;
    enum   eQCWWANError                  eRCode;

    /* Intialize the request structure */
    slmemset( (char *)&req,
              0,
              sizeof(struct QmiImsSLQSSetIMSUserConfigReq) );
    req.pSetIMSUserConfigReq = pSetIMSUserConfigReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_IMS_SETTINGS_SET_IMS_USER_CFG,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSSetIMSUserConfig Settings Resp from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvUserCfgSetResp(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSSetIMSUserConfigResp *lResp =
        (struct QmiImsSLQSSetIMSUserConfigResp *)pResp;
    SetIMSUserConfigResp *tResp = lResp->pSetIMSUserConfigResp;

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
 * This function unpacks the SLQSSetIMSUserConfig response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiImsSLQSSetIMSUserConfig(
    BYTE                                  *pMdmResp,
    struct QmiImsSLQSSetIMSUserConfigResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,   &qmUnpackTlvResultCode },
        { eTLV_SETTINGS_RESP, &UnpackTlvUserCfgSetResp },
        { eTLV_TYPE_INVALID,  NULL } /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_IMS_SETTINGS_SET_IMS_USER_CFG );
    return eRCode;
}

