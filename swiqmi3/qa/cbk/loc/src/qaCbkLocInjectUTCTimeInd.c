/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "sludefs.h"
#include "qaCbkLocInjectUTCTimeInd.h"

package enum eQCWWANError UnpackCbkTlvInjectUTCTime(
        BYTE *pTlvData,
        BYTE *pResp )
{
    QmiCbkLocInjectUTCTimeInd *pStatus;
    pStatus = (QmiCbkLocInjectUTCTimeInd *) pResp;


    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = GetLong( pTlvData, &(pStatus->status));

    return eRCode;
}

package enum eQCWWANError UpkQmiCbkLocInjectUTCTimeInd(
        BYTE                  *pMdmResp,
        QmiCbkLocInjectUTCTimeInd   *pStatus)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_INJECT_UTC_TIME_STATUS,     &UnpackCbkTlvInjectUTCTime },
        { eTLV_TYPE_INVALID,                NULL } /* Important. Sentinel.
                                                    * Signifies last item in map.
                                                    */
    };

    eRCode = qmunpackresp( pMdmResp,
            (BYTE *)pStatus,
            map,
            eQMI_LOC_INJECT_UTC_TIME );
    return eRCode;
}
