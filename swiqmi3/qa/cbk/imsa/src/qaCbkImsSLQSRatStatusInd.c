/*
 * \ingroup cbk
 *
 * \file    qaCbkImsSLQSRatStatusInd.c
 *
 * \brief   Contains UnPacking routines for the eQMI_IMSA_RAT_HANDOVER_STATUS_IND message.
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkImsaSLQSRatStatusInd.h"

/* Functions */

/*
 * This function unpacks the RAT Handover Status TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvRatHandoverStatus(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
     struct RatHandoverStatusTlv *lResp =
        &((struct QmiCbkImsaRatStatusInd *)pResp)->RatHandover;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE i;

    /* Extract the RAT Handover Status */
    eRCode = GetLong( pTlvData, &lResp->RatHandoverStatus );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
    eRCode = GetLong( pTlvData, &lResp->SourceRAT );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
    eRCode = GetLong( pTlvData, &lResp->TargetRAT );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    eRCode = GetByte( pTlvData, &lResp->ErrorCodeLen );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    if (lResp->ErrorCodeLen) 
    {
        for (i = 0; i < lResp->ErrorCodeLen; i++) 
        {
            eRCode = GetByte( pTlvData, &lResp->ErrorCodeData[i] );
            if( eRCode != eQCWWAN_ERR_NONE )
            {
                return eRCode;
            }
        }
    lResp->ErrorCodeData[i] = 0;
    }   
    lResp->TlvPresent   = TRUE;
    return eRCode;
}
/*
 * This function unpacks the RAT Handover Indication message to a
 * user-provided response structure.
 *
 * \param   pMdmResp - Pointer to packed response from the modem.
 *
 * \param   pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkImsaRatStatusInd(
    BYTE                            *pMdmResp,
    struct QmiCbkImsaRatStatusInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RAT_HANDOVER_STATUS, &UnpackCbkTlvRatHandoverStatus },
        { eTLV_TYPE_INVALID,   NULL } /* Important. Sentinel.
                                       * Signifies last item in map.
                                       */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_IMSA_RAT_HANDOVER_STATUS_IND );
    return eRCode;
}
