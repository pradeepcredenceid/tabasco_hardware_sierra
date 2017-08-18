/*
 * \ingroup swioma
 *
 * \file    qaSwiOmaDmSelectionExt.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_SWIOMA_DM_SELECTIONEXT message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaSwiOmaDmSelectionExt.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSOMADMSendSelectionExt user selection field to the
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
enum eQCWWANError BuildTlvSelectionExt( BYTE *pBuf, BYTE *pParam )
{
    struct QmiSwiOmaDmSelectionReqExt *pReq =
        (struct QmiSwiOmaDmSelectionReqExt *)pParam;

    /* Add TLV data */
    return PutByte( pBuf, pReq->selection );
}

/*
 * This function packs the SLQSOMADMSendSelectionExt parameters to the QMI
 * message SDU
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaGobiApiSwiOmadmsExt.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiSwiOmaDmSelectionExt(
    WORD  *pMlength,
    BYTE  *pParamField,
    ULONG selection )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_USER_SELECTION_EXT, &BuildTlvSelectionExt },
        { eTLV_TYPE_INVALID,   NULL } /* Important. Sentinel.
                                       * Signifies last item in map
                                       */
    };

    struct QmiSwiOmaDmSelectionReqExt req;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    slmemset((CHAR *)&req, 0, sizeof(struct QmiSwiOmaDmSelectionReqExt));
    req.selection = selection;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_SWIOMA_SEND_SELECTION,
                      pMlength );

    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSOMADMSendSelectionExt response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp           - Pointer to packed response from the modem.
 *
 * \param  pApiResp           - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiSwiOmaDmSelectionExt(
    BYTE                               *pMdmResp,
    struct QmiSwiOmaDmSelectionRespExt *pApiResp )
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
                           eQMI_SWIOMA_SEND_SELECTION );

    return eRCode;
}
