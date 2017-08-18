/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "sludefs.h"
#include "qaCbkLocGetFixCriteriaInd.h"

static ULONG accuracy;
static ULONG intermediateStatus;
static ULONG minInterval;
static LocAppProvider appProvider;

package enum eQCWWANError UnpackCbkTlvGetFixCriteriaStatus(
        BYTE *pTlvData,
        BYTE *pResp )
{
    ULONG *lResp =
        &(( QmiCbkLocGetFixCriteriaInd *)pResp)->status;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the Get Fix Criteria status TLV */
    eRCode = GetLong (pTlvData, lResp);

    return eRCode;
}

package enum eQCWWANError UnpackCbkTlvGetFixCriteriaAccuracy(
        BYTE *pTlvData,
        BYTE *pResp )
{
    QmiCbkLocGetFixCriteriaInd *lResp = ( QmiCbkLocGetFixCriteriaInd *)pResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Allocate memory to pointer */
    lResp->pAccuracy = &accuracy;
   
    /* Extract the accuracy TLV */
    eRCode = GetLong (pTlvData, lResp->pAccuracy);

    return eRCode;
}

package enum eQCWWANError UnpackCbkTlvGetFixCriteriaIntermediate(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocGetFixCriteriaInd *lResp = ( QmiCbkLocGetFixCriteriaInd *)pResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Allocate memory to pointer */
    lResp->pIntermediate = &intermediateStatus;
    
    /* Extract the intermediate status TLV */
    eRCode = GetLong (pTlvData, lResp->pIntermediate);

    return eRCode;
}

package enum eQCWWANError UnpackCbkTlvGetFixCriteriaInterval(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocGetFixCriteriaInd *lResp = ( QmiCbkLocGetFixCriteriaInd *)pResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Allocate memory to pointer */
    lResp->pMinInterval = &minInterval;
    
    /* Extract the intermediate status TLV */
    eRCode = GetLong (pTlvData, lResp->pMinInterval);

    return eRCode;
}

package enum eQCWWANError UnpackCbkTlvGetFixCriteriaAppProvider(
    BYTE *pTlvData,
    BYTE *pResp )
{
    QmiCbkLocGetFixCriteriaInd *lResponse = ( QmiCbkLocGetFixCriteriaInd *)pResp;
    BYTE index = 0;
    /* Allocate memory to pointer */
    lResponse->pAppID = &appProvider;
    LocAppProvider *lResp = lResponse->pAppID;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    
    if (NULL == lResp)
        return eRCode;
    
    /* Extract the application provider */
    eRCode = GetByte(pTlvData, &(lResp->providerLen));
    if (eRCode != eQCWWAN_ERR_NONE)
        return eRCode;
        
    index = 0;
    while (index < lResp->providerLen)
    {
        eRCode = GetByte( pTlvData, &(lResp->provider[index++]) );
        if (eRCode !=  eQCWWAN_ERR_NONE)
            return eRCode;
    }

    /* Extract the application name */
    eRCode = GetByte(pTlvData, &(lResp->nameLen));
    if (eRCode != eQCWWAN_ERR_NONE)
        return eRCode;
        
    index = 0;
    while (index < lResp->nameLen)
    {
        eRCode = GetByte( pTlvData, &(lResp->name[index++]) );
        if (eRCode !=  eQCWWAN_ERR_NONE)
            return eRCode;
    }
    
    /* Extract the application version valid */
    eRCode = GetByte(pTlvData, &(lResp->verValid));
    if (eRCode != eQCWWAN_ERR_NONE)
        return eRCode;
    
    /* Extract the application version */
    eRCode = GetByte(pTlvData, &(lResp->versionLen));
    if (eRCode != eQCWWAN_ERR_NONE)
        return eRCode;
        
    index = 0;
    while (index < lResp->versionLen)
    {
        eRCode = GetByte( pTlvData, &(lResp->version[index++]) );
        if (eRCode !=  eQCWWAN_ERR_NONE)
            return eRCode;
    }
    
    return eRCode;
}

package enum eQCWWANError UpkQmiCbkLocGetFixCriteriaInd(
        BYTE                    *pMdmResp,
        QmiCbkLocGetFixCriteriaInd   *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_GET_FIX_CRITERIA_STATUS,           &UnpackCbkTlvGetFixCriteriaStatus },
        { eTLV_GET_FIX_CRITERIA_ACCURACY,         &UnpackCbkTlvGetFixCriteriaAccuracy },
        { eTLV_GET_FIX_CRITERIA_INTERMEDIATE,     &UnpackCbkTlvGetFixCriteriaIntermediate },
        { eTLV_GET_FIX_CRITERIA_MIN_INTERVAL,     &UnpackCbkTlvGetFixCriteriaInterval },
        { eTLV_GET_FIX_CRITERIA_APP_ID,           &UnpackCbkTlvGetFixCriteriaAppProvider },
        { eTLV_TYPE_INVALID,                      NULL } /* Important. Sentinel.
                                                    * Signifies last item in map.
                                                    */
    };

    eRCode = qmunpackresp( pMdmResp,
            (BYTE *)pApiResp,
            map,
            eQMI_LOC_GET_FIX_CRITERIA );
    return eRCode;
}
