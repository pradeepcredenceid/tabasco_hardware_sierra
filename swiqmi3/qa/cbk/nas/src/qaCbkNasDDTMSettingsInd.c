/*
 * \ingroup nas
 *
 * \file    qaCbkNasDDTMSettingsInd.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          eQMI_NAS_DDTM_IND message.
 *
 * Copyright: © 2010-2014 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkNasDDTMSettingsInd.h"
#include "qaGobiApiNasCommon.h"
#include "qaGobiApiNas.h"

#define SERVICE_OPTION_MAX_LEN  256

/* Functions */
/*
 * This function unpacks the DDTM Settings Information from the QMI
 * indication message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UnpackTlvDDTMSettingsInd(
    BYTE *pTlvData,
    BYTE *pResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE LoopIndex;
    USHORT aServiceOption[SERVICE_OPTION_MAX_LEN];

    /* Get the required TLV structure to the local pointer */
    struct NasDDTMSettingsTlv *lResp =
            &( ( (struct QmiCbkNasDDTMSettingsInd *)pResp)->DDTMSettingsTlv );

    struct DDTMSettings *lResponse = &(lResp->ddtm_settings);

    /* Allocate memory to the pointer */
    lResponse->DDTMPref.pServiceOption = (USHORT *)aServiceOption;

    /* Extract the current DDTM status*/
    eRCode = GetByte( pTlvData, &(lResponse->CurrentDDTMStatus) );
    if( eRCode != eQCWWAN_ERR_NONE )
        return eRCode;

    /* Extract the DDTM preferences*/
    eRCode = GetByte( pTlvData, &(lResponse->DDTMPref.DDTMPrefSetting) );
    if( eRCode != eQCWWAN_ERR_NONE )
        return eRCode;

    /* Extract the DDTM action*/
    eRCode = GetWord( pTlvData, &(lResponse->DDTMPref.DDTMAction) );
    if( eRCode != eQCWWAN_ERR_NONE )
        return eRCode;

    /* Extract the DDTM action*/
    eRCode = GetByte( pTlvData, &(lResponse->DDTMPref.SOListAction) );
    if( eRCode != eQCWWAN_ERR_NONE )
        return eRCode;

    /* Extract the SOLen*/
    eRCode = GetByte( pTlvData, &(lResponse->DDTMPref.SOLen) );
    if( eRCode != eQCWWAN_ERR_NONE )
        return eRCode;

    /* Extract the DDTM action*/
    for (LoopIndex=0; LoopIndex < lResponse->DDTMPref.SOLen; LoopIndex++)
    {
        eRCode = GetWord( pTlvData, &(lResponse->DDTMPref.pServiceOption[LoopIndex]) );
        if( eRCode != eQCWWAN_ERR_NONE )
            return eRCode;
    }

    return eRCode;
}

/*
 * This function unpacks the DDTM settings indication message to a
 * user-provided response structure.
 *
 * \param     pMdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT]  - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiCbkNasDDTMSettingsInd(
    BYTE       *pMdmResp,
    struct QmiCbkNasDDTMSettingsInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_IND_DDTM_SETTINGS, &UnpackTlvDDTMSettingsInd },
        { eTLV_TYPE_INVALID,      NULL }  /* Important. Sentinel.
                                           * Signifies last item in map.
                                           */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_NAS_DDTM_IND );
    return eRCode;
}
