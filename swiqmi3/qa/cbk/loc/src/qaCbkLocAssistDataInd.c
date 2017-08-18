/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "sludefs.h"
#include "qaCbkLocAssistDataInd.h"

package enum eQCWWANError UnpackCbkTlvAssistData(
        BYTE *pTlvData,
        BYTE *pResp )
{
    delAssistDataStatus *pStatus;
    pStatus = (delAssistDataStatus *) pResp;


    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = GetLong( pTlvData, &(pStatus->status));

    return eRCode;
}

package enum eQCWWANError UpkQmiCbkLocAssistDataInd(
        BYTE                              *pMdmResp,
        delAssistDataStatus   *pStatus)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_ASSIST_DATA_STATUS,       &UnpackCbkTlvAssistData },
        { eTLV_TYPE_INVALID,                NULL } /* Important. Sentinel.
                                                    * Signifies last item in map.
                                                    */
    };

    eRCode = qmunpackresp( pMdmResp,
            (BYTE *)pStatus,
            map,
            eQMI_LOC_DELETE_ASSIST_DATA );
    return eRCode;
}
