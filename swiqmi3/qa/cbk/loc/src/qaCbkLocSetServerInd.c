/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "sludefs.h"
#include "qaCbkLocSetServerInd.h"

package enum eQCWWANError UnpackCbkTlvSetServerStatus(
        BYTE *pTlvData,
        BYTE *pResp )
{
    ULONG *lResp =
        &(( QmiCbkLocSetServerInd *)pResp)->status;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = GetLong( pTlvData, lResp);

    return eRCode;
}

package enum eQCWWANError UpkQmiCbkLocSetServerInd(
        BYTE                    *pMdmResp,
        QmiCbkLocSetServerInd   *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_SET_SERVER_STATUS,           &UnpackCbkTlvSetServerStatus },
        { eTLV_TYPE_INVALID,                 NULL } /* Important. Sentinel.
                                                    * Signifies last item in map.
                                                    */
    };

    eRCode = qmunpackresp( pMdmResp,
            (BYTE *)pApiResp,
            map,
            eQMI_LOC_SET_SERVER );
    return eRCode;
}
