/*
 * \ingroup cbk
 *
 * \file    qaCbkImsSLQSRegStatusInd.c
 *
 * \brief   Contains UnPacking routines for the eQMI_IMSA_REGISTRATION_STATUS_IND message.
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkImsaSLQSRegStatusInd.h"

/* Functions */

/*
 * This function unpacks the IMSRegStatus TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvImsRegStatus(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct IMSRegStatusTlv *lResp =
        &((struct QmiCbkImsaRegStatusInd *)pResp)->IMSRegistration;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the IMS Registration Status */
    eRCode = GetByte( pTlvData, &lResp->ImsRegistered );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
    
    lResp->TlvPresent   = TRUE;
    return eRCode;
}

/*
 * This function unpacks the IMSRegStatusErrorCode TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvImsRegStatusErrorCode(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct IMSRegStatusErrorCodeTlv *lResp =
        &((struct QmiCbkImsaRegStatusInd *)pResp)->IMSRegistrationError;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the IMS Registration Status Errpr Code */
    eRCode = GetWord( pTlvData, &lResp->ErrorCode );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
    
    lResp->TlvPresent   = TRUE;
    return eRCode;
}

/*
 * This function unpacks the NewImsRegStatus TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvNewImsRegStatus(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct NewIMSRegStatusTlv *lResp =
        &((struct QmiCbkImsaRegStatusInd *)pResp)->NewIMSRegistration;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;    

    /* Extract the New IMS Registration Status */
    eRCode = GetLong( pTlvData, &lResp->ImsRegStatus );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
    
    lResp->TlvPresent   = TRUE;
    return eRCode;
}
/*
 * This function unpacks the IMS Registration Status Indication message to a
 * user-provided response structure.
 *
 * \param   pMdmResp - Pointer to packed response from the modem.
 *
 * \param   pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkImsaRegStatusInd(
    BYTE                            *pMdmResp,
    struct QmiCbkImsaRegStatusInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_IMS_REG_STATUS, &UnpackCbkTlvImsRegStatus },
        { eTLV_IMS_REG_STATUS_ERROR_CODE, &UnpackCbkTlvImsRegStatusErrorCode },
        { eTLV_NEW_IMS_REG_STATUS,   &UnpackCbkTlvNewImsRegStatus },
        { eTLV_TYPE_INVALID,   NULL } /* Important. Sentinel.
                                       * Signifies last item in map.
                                       */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_IMSA_REGISTRATION_STATUS_IND );
    return eRCode;
}
