/*
 * \ingroup loc
 *
 * \file    qaSlqsLocExtPowerState.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_LOC_Extermal_Power_State message.
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaSlqsLocExtPowerState.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the External Power State field to the QMI message SDU
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
enum eQCWWANError BuildTlvLocExtPowerState( BYTE *pBuf, BYTE *pParam )
{
	LOCExtPowerStateReqResp *pReq = (LOCExtPowerStateReqResp *)pParam;

    /* Add TLV data */
    return PutLong( pBuf, pReq->extPowerState );
}

/*
 * This function packs the SLQS External Power State Config request parameters to the
 * QMI message SDU.
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaSlqsLocExtPowerState.h for remaining parameter descriptions.
 */

enum eQCWWANError PkQmiLocSlqsExtPowerState(
    WORD                    *pMlength,
    BYTE                    *pParamField,
    LOCExtPowerStateReqResp *pLOCExtPowerStateReqResp )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_LOC_EXT_POWER_STATE,   &BuildTlvLocExtPowerState },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map
                                     */
    };
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pLOCExtPowerStateReqResp,
                      map,
                      eQMI_LOC_SET_EXTERNAL_POWER_CONFIG,
                      pMlength );
    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the External Power State from the  QMI response message to a user
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
enum eQCWWANError UpkQmiLocSlqsExtPowerState(
    BYTE                               *pMdmResp,
    struct QmiLocSlqsExtPowerStateResp *pApiResp )
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
                           eQMI_LOC_SET_EXTERNAL_POWER_CONFIG);
    return eRCode;
}
