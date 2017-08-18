/*
 * \ingroup : loc
 *
 * \file    : qaSlqsLocSetCradleMountConfig.c
 *
 * \brief   : Contains Packing and UnPacking routines for the
 *            eQMI_LOC_SET_CRADLE_MOUNT_CONFIG message.
 *
 * Copyright: © 2011-2016 Sierra Wireless, Inc. all rights reserved
 */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaSlqsLocSetCradleMountConfig.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the Cradle Mount state field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvCradleMountState( BYTE *pBuf, BYTE *pParam )
{
	LocSetCradleMountReq *pReq =
        (LocSetCradleMountReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = PutLong( pBuf, pReq->state);
    return eRCode;
}

/*
 * This function packs the Cradle Mount Confidence field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvCradleMountConfidence( BYTE *pBuf, BYTE *pParam )
{
	LocSetCradleMountReq *pReq =
        (LocSetCradleMountReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pConfidence == NULL )
        return eQCWWAN_ERR_NONE;

    eRCode = PutByte( pBuf, *pReq->pConfidence);
    return eRCode;
}

/*
 * Packs the PkQmiLocSetCradleMountConfig parameters to the QMI message SDU
 *
 * \param  pParamField[OUT]          - Pointer to storage into which the packed
 *                                     data will be placed by this function.
 *
 * \param  pMlength[OUT]             - Total length of built message.
 *
 * \param  pLocSetCradleMountReq[IN] - Set Cradle Mount Req
 *
 * \return  eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX on error
 *
 */

enum eQCWWANError PkQmiLocSetCradleMountConfig(
    WORD                    *pMlength,
    BYTE                    *pParamField,
    LocSetCradleMountReq    *pLocSetCradleMountReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_CRADLE_MOUNT_STATE,              &BuildTlvCradleMountState },
        { eTLV_CRADLE_MOUNT_CONFIDENCE,         &BuildTlvCradleMountConfidence },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map
                                     */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pLocSetCradleMountReq,
                      map,
                      eQMI_LOC_SET_CRADLE_MOUNT_CONFIG,
                      pMlength );
    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************

 * This function unpacks the eQMI_LOC_SET_CRADLE_MOUNT_CONFIG response message
 * to a user-provided response structure.
 *
 * \param  pMdmResp  [IN]  - Pointer to packed response from the modem.
 *
 * \param  pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE  - on success;  eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiLocSetCradleMountConfig(
                  BYTE                                    *pMdmResp,
                  struct QmiLocSetCradleMountConfigResp   *pApiResp)
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
                           (BYTE *)pApiResp,
                           map,
                           eQMI_LOC_SET_CRADLE_MOUNT_CONFIG);
    return eRCode;
}
