/*
 * \ingroup loc
 *
 * \file    qaSlqsLocEventRegister.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_LOC_REG_EVENTS message.
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaSlqsLocEventRegister.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the Event mask field to the QMI message SDU
 *
 * \param  pBuf         [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \param  pParam       [IN]  - Pointer to structure containing data for
 *                              this TLV.
 *
 * \return eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvLocEventMask( BYTE *pBuf, BYTE *pParam )
{
    LOCEventRegisterReqResp *pReq = (LOCEventRegisterReqResp *)pParam;

    /* Add TLV data */
    return Put64( pBuf, pReq->eventRegister );
}

/*
 * This function packs the SLQS Event Register request parameters to the
 * QMI message SDU.
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaSlqsLocEventRegister.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiLocSlqsEventRegister(
    WORD                    *pMlength,
    BYTE                    *pParamField,
    LOCEventRegisterReqResp *pLOCEventRegisterReqResp )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_LOC_EVENT_MASK,   &BuildTlvLocEventMask },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map
                                     */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pLOCEventRegisterReqResp,
                      map,
                      eQMI_LOC_REG_EVENTS,
                      pMlength );
    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the EventMask from the  QMI response message to a user
 * provided response structure
 *
 * \param  pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param  pResp    [OUT]     - Pointer to structure containing storage
 *                              to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success; eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiLocSlqsEventRegister(
    BYTE                               *pMdmResp,
    struct QmiLocSlqsEventRegisterResp *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,     &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID,     NULL } /* Important. Sentinel.
                                         * Signifies last item in map.
                                         */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_LOC_REG_EVENTS );
    return eRCode;
}
