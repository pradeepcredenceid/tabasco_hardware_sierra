/*
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkQosPriEventInd.h"

static enum eQCWWANError uEvent(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    struct QmiCbkQosPriEventInd *lResp =
        (struct QmiCbkQosPriEventInd *)pResp;

    eRCode = GetWord( pTlvData, &lResp->event);
    if ( eQCWWAN_ERR_NONE != eRCode)
        return eRCode;

    return eRCode;
}

package enum eQCWWANError UpkQmiCbkQosPriEventInd(
    BYTE    *pMdmResp,
    struct QmiCbkQosPriEventInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_EVENT,    &uEvent },
        { eTLV_TYPE_INVALID,             NULL }  /* Important. Sentinel.
                                                  * Signifies last item in map.
                                                  */
    };

    eRCode = qmunpackresp( pMdmResp,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_QOS_PRIMARY_QOS_EVENT_IND );
    return eRCode;
}


