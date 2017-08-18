/*
 * \ingroup swioma
 *
 * \file    qaSwiOmaDmcancelSessionExt.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_SWIOMA_DM_CANCEL_SESSION message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaSwiOmaDmSessionCancelExt.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSOMADMCancelSession Session field to the
 * QMI message SDU
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvSessionExt( BYTE *pBuf, BYTE *pParam )
{
    struct QmiSwiOmaDmSessionCancelReqExt *pReq =
        (struct QmiSwiOmaDmSessionCancelReqExt *)pParam;

    /* Add TLV data */
    return PutByte( pBuf, pReq->session );
}

/*
 * This function packs the SLQSOMADMCancelSession parameters to the QMI message
 * SDU
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaGobiApiSwiOmadms.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiSwiOmaDmSessionCancelExt(
    WORD *pMlength,
    BYTE *pParamField,
    ULONG session )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_SESSION,      &BuildTlvSessionExt },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map.
                                     */
    };

    struct QmiSwiOmaDmSessionCancelReqExt req;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    slmemset((CHAR *)&req, 0, sizeof(struct QmiSwiOmaDmSessionCancelReqExt));
    req.session = session;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_SWIOMA_CANCEL_SESSION,
                      pMlength );
    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSOMADMCancelSessionExt response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp     [IN]  - Pointer to packed response from the modem.
 *
 * \param  pApiResp     [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiSwiOmaDmSessionCancelExt(
    BYTE                                *pMdmResp,
    struct QmiSwiOmaDmSessionCancelRespExt *pApiResp )
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
                           eQMI_SWIOMA_CANCEL_SESSION );
    return eRCode;
}
