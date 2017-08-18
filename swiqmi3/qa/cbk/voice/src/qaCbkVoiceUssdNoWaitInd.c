/*
 * \ingroup cbk
 *
 * \file    qaCbkVoiceUssdNoWaitInd.c
 *
 * \brief   Contains UnPacking routines for the QMI_VOICE_USSD_NO_WAIT_IND message.
 *
 * Copyright: © 2011 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkVoiceUssdNoWaitInd.h"
#include "qaGobiApiVoiceCommon.h"      /* for UnpackTlvCommonAlphaIdentifier */
#include "qaGobiApiCbk.h"              /* for USSD coding scheme*/

/* function prototypes */
extern swi_bool slUcs2ToUtf8(
    swi_UCS2   *ucs2stringp,
    swi_uint16 numucs2chars,
    unsigned char *utf8stringp,
    swi_uint16 maxbytes);

/* Functions */

/*
 * This function unpacks the USSD Error TLV parameter from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvUssdNoWaitError(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct VoiceUssdNoWaitErrorTlv *lResponse =
        &((struct QmiCbkVoiceUssdIndNoWait *)pResp)->NwETlv;

    BYTE lByte = 0;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the Error Type*/
    eRCode = GetByte( pTlvData, &lByte);
    if( eQCWWAN_ERR_NONE == eRCode )
    {
        lResponse->Error = lByte;

        /* Initialize the remaining structure elements */
        lResponse->TlvPresent  = TRUE;
        return eRCode;
    }

    return eRCode;
}

/*
 * This function unpacks the USSD Failure Cause TLV parameter from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvUssdNoWaitFailureCause(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct VoiceUssdNoWaitFailCauseTlv *lResponse =
        &((struct QmiCbkVoiceUssdIndNoWait *)pResp)->FCTlv;

    BYTE lByte = 0;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the failure cause */
    eRCode = GetByte( pTlvData, &lByte);
    if( eQCWWAN_ERR_NONE == eRCode )
    {
        lResponse->Failure_Cause = lByte;

        /* Initialize the remaining structure elements */
        lResponse->TlvPresent  = TRUE;
        return eRCode;
    }

    return eRCode;
}

/*
 * This function unpacks the Ussd Data from Network TLV from the QMI indication
 * message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvUssdNoWaitUssdData(
    BYTE *pTlvData,
    BYTE *pResp )
{
    BYTE              uCount = 0;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Get the required TLV structure to the local pointer */
    struct USSInfo *lResponse =
        &((struct QmiCbkVoiceUssdIndNoWait *)pResp)->UInfoTLV.NetworkInfo;

    struct VoiceUssdNoWaitUSSINFOTlv *lResp =
        &((struct QmiCbkVoiceUssdIndNoWait *)pResp)->UInfoTLV;

    /* Extract the ussDCS*/
    eRCode = GetByte( pTlvData, &(lResponse->ussDCS) );
    if( eRCode != eQCWWAN_ERR_NONE )
        return eRCode;

    /* Extract the ussLen*/
    eRCode = GetByte( pTlvData, &(lResponse->ussLen) );
    if ( eRCode != eQCWWAN_ERR_NONE )
        return eRCode;

    /* Extract the Data encoded as per the DCS*/
    /* Check if DCS is ucs2 coding scheme */
    if( USSD_DCS_UCS2 == lResponse->ussDCS )
    {
        BYTE ussData[MAXUSSDLENGTH];

        /* Extract the Data encoded as UCS2*/
        while ((uCount < lResponse->ussLen) && (eRCode == eQCWWAN_ERR_NONE))
        {
            eRCode = GetByte( pTlvData, &ussData[uCount+1] );
            eRCode = GetByte( pTlvData, &ussData[uCount] );
            uCount+=2;
        }
        /* convert ucs2 to utf-8 for Android*/
        slUcs2ToUtf8( (swi_UCS2 *)ussData, uCount/2, (unsigned char *)&lResponse->ussData, MAXUSSDLENGTH);
    }
    else /* ASCII coding scheme */
    {
        /* Extract the Data encoded as ASCII*/
        while ((uCount < lResponse->ussLen) && (eRCode == eQCWWAN_ERR_NONE))
        {
            eRCode = GetByte( pTlvData, &lResponse->ussData[uCount] );
            uCount++;
        }
    }

    /* Initilize the remaining structure elements */
    lResp->TlvPresent  = TRUE;
    return eRCode;
}

/*
 * This function unpacks the USSD Alpha identifier TLV parameter from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvUssdNoWaitAlphaIdentifier(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct VoiceUssdNoWaitAlphaIDTlv *lResponse =
        &((struct QmiCbkVoiceUssdIndNoWait *)pResp)->AIDTlv;

    alphaIDInfo * pAlphaIdInfo = &lResponse->Alpha_Identifier;

    return UnpackTlvCommonAlphaIdentifier( pTlvData, pAlphaIdInfo );
}

/*
 * This function unpacks the Event Report Indication message to a
 * user-provided response structure.
 *
 * \param   pMdmResp - Pointer to packed response from the modem.
 *
 * \param   pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkVoiceUssdIndNoWait(
    BYTE                      *pMdmResp,
    struct QmiCbkVoiceUssdIndNoWait *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_USSD_ERROR,             &UnpackCbkTlvUssdNoWaitError           },
        { eTLV_USSD_FAILURE_CAUSE,     &UnpackCbkTlvUssdNoWaitFailureCause    },
        { eTLV_USSD_DATA_FROM_NETWORK, &UnpackCbkTlvUssdNoWaitUssdData        },
        { eTLV_USSD_ALPHA_IDENTIFIER,  &UnpackCbkTlvUssdNoWaitAlphaIdentifier },
        { eTLV_TYPE_INVALID,           NULL } /* Important. Sentinel.
                                               * Signifies last item in map.
                                               */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_VOICE_ORIG_USSD_NO_WAIT_IND );
    return eRCode;
}
