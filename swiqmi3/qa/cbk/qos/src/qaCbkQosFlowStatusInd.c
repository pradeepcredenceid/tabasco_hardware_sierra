/*
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkQosFlowStatusInd.h"

static enum eQCWWANError uStatus(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    struct QmiCbkQosFlowStatusInd *lResp =
        (struct QmiCbkQosFlowStatusInd *)pResp;

    eRCode = GetLong( pTlvData, &lResp->id);
    if ( eQCWWAN_ERR_NONE != eRCode)
        return eRCode;

    eRCode = GetByte( pTlvData, &lResp->status);
    if ( eQCWWAN_ERR_NONE != eRCode)
        return eRCode;

    eRCode = GetByte( pTlvData, &lResp->event);
    if ( eQCWWAN_ERR_NONE != eRCode)
        return eRCode;

    return eRCode;
}

static enum eQCWWANError uReason(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    struct QmiCbkQosFlowStatusInd *lResp =
        (struct QmiCbkQosFlowStatusInd *)pResp;

    eRCode = GetByte( pTlvData, &lResp->reason);
    if ( eQCWWAN_ERR_NONE != eRCode)
        return eRCode;

    return eRCode;
}

package enum eQCWWANError UpkQmiCbkQosFlowStatusInd(
    BYTE    *pMdmResp,
    struct QmiCbkQosFlowStatusInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_STAT_INFO,    &uStatus },
        { eTLV_REASON_CODE,    &uReason },
        { eTLV_TYPE_INVALID,             NULL }  /* Important. Sentinel.
                                                  * Signifies last item in map.
                                                  */
    };

    eRCode = qmunpackresp( pMdmResp,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_QOS_FLOW_STATUS_IND );
    return eRCode;
}


