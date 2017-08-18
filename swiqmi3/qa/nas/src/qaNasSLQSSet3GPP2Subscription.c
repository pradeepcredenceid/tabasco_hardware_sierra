/*
 * \ingroup nas
 *
 * \file qaNasSLQSSet3GPP2Subscription.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         eQMI_NAS_SET_3GPP2_SUBSCRIPTION_INFO message.
 *
 * Copyright: © 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaNasSLQSSet3GPP2Subscription.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the NAM ID Information to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvSetNamID( BYTE *pBuf, BYTE *pParam )
{
    nasSet3GPP2SubscriptionInfoReq *pReq =
                                ( nasSet3GPP2SubscriptionInfoReq * )pParam;

    /* Insert NAM ID */
    return PutByte( pBuf, pReq->namID );
}


enum eQCWWANError BuildTlvSetMdn( BYTE *pBuf, BYTE *pParam )
{
    nasSet3GPP2SubscriptionInfoReq *pReq =
                                ( nasSet3GPP2SubscriptionInfoReq * )pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE Index;

    eRCode = PutByte( pBuf, pReq->pMdn->mdnLen );

    if(eRCode) return eRCode;
    for( Index=0; Index < pReq->pMdn->mdnLen; Index++)
    {
        eRCode = PutByte( pBuf, pReq->pMdn->mdn[Index] );
        if ( eQCWWAN_ERR_NONE != eRCode )
            return eRCode;
    }
    return eRCode;
}


/*
 * This function packs the Set3GPP2Subscription parameters to the QMI message SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 */
enum eQCWWANError PkQmiNasSlqsSet3GPP2SubscriptionInfo (
    WORD                           *pMlength,
    BYTE                           *pParamField,
    nasSet3GPP2SubscriptionInfoReq *pSet3GPP2SubsInfoReq )
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_NAM_ID_SET_REQ,   &BuildTlvSetNamID },
        { eTLV_MDN_3GPP2_SET_INFO,   &BuildTlvSetMdn },
        { eTLV_TYPE_INVALID, NULL }  /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };

    enum eQCWWANError eRCode;
    eRCode = qmbuild( pParamField,
                      (BYTE *)pSet3GPP2SubsInfoReq,
                      map,
                      eQMI_NAS_SET_3GPP2_SUBSCRIPTION,
                      pMlength );
    return eRCode;
}


/*
 * This function unpacks the SlqsSet3GPP2SubscriptionInfoResp response message
 * to to a user-provided response structure.
 *
 * \param     pMdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT]  - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiNasSlqsSet3GPP2SubscriptionInfo(
    BYTE                                          *pMdmResp,
    struct QmiNasSLQSSet3GPP2SubscriptionInfoResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,               &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID,              NULL }  /* Important. Sentinel.
                                                   * Signifies last item in map.
                                                   */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_NAS_SET_3GPP2_SUBSCRIPTION );
    return eRCode;
}
