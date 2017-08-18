/*
 * \ingroup ims
 *
 * \file   qaImsSLQSImsConfigIndRegister.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_IMS_CONFIG_IND_REGISTER message.
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
#include "qaImsSLQSImsConfigIndRegister.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSImsConfigIndicationRegister SIP Config Events
 * to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvIMSSIPCfgEvents(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    imsCfgIndRegisterInfo *pReq = (imsCfgIndRegisterInfo *)pParam;

    if( NULL == pReq->pSIPConfigEvents )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Add Tlv data */
    return PutByte( pBuf, *(pReq->pSIPConfigEvents) );
}

/*
 * This function packs the SLQSImsConfigIndicationRegister Reg Mgr Config
 * Events to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvIMSRegMgrCfgEvents(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    imsCfgIndRegisterInfo *pReq = (imsCfgIndRegisterInfo *)pParam;

    if( NULL == pReq->pRegMgrConfigEvents )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Add Tlv data */
    return PutByte( pBuf, *(pReq->pRegMgrConfigEvents) );
}

/*
 * This function packs the SLQSImsConfigIndicationRegister SMS Config Events
 * to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvIMSSMSCfgEvents(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    imsCfgIndRegisterInfo *pReq = (imsCfgIndRegisterInfo *)pParam;

    if( NULL == pReq->pSMSConfigEvents )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Add Tlv data */
    return PutByte( pBuf, *(pReq->pSMSConfigEvents) );
}

/*
 * This function packs the SLQSImsConfigIndicationRegister User Config Events
 * to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvIMSUserCfgEvents(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    imsCfgIndRegisterInfo *pReq = (imsCfgIndRegisterInfo *)pParam;

    if( NULL == pReq->pUserConfigEvents )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Add Tlv data */
    return PutByte( pBuf, *(pReq->pUserConfigEvents) );
}

/*
 * This function packs the SLQSImsConfigIndicationRegister VoIP Config Events
 * to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvIMSVoIPCfgEvents(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    imsCfgIndRegisterInfo *pReq = (imsCfgIndRegisterInfo *)pParam;

    if( NULL == pReq->pVoIPConfigEvents )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Add Tlv data */
    return PutByte( pBuf, *(pReq->pVoIPConfigEvents) );
}

/*
 * This function packs the SLQSImsConfigIndicationRegister parameters to the
 * QMI message SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \param  pSetIMSVoIPConfigReq [IN] - Request parameters for packing.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiImsSLQSImsCfgIndRegister(
    WORD                  *pMlength,
    BYTE                  *pParamField,
    imsCfgIndRegisterInfo *pImsCfgIndRegisterInfo )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_IMS_SIP_CONFIG_EVENTS,     &BuildTlvIMSSIPCfgEvents },
        { eTLV_IMS_REG_MGR_CONFIG_EVENTS, &BuildTlvIMSRegMgrCfgEvents },
        { eTLV_IMS_SMS_CONFIG_EVENTS,     &BuildTlvIMSSMSCfgEvents },
        { eTLV_IMS_USER_CONFIG_EVENTS,    &BuildTlvIMSUserCfgEvents },
        { eTLV_IMS_VOIP_CONFIG_EVENTS,    &BuildTlvIMSVoIPCfgEvents },
        { eTLV_TYPE_INVALID,              NULL } /* Important. Sentinel.
                                                  * Signifies last item in map
                                                  */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pImsCfgIndRegisterInfo,
                      map,
                      eQMI_IMS_CONFIG_INDICATION_REGISTER,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the SLQSImsConfigIndicationRegister response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiImsSLQSImsCfgIndRegister(
    BYTE                                   *pMdmResp,
    struct QmiImsSLQSImsCfgIndRegisterResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,  &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map.
                                     */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_IMS_CONFIG_INDICATION_REGISTER );
    return eRCode;
}

