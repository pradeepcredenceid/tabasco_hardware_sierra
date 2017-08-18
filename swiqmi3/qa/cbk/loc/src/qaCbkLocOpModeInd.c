/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "sludefs.h"
#include "qaCbkLocOpModeInd.h"

package enum eQCWWANError UnpackCbkTlvOpMode(
        BYTE *pTlvData,
        BYTE *pResp )
{
    ULONG *mode = (ULONG*) pResp;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = GetLong( pTlvData, mode );

    return eRCode;
}

package enum eQCWWANError UpkQmiCbkLocOpModeInd(
        BYTE                              *pMdmResp,
        ULONG   *mode)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_OP_MODE,       &UnpackCbkTlvOpMode },
        { eTLV_TYPE_INVALID,                NULL } /* Important. Sentinel.
                                                    * Signifies last item in map.
                                                    */
    };

    eRCode = qmunpackresp( pMdmResp,
            (BYTE *)mode,
            map,
            eQMI_LOC_SET_OPERATION_MODE );
    return eRCode;
}
