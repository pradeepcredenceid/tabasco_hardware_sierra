/*
 * \ingroup pds
 *
 * \file qaPdsStartTrackingSession.c
 *
 * \brief Contains Packing and UnPacking routines for the
 *        eQMI_PDS_DETERMINE_POSITION message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaPdsDeterminePosition.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSPDSDeterminePosition parameters
 * to the QMI message SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                                  data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE   - Increment successful
 * \return eQCWWAN_ERR_MEMORY - Access beyond allowed size attempted
 *
 * \sa     qaGobiApiPds.h for remaining parameter descriptions.
 *
 */
enum eQCWWANError PkQmiPdsDeterminePosition(
    WORD  *pMlength,
    BYTE  *pParamField )
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map
                                     */
    };

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      NULL,
                      map,
                      eQMI_PDS_FIX_POSITION,
                      pMlength );
    return eRCode;
}


/******************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSPDSDeterminePosition response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp - Pointer to packed response from the modem.
 *
 * \param  pApiResp - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE   - Increment successful
 * \return eQCWWAN_ERR_MEMORY - Access beyond allowed size attempted
 *
 */
enum eQCWWANError UpkQmiPdsDeterminePosition(
    BYTE                               *pMdmResp,
    struct QmiPdsDeterminePositionResp *pApiResp )
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
                           eQMI_PDS_FIX_POSITION );
    return eRCode;
}
