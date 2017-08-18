/*
 * \ingroup cbk
 *
 * \file    qaCbkImsSLQSSIPCfgInd.c
 *
 * \brief   Contains UnPacking routines for the QMI_IMS_SIP_CONFIG_IND message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkImsSLQSSIPCfgInd.h"

/* Locals */

/* Functions */

/*
 * This function unpacks the SIP port info TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSIPPortNum(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct SIPPortTlv *lResp =
        &((struct QmiCbkImsSIPConfigInd *)pResp)->SPTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              lTemp  = 0;

    /* Extract the SIP Local Port */
    eRCode = GetWord( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->SIPLocalPort = lTemp;
    lResp->TlvPresent   = TRUE;

    return eRCode;
}

/*
 * This function unpacks the SIP Registration Timer info TLV parameters from
 * the QMI indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSIPRegTmr(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct SIPRegnTmrTlv *lResp =
        &((struct QmiCbkImsSIPConfigInd *)pResp)->SRTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    ULONG             lTemp  = 0;

    /* Extract the SIP Registration Timer */
    eRCode = GetLong( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->tmrSIPRegn = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the Subscriber Timer info TLV parameters from
 * the QMI indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSubscrTmr(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct SubscrTmrTlv *lResp =
        &((struct QmiCbkImsSIPConfigInd *)pResp)->STTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    ULONG             lTemp  = 0;

    /* Extract the Subscribe Timer */
    eRCode = GetLong( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->subscrTmr = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the Timer T1 info TLV parameters from
 * the QMI indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvTmrT1(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct TmrT1Tlv *lResp =
        &((struct QmiCbkImsSIPConfigInd *)pResp)->TT1Tlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    ULONG             lTemp  = 0;

    /* Extract the Timer T1 */
    eRCode = GetLong( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->tmrT1      = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the Timer T2 info TLV parameters from
 * the QMI indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvTmrT2(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct TmrT2Tlv *lResp =
        &((struct QmiCbkImsSIPConfigInd *)pResp)->TT2Tlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    ULONG             lTemp  = 0;

    /* Extract the Timer T2 */
    eRCode = GetLong( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->tmrT2      = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the Timer Tf info TLV parameters from
 * the QMI indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvTmrTf(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct TmrTfTlv *lResp =
        &((struct QmiCbkImsSIPConfigInd *)pResp)->TTfTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    ULONG             lTemp  = 0;

    /* Extract the Timer Tf */
    eRCode = GetLong( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->tmrTf      = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the SigComp Status info TLV parameters from
 * the QMI indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSigCompEn(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct SigCompEnTlv *lResp =
        &((struct QmiCbkImsSIPConfigInd *)pResp)->SCTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lTemp  = 0;

    /* Extract the SigComp Status */
    eRCode = GetByte( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->SigCompEn  = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the SIP Config Info Indication message to a
 * user-provided response structure.
 *
 * \param   pMdmResp - Pointer to packed response from the modem.
 *
 * \param   pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkImsSIPCfgInd(
    BYTE                         *pMdmResp,
    struct QmiCbkImsSIPConfigInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_SIP_PORT_NO,  &UnpackCbkTlvSIPPortNum },
        { eTLV_SIP_REGN_TMR, &UnpackCbkTlvSIPRegTmr  },
        { eTLV_SUBSCR_TMR,   &UnpackCbkTlvSubscrTmr },
        { eTLV_TIMR_T1,      &UnpackCbkTlvTmrT1 },
        { eTLV_TIMR_T2,      &UnpackCbkTlvTmrT2 },
        { eTLV_TIMR_TF,      &UnpackCbkTlvTmrTf },
        { eTLV_SIGCOMP_EN,   &UnpackCbkTlvSigCompEn },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map.
                                     */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_IMS_SIP_CONFIG_IND );
    return eRCode;
}
