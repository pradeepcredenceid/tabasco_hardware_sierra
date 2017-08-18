/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "sludefs.h"
#include "qaCbkLocGetOpModeInd.h"

static ULONG mode;

package enum eQCWWANError UnpackCbkTlvGetOpModeStatus(
        BYTE *pTlvData,
        BYTE *pResp )
{
    ULONG *lResp =
        &(( QmiCbkLocGetOpModeInd *)pResp)->status;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the Operation Mode status TLV */
    eRCode = GetLong (pTlvData, lResp);

    return eRCode;
}

package enum eQCWWANError UnpackCbkTlvGetOpModeCurMode(
        BYTE *pTlvData,
        BYTE *pResp )
{
    QmiCbkLocGetOpModeInd *lResp = ( QmiCbkLocGetOpModeInd *)pResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    /* Allocate memory to pointer */
    lResp->pMode = &mode;
    
    /* Extract the current Operation Mode TLV */
    eRCode = GetLong (pTlvData, lResp->pMode);
    
    return eRCode;
}

package enum eQCWWANError UpkQmiCbkLocGetOpModeInd(
        BYTE                    *pMdmResp,
        QmiCbkLocGetOpModeInd   *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_GET_OP_MODE_STATUS,     &UnpackCbkTlvGetOpModeStatus },
        { eTLV_GET_OP_MODE_CURMODE,    &UnpackCbkTlvGetOpModeCurMode },
        { eTLV_TYPE_INVALID,                NULL } /* Important. Sentinel.
                                                    * Signifies last item in map.
                                                    */
    };

    eRCode = qmunpackresp( pMdmResp,
            (BYTE *)pApiResp,
            map,
            eQMI_LOC_GET_OPERATION_MODE );
    return eRCode;
}
