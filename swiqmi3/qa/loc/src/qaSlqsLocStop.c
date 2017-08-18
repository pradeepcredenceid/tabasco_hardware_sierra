/*
 * \ingroup : loc
 *
 * \file    : qaSlqsLocStop.c
 *
 * \brief   : Contains Packing and UnPacking routines for the
 *            eQMI_LOC_STOP message.
 *
 * Copyright: © 2011-2015 Sierra Wireless, Inc. all rights reserved
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaSlqsLocStop.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the LOC Stop request Session Id  field
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvStopSessionId( BYTE *pBuf, BYTE *pParam )
{
    LOCStopReq *lReq =  (LOCStopReq *)pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if(lReq == NULL)
        return eQCWWAN_ERR_INVALID_ARG;

    /* Insert Session Id */
    eRCode = PutByte( pBuf, lReq->sessionId );
    return eRCode;
}
/*
 * Packs the PkQmiLocStopReq parameters to the QMI message SDU
 *
 * \param  pParamField[OUT]         - Pointer to storage into which the packed
 *                                    data will be placed by this function.
 *
 * \param  pMlength[OUT]            - Total length of built message.
 *
 * \param  pLOCStartReqReq[IN]     - LOC Start Request Parameters
 *
 * \return  eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX on error
 *
 */
enum eQCWWANError PkQmiLocStopReq(
    WORD                *pMlength,
    BYTE                *pParamField,
    LOCStopReq        *pLOCStopReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_STOP_SESSION_ID,            &BuildTlvStopSessionId },
        { eTLV_TYPE_INVALID,               NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pLOCStopReq,
                      map,
                      eQMI_LOC_STOP,
                      pMlength );

    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************

 * This function unpacks the eQMI_LOC_STOP response message
 * to a user-provided response structure.
 *
 * \param  pMdmResp  [IN]  - Pointer to packed response from the modem.
 *
 * \param  pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE   - on success;eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiLocStopResp(
                  BYTE                         *pMdmResp,
                  struct QmiLocStopResp        *pApiResp)
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
                           eQMI_LOC_STOP );
    return eRCode;
}
