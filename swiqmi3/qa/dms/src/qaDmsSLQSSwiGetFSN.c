/*
 * \ingroup dms
 *
 * \file    qaDmsSLQSSwiGetFSN.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          eQMI_DMS_SWI_GET_FSN message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaGobiApiDms.h"
#include "qaDmsSLQSSwiGetFSN.h"

/*****************************************************************************
 * Request handling
 *****************************************************************************/
/*
 * This function packs the SLQSSwiGetFSN request parameters to
 * the QMI message SDU
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed data
 *                              will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError PkQmiDmsSLQSSwiGetFSN(
    WORD *pMlength,
    BYTE *pParamField )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID, NULL }/* Important. Sentinel.
                                    * Signifies last item in map
                                    */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode =  qmbuild( pParamField,
                       NULL,
                       map,
                       eQMI_DMS_GET_FSN,
                       pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 *****************************************************************************/
/*
 * Unpacks the FS Number from the QMI response message to a user
 * provided response structure
 *
 * \param       pTlvData [IN/OUT] - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]    - Pointer to structure containing storage
 *                                  to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvFSN( BYTE *pTlvData, BYTE *pResp )
{
    FactorySequenceNumber *lResp =
        ( ( struct QmiDmsSLQSSwiGetFSNResp * )pResp )->pFSNumber;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( NULL == lResp )
        return eRCode;

    eRCode = qmQmiExtractString(pTlvData,(CHAR *)lResp->FSNumber,
                                MAX_FSN_LENGTH);
    return eRCode;
}

/*
 * This function unpacks the SLQSSwiGetFSN response to a user-provided
 * response structure.
 *
 * \param  pMdmResp           - Pointer to packed response from the modem.
 *
 * \param  pApiResp           - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiDmsSLQSSwiGetFSN(
    BYTE                           *pMdmResp,
    struct QmiDmsSLQSSwiGetFSNResp *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,  &qmUnpackTlvResultCode },
        { eTLV_DMS_DEV_FSN,  &UnpackTlvFSN },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map.
                                     */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_DMS_GET_FSN );
    return eRCode;
}
