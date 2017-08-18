/*
 * \ingroup uim
 *
 * \file    qaUimSLQSUimGetServiceStatus.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_UIM_GET_SERVICE_STATUS message.
 *
 * Copyright: © 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaUimSLQSUimGetServiceStatus.h"
#include "qaGobiApiUimCommon.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the Session Information parameter to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvGSSessionInfo( BYTE *pBuf, BYTE *pParam )
{
    UIMSessionInformation *pReq = &(( UIMGetServiceStatusReq *)pParam)->sessionInfo;

    if ( NULL == pReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Insert Session Information */
    return PackTlvCommonSessionInfo( pBuf, pReq );
}


/*
 * This function packs the Capability mask parameter to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvCapMask( BYTE *pBuf, BYTE *pParam )
{
    UIMGetServiceStatusReq *pReq = ( UIMGetServiceStatusReq *)pParam;

    /* Insert capability mask */
    return PutLong( pBuf, pReq->capMask );
}

/*
 * This function packs the SLQS GetServiceStatus request parameters to the QMI message SDU.
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaUimSLQSUimGetServiceStatus.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiUimSlqsGetServiceStatus(
    WORD            *pMlength,
    BYTE            *pParamField,
    UIMGetServiceStatusReq *pUIMGetServiceStatusReq )
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_GS_SESSION_INFO,   &BuildTlvGSSessionInfo },
        { eTLV_CAP_MASK,          &BuildTlvCapMask },
        { eTLV_TYPE_INVALID,      NULL } /* Important. Sentinel.
                                          * Signifies last item in map
                                          */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pUIMGetServiceStatusReq,
                      map,
                      eQMI_UIM_GET_SERVICE_STATUS,
                      pMlength );
    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the FDN status Value from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvFDNStatus(
    BYTE *pTlvData,
    BYTE *pResp )
{
    UIMGetServiceStatusResp *lResp = ((struct QmiUimSLQSGetServiceStatusResp *)
                               pResp)->pUIMGetServiceStatusResp;
    enum eQCWWANError eRCode;

    /* Check For Invalid Parameter */
    if ( NULL == lResp->pFDNStatus )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Copy the FDN status value into the List */
    return GetByte( pTlvData, lResp->pFDNStatus );
   
}

/******************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the hidden key status Value from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvHiddenKeyStatus(
    BYTE *pTlvData,
    BYTE *pResp )
{
    UIMGetServiceStatusResp *lResp = ((struct QmiUimSLQSGetServiceStatusResp *)
                               pResp)->pUIMGetServiceStatusResp;
    enum eQCWWANError eRCode;

    /* Check For Invalid Parameter */
    if ( NULL == lResp->pHiddenKeyStatus )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Copy the hidden key status value into the List */
    return GetByte( pTlvData, lResp->pHiddenKeyStatus );
   
}

/******************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the Index from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvIndex(
    BYTE *pTlvData,
    BYTE *pResp )
{
    UIMGetServiceStatusResp *lResp = ((struct QmiUimSLQSGetServiceStatusResp *)
                               pResp)->pUIMGetServiceStatusResp;
    enum eQCWWANError eRCode;

    /* Check For Invalid Parameter */
    if ( NULL == lResp->pIndex )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Copy the Index into the List */
    return GetByte( pTlvData, lResp->pIndex );
   
}

/*
 * This function unpacks the SLQS GetServiceStatus response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp           - Pointer to packed response from the modem.
 *
 * \param  pApiResp           - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiUimSlqsGetServiceStatus(
    BYTE                           *pMdmResp,
    struct QmiUimSLQSGetServiceStatusResp *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,          &qmUnpackTlvResultCode },
        { eTLV_GS_FDN_STATUS,        &UnpackTlvFDNStatus },
        { eTLV_HIDDEN_KEY_STATUS,    &UnpackTlvHiddenKeyStatus },
        { eTLV_INDEX,                &UnpackTlvIndex },
        { eTLV_TYPE_INVALID,         NULL } /* Important. Sentinel.
                                             * Signifies last item in map.
                                             */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_UIM_GET_SERVICE_STATUS );
    return eRCode;
}
