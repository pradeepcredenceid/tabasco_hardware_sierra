/*
 * \ingroup cbk
 *
 * \file qaCbkLocInjectTimeInd.c
 *
 * \brief Contains UnPacking routines for the
 *        QMI_LOC_INJECT_TIME_SYNC_DATA_IND message.
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "sludefs.h"
#include "qaCbkLocInjectTimeInd.h"

/*
 * This function unpacks the Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvInjectTimeStatus(
    BYTE *pTlvData,
    BYTE *pResp )
{
    ULONG *lResp =
        &(( QmiCbkLocInjectTimeInd *)pResp)->injectTimeSyncStatus;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Inject Time Sync TLV */
    eRCode = GetLong (pTlvData, lResp);

    return eRCode;
}

/*
 * This function unpacks the Inject Time Sync Data Indication message to a
 * user-provided response structure.
 *
 * \param  pMdmResp - Pointer to packed response from the modem.
 *
 * \param  pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkLocInjectTimeInd(
        BYTE                              *pMdmResp,
        QmiCbkLocInjectTimeInd            *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_IND_INJECT_TIME_STATUS,      &UnpackCbkTlvInjectTimeStatus },
        { eTLV_TYPE_INVALID,                NULL } /* Important. Sentinel.
                                                    * Signifies last item in map.
                                                    */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_LOC_INJECT_TIME_SYNC_DATA_IND );
    return eRCode;
}
