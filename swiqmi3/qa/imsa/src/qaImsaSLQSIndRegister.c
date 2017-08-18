/*
 * \ingroup imsa
 *
 * \file   qaImsaSLQSIndRegister.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_IMSA_INDICATION_REGISTER message.
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"
#include "qaGobiApiImsa.h"
#include "qaImsaSLQSIndRegister.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSRegisterIMSAIndication Registration Status Config TLV
 * to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvIMSARegStatusConfig(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    IMSAIndRegisterInfo *pReq = (IMSAIndRegisterInfo *)pParam;

    if( NULL == pReq->pRegStatusConfig )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Add Tlv data */
    return PutByte( pBuf, *(pReq->pRegStatusConfig) );
}

/*
 * This function packs the SLQSRegisterIMSAIndication service status config TLV
 * to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvIMSARServiceStatusConfig(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    IMSAIndRegisterInfo *pReq = (IMSAIndRegisterInfo *)pParam;

    if( NULL == pReq->pServiceStatusConfig )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Add Tlv data */
    return PutByte( pBuf, *(pReq->pServiceStatusConfig) );
}

/*
 * This function packs the SLQSRegisterIMSAIndication RAT handover config indication TLV
 * to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvIMSARatHandoverStatusConfig(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    IMSAIndRegisterInfo *pReq = (IMSAIndRegisterInfo *)pParam;

    if( NULL == pReq->pRatHandoverStatusConfig )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Add Tlv data */
    return PutByte( pBuf, *(pReq->pRatHandoverStatusConfig) );
}

/*
 * This function packs the SLQSRegisterIMSAIndication parameters to the
 * QMI message SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \param  pImsaIndRegisterInfo [IN] - Request parameters for packing.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiImsaSLQSIndRegister(
    WORD                  *pMlength,
    BYTE                  *pParamField,
    IMSAIndRegisterInfo   *pImsaIndRegisterInfo )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_IMSA_REG_STATUS_CONFIG,              &BuildTlvIMSARegStatusConfig },
        { eTLV_IMSA_SVC_STATUS_CONFIG,              &BuildTlvIMSARServiceStatusConfig },
        { eTLV_IMSA_RAT_HANDOVER_STATUS_CONFIG,     &BuildTlvIMSARatHandoverStatusConfig },
        { eTLV_TYPE_INVALID,              NULL } /* Important. Sentinel.
                                                  * Signifies last item in map
                                                  */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pImsaIndRegisterInfo,
                      map,
                      eQMI_IMSA_INDICATION_REGISTER,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the SLQSRegisterIMSAIndication response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiImsaSLQSIndRegister(
    BYTE                                   *pMdmResp,
    struct QmiImsaSLQSIndRegisterResp      *pApiResp)
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
                           eQMI_IMSA_INDICATION_REGISTER );
    return eRCode;
}

