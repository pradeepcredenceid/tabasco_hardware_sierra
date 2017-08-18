/*
 * \ingroup cbk
 *
 * \file    qaCbkImsSLQSUserCfgInd.c
 *
 * \brief   Contains UnPacking routines for the QMI_IMS_USER_CONFIG_IND message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkImsSLQSUserCfgInd.h"

/* Locals */

/* Functions */

/*
 * This function unpacks the IMS Domain Name TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvIMSDomain(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct IMSDomainTlv *lResp =
        &((struct QmiCbkImsUserConfigInd *)pResp)->IDTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the IMS Domain Name */
    eRCode = qmQmiExtractString( pTlvData,
                                 (CHAR *)lResp->imsDomainName,
                                 (ULONG)MAX_IMS_DOMAIN_NAME_LEN );

    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->TlvPresent = TRUE;
    return eRCode;
}

/*
 * This function unpacks the User Config Info Indication message to a
 * user-provided response structure.
 *
 * \param   pMdmResp - Pointer to packed response from the modem.
 *
 * \param   pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkImsUserCfgInd(
    BYTE                          *pMdmResp,
    struct QmiCbkImsUserConfigInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_IMS_DMIN,     &UnpackCbkTlvIMSDomain },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map.
                                     */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_IMS_USER_CONFIG_IND );
    return eRCode;
}
