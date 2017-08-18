/*
 * \ingroup cbk
 *
 * \file    qaCbkQosNetworkStatusInd.c
 *
 * \brief   Contains UnPacking routines for the
 *          QMI_QOS_NETWORK_STATUS_IND message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkQosNetworkStatusInd.h"

/* Functions */

/*
 * This function unpacks the Packet Service Status TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvQosNetworkStatus(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiCbkQosNetworkStatusInd *lResp =
            (struct QmiCbkQosNetworkStatusInd *)pResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE lByte = 0;

    eRCode = GetByte ( pTlvData, &lByte );
    if ( eQCWWAN_ERR_NONE == eRCode )
        lResp->Status= lByte;

    return eRCode;
}

/*
 * This function unpacks the Network Qos support Status Indication to a
 * user-provided response structure.
 *
 * \param   pMdmResp - Pointer to packed response from the modem.
 *
 * \param   pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkQosNetworkStatusInd(
    BYTE    *pMdmResp,
    struct QmiCbkQosNetworkStatusInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_NETWORK_QOS_SUPPORT_STATUS,    &UnpackCbkTlvQosNetworkStatus },
        { eTLV_TYPE_INVALID,             NULL }  /* Important. Sentinel.
                                                  * Signifies last item in map.
                                                  */
    };

    eRCode = qmunpackresp( pMdmResp,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_QOS_NETWORK_STATUS_IND );
    return eRCode;
}
