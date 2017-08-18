/*
 * \ingroup cbk
 *
 * \file    qaCbkWmsMessageWaitingInd.c
 *
 * \brief   Contains UnPacking routines for the
 *          QMI_WMS_MESSAGE_WAITING_IND message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaGobiApiCbk.h"
#include "qaCbkWmsMessageWaitingInd.h"

/* Functions */

/*
 * This function unpacks the Message Waiting information content parameter
 * from the QMI indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvMessageWaitingInfoInd(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    msgWaitingInfo *lResp = (msgWaitingInfo *)pResp;
    WORD              lTemp = 0;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Check For Invalid Parameter */
    if ( NULL == lResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Check for the Buffer Undersize Error */
    lTemp = qmQmiGetResponseTLVlength((struct qmTBuffer *)pTlvData);
    if ( lTemp > sizeof(msgWaitingInfo) )
    {
        return eQCWWAN_ERR_BUFFER_SZ;
    }

    /* Copy the Transport Type response into the structure */
    eRCode = GetByte( pTlvData, &(lResp->numInstances));
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Extracting each set of parameters based on num_instances */
    for (lTemp=0; lTemp < lResp->numInstances; lTemp++)
    {
        eRCode = GetByte( pTlvData,
             &(lResp->msgWaitInfo[lTemp].msgType));
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }

        eRCode = GetByte( pTlvData,
             &(lResp->msgWaitInfo[lTemp].activeInd));
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }

        eRCode = GetByte( pTlvData,
             &(lResp->msgWaitInfo[lTemp].msgCount));
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    return eRCode;
}

/*
 * This function unpacks the Message Waiting information Indication message to
 * a user-provided response structure.
 *
 * \param   pMdmResp - Pointer to packed response from the modem.
 *
 * \param   pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkWmsMessageWaitingInfoInd(
    BYTE          *pMdmResp,
    msgWaitingInfo *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_MESSAGE_WAITING_INFO, &UnpackCbkTlvMessageWaitingInfoInd},
        { eTLV_TYPE_INVALID,     NULL } /* Important. Sentinel.
                                         * Signifies last item in map.
                                         */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_WMS_MSG_WAITING_IND );
    return eRCode;
}
