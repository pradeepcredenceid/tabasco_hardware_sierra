/*
 * \ingroup sms
 *
 * \file    qaSmsSLQSGetMessageWaiting.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_WMS_GET_MESSAGE_WAITING message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaSmsSLQSGetMessageWaiting.h"

/*****************************************************************************
 * Request handling
 *****************************************************************************/

/*
 * This function packs the SLQS Get Message Waiting Info request parameters to
 * the QMI message SDU
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError PkQmiSmsSLQSGetMessageWaiting(
    WORD *pMlength,
    BYTE *pParamField )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID,    NULL } /* Important. Sentinel.
                                        * Signifies last item in map
                                        */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      NULL,
                      map,
                      eQMI_WMS_GET_MSG_WAITING,
                      pMlength );
    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the Message Waiting info from the
 * QMI response message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvMsgWaitingInfo(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiSmsSLQSGetMessageWaitingResp *lResp =
        (struct QmiSmsSLQSGetMessageWaitingResp *)pResp;

    enum eQCWWANError eRCode;
    WORD              lTemp = 0;

    /* Check For Invalid Parameter */
    if ( NULL == lResp->pGetMsgWaitingResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Check for the Buffer Undersize Error */
    lTemp = qmQmiGetResponseTLVlength((struct qmTBuffer *)pTlvData);
    if ( lTemp > sizeof(getMsgWaitingInfo) )
    {
        return eQCWWAN_ERR_BUFFER_SZ;
    }

    /* Copy the Transport Type response into the structure */
    eRCode = GetByte( pTlvData, &(lResp->pGetMsgWaitingResp->numInstances));
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Extracting each set of parameters based on num_instances */
    for (lTemp=0; lTemp < lResp->pGetMsgWaitingResp->numInstances; lTemp++)
    {
        eRCode = GetByte( pTlvData,
             &(lResp->pGetMsgWaitingResp->msgWaitInfo[lTemp].msgType));
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }

        eRCode = GetByte( pTlvData,
             &(lResp->pGetMsgWaitingResp->msgWaitInfo[lTemp].activeInd));
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }

        eRCode = GetByte( pTlvData,
             &(lResp->pGetMsgWaitingResp->msgWaitInfo[lTemp].msgCount));
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    return eRCode;
}

/*
 * This function unpacks the SLQS Get Message Waiting Info response to
 * a user-provided response structure.
 *
 * \param  pMdmResp           - Pointer to packed response from the modem.
 *
 * \param  pApiResp           - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiSmsSLQSGetMessageWaiting(
    BYTE                                   *pMdmResp,
    struct QmiSmsSLQSGetMessageWaitingResp *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,          &qmUnpackTlvResultCode },
        { eTLV_MESSAGE_WAITING_INFO,     &UnpackTlvMsgWaitingInfo },
        { eTLV_TYPE_INVALID,         NULL } /* Important. Sentinel.
                                             * Signifies last item in map.
                                             */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_WMS_GET_MSG_WAITING );
    return eRCode;
}
