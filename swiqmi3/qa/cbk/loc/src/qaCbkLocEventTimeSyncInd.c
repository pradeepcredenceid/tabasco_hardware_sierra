/*
 * \ingroup cbk
 *
 * \file qaCbkLocEventTimeSyncInd.c
 *
 * \brief Contains UnPacking routines for the
 *        QMI_LOC_EVENT_TIME_SYNC_REQ_IND message.
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "sludefs.h"
#include "qaCbkLocEventTimeSyncInd.h"

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
package enum eQCWWANError UnpackCbkTlvRefCounter(
    BYTE *pTlvData,
    BYTE *pResp )
{
    ULONG *lResp =
        &(( QmiCbkLocEventTimeSyncInd *)pResp)->timeSyncRefCounter;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the parameter from Event Time Sync TLV */
    eRCode = GetLong (pTlvData, lResp);

    return eRCode;
}

/*
 * This function unpacks the Event Report Indication message to a
 * user-provided response structure.
 *
 * \param  pMdmResp - Pointer to packed response from the modem.
 *
 * \param  pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkLocEventTimeSyncInd(
        BYTE                                 *pMdmResp,
        QmiCbkLocEventTimeSyncInd            *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_IND_REF_COUNTER,      &UnpackCbkTlvRefCounter },
        { eTLV_TYPE_INVALID,         NULL } /* Important. Sentinel.
                                             * Signifies last item in map.
                                             */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_LOC_EVENT_TIME_SYNC_REQ_IND );
    return eRCode;
}
