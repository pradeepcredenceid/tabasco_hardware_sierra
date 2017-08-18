/*
 * \ingroup cbk
 *
 * \file    qaCbkImsSLQSSMSCfgInd.c
 *
 * \brief   Contains UnPacking routines for the QMI_IMS_SMS_CONFIG_IND message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkImsSLQSSMSCfgInd.h"

/* Locals */

/* Functions */

/*
 * This function unpacks the SMS Format TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSMSFmt(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct SMSFmtTlv *lResp =
        &((struct QmiCbkImsSMSConfigInd *)pResp)->SFTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lTemp  = 0;

    /* Extract the SMS Format */
    eRCode = GetByte( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->smsFormat  = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the SMS Over IP Network Indication Flag TLV parameters
 * from the QMI indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSMSoIPNWIndFlag(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct SMSoIPNwTlv *lResp =
        &((struct QmiCbkImsSMSConfigInd *)pResp)->SINTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lTemp  = 0;

    /* Extract the SMS over IP NW Indication Flag */
    eRCode = GetByte( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->smsoIPNW  = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the Phone Context URI TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvPhCtxtURI(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct PhCtxtURITlv *lResp =
        &((struct QmiCbkImsSMSConfigInd *)pResp)->PCURTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the Phone Context URI */
    eRCode = qmQmiExtractString( pTlvData,
                                 (CHAR *)lResp->PhCtxtURI,
                                 (ULONG)MAX_PHONE_CTXT_URI_LEN );

    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->TlvPresent = TRUE;
    return eRCode;
}

/*
 * This function unpacks the SMS Config Info Indication message to a
 * user-provided response structure.
 *
 * \param   pMdmResp - Pointer to packed response from the modem.
 *
 * \param   pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkImsSMSCfgInd(
    BYTE                         *pMdmResp,
    struct QmiCbkImsSMSConfigInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_SMS_FOMT,           &UnpackCbkTlvSMSFmt },
        { eTLV_SMS_OVER_IP_NW_IND, &UnpackCbkTlvSMSoIPNWIndFlag },
        { eTLV_PHN_CTXT_URI,       &UnpackCbkTlvPhCtxtURI },
        { eTLV_TYPE_INVALID,       NULL } /* Important. Sentinel.
                                           * Signifies last item in map.
                                           */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_IMS_SMS_CONFIG_IND );
    return eRCode;
}
