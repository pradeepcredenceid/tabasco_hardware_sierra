/*
 * \ingroup uim
 *
 * \file    qaUimSLQSUimSetServiceStatus.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_UIM_SET_SERVICE_STATUS message.
 *
 * Copyright: © 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaUimSLQSUimSetServiceStatus.h"
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
enum eQCWWANError BuildTlvSSSessionInfo( BYTE *pBuf, BYTE *pParam )
{
    UIMSessionInformation *pReq = &(( UIMSetServiceStatusReq *)pParam)->sessionInfo;

    if ( NULL == pReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Insert Session Information */
    return PackTlvCommonSessionInfo( pBuf, pReq );
}


/*
 * This function packs the FDN status parameter to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvFDNStatus( BYTE *pBuf, BYTE *pParam )
{
    UIMSetServiceStatusReq *pReq = ( UIMSetServiceStatusReq *)pParam;

    if ( NULL == pReq->pFDNStatus )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert FDN status */
    return PutByte( pBuf, *pReq->pFDNStatus );
}

/*
 * This function packs the SLQS SetServiceStatus request parameters to the QMI message SDU.
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaUimSLQSUimSetServiceStatus.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiUimSlqsSetServiceStatus(
    WORD            *pMlength,
    BYTE            *pParamField,
    UIMSetServiceStatusReq *pUIMSetServiceStatusReq )
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_SS_SESSION_INFO,   &BuildTlvSSSessionInfo },
        { eTLV_SS_FDN_STATUS,     &BuildTlvFDNStatus },
        { eTLV_TYPE_INVALID,      NULL } /* Important. Sentinel.
                                          * Signifies last item in map
                                          */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pUIMSetServiceStatusReq,
                      map,
                      eQMI_UIM_SET_SERVICE_STATUS,
                      pMlength );
    return eRCode;
}

/*
 * This function unpacks the SLQS SetServiceStatus response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp           - Pointer to packed response from the modem.
 *
 * \param  pApiResp           - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiUimSlqsSetServiceStatus(
    BYTE                           *pMdmResp,
    struct QmiUimSLQSSetServiceStatusResp *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,  &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                               * Signifies last item in map.
                                               */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_UIM_SET_SERVICE_STATUS );
    return eRCode;
}
