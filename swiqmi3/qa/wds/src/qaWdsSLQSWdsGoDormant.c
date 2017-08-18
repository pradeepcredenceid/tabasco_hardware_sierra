/*
 * \ingroup wds
 *
 * \file qaWdsSLQSWdsGoDormant.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_WDS_GO_DORMANT message.
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 *
 */
/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"

#include "qaWdsSLQSWdsGoDormant.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the GoDormant parameters
 * to the QMI message SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success
 *
 */
enum eQCWWANError PkQmiWdsGoDormant(
    WORD    *pMlength,
    BYTE    *pParamField)
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID, NULL }  /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };

    enum eQCWWANError eRCode;
    eRCode = qmbuild(pParamField,
                     NULL,
                     map,
                     eQMI_WDS_G0_DORMANT,
                     pMlength);
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the GoDormant response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp - Pointer to packed response from the modem.
 *
 * \param  pApiResp - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success
 *
 */
enum eQCWWANError UpkQmiWdsGoDormant(
    BYTE   *pMdmResp,
    struct QmiWdsGoDormantResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,     &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID,    NULL }  /* Important. Sentinel.
                                         * Signifies last item in map.
                                         */
    };

    eRCode = qmunpackresp( pMdmResp,
                          (BYTE *)pApiResp,
                          map,
                          eQMI_WDS_G0_DORMANT );
    return eRCode;
}

