/*
 * \ingroup cbk
 *
 * \file    qaCbkImsSLQSSvcStatusInd.c
 *
 * \brief   Contains UnPacking routines for the eQMI_IMSA_SERVICE_STATUS_IND message.
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkImsaSLQSSvcStatusInd.h"

/* Functions */

/*
 * This function unpacks the SmsSvcStatus TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSmsSvcStatus(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct SmsSvcStatusTlv *lResp =
        &((struct QmiCbkImsaSvcStatusInd *)pResp)->SmsService;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the VOIP Service Status */
    eRCode = GetLong( pTlvData, &lResp->SmsSvcStatus );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
    
    lResp->TlvPresent   = TRUE;
    return eRCode;
}

/*
 * This function unpacks the VoipSvcStatus TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvVoipSvcStatus(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct VoipSvcStatusTlv *lResp =
        &((struct QmiCbkImsaSvcStatusInd *)pResp)->VoipService;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the VOIP Service Status */
    eRCode = GetLong( pTlvData, &lResp->VoipSvcStatus );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
    
    lResp->TlvPresent   = TRUE;
    return eRCode;
}

/*
 * This function unpacks the VtSvcStatus TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvVtSvcStatus(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct VtSvcStatusTlv *lResp =
        &((struct QmiCbkImsaSvcStatusInd *)pResp)->VtService;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the VT Service Status */
    eRCode = GetLong( pTlvData, &lResp->VtSvcStatus );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
   
    lResp->TlvPresent   = TRUE;
    return eRCode;
}

/*
 * This function unpacks the SmsRat TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSmsRat(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct SmsRatTlv *lResp =
        &((struct QmiCbkImsaSvcStatusInd *)pResp)->SmsRat;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the SMS RAT */
    eRCode = GetLong( pTlvData, &lResp->SmsRatVal );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
    
    lResp->TlvPresent   = TRUE;
    return eRCode;
}

/*
 * This function unpacks the VoipRat TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvVoipRat(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct VoipRatTlv *lResp =
        &((struct QmiCbkImsaSvcStatusInd *)pResp)->VoipRat;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the VOIP RAT */
    eRCode = GetLong( pTlvData, &lResp->VoipRatVal );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
   
    lResp->TlvPresent   = TRUE;
    return eRCode;
}

/*
 * This function unpacks the VtRat TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvVtRat(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct VtRatTlv *lResp =
        &((struct QmiCbkImsaSvcStatusInd *)pResp)->VtRat;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the VT RAT */
    eRCode = GetLong( pTlvData, &lResp->VtRatVal );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->TlvPresent   = TRUE;
    return eRCode;
}

/*
 * This function unpacks the UtSvcStatus TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvUtSvcStatus(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct UtSvcStatusTlv *lResp =
        &((struct QmiCbkImsaSvcStatusInd *)pResp)->UtService;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the UT Service Status */
    eRCode = GetLong( pTlvData, &lResp->UtSvcStatus );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
    
    lResp->TlvPresent   = TRUE;
    return eRCode;
}

/*
 * This function unpacks the UtRat TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvUtRat(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct UtRatTlv *lResp =
        &((struct QmiCbkImsaSvcStatusInd *)pResp)->UtRat;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    /* Extract the UT RAT */
    eRCode = GetLong( pTlvData, &lResp->UtRatVal );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }
    
    lResp->TlvPresent   = TRUE;
    return eRCode;
}
/*
 * This function unpacks the Service Status Indication message to a
 * user-provided response structure.
 *
 * \param   pMdmResp - Pointer to packed response from the modem.
 *
 * \param   pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkImsaSvcStatusInd(
    BYTE                            *pMdmResp,
    struct QmiCbkImsaSvcStatusInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_SMS_SVC_STATUS, &UnpackCbkTlvSmsSvcStatus },
        { eTLV_VOIP_SVC_STATUS, &UnpackCbkTlvVoipSvcStatus },
        { eTLV_VT_SVC_STATUS,   &UnpackCbkTlvVtSvcStatus },
        { eTLV_SMS_RAT,         &UnpackCbkTlvSmsRat },
        { eTLV_VOIP_RAT,        &UnpackCbkTlvVoipRat},
        { eTLV_VT_RAT,          &UnpackCbkTlvVtRat  },
        { eTLV_UT_SVC_STATUS,   &UnpackCbkTlvUtSvcStatus },
        { eTLV_UT_RAT,          &UnpackCbkTlvUtRat },
        { eTLV_TYPE_INVALID,   NULL } /* Important. Sentinel.
                                       * Signifies last item in map.
                                       */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_IMSA_SERVICE_STATUS_IND );
    return eRCode;
}
