/*
 * \ingroup cbk
 *
 * \file    qaCbkImsSLQSVoIPCfgInd.c
 *
 * \brief   Contains UnPacking routines for the QMI_IMS_VOIP_CONFIG_IND message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkImsSLQSVoIPCfgInd.h"

/* Locals */

/* Functions */

/*
 * This function unpacks the Session Duration TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSsnDur(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct SessDurTlv *lResp =
        &((struct QmiCbkImsVoIPConfigInd *)pResp)->SDTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              lTemp  = 0;

    /* Extract the Session Duration */
    eRCode = GetWord( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->sessExp  = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the Min Sessn Expiry Tmr TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvMinSessExp(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct MinSessExpTlv *lResp =
        &((struct QmiCbkImsVoIPConfigInd *)pResp)->MSETlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              lTemp  = 0;

    /* Extract the Minimum Session Expiry */
    eRCode = GetWord( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->minSessExp  = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the AMR WB Enable TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvAMRWB(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct EnabAMRWBTlv *lResp =
        &((struct QmiCbkImsVoIPConfigInd *)pResp)->EAWTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lTemp  = 0;

    /* Extract the AMR WB Enable */
    eRCode = GetByte( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->amrWBEnable = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the SCR AMR Enable TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSCRAMR(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct EnabSCRAMRTlv *lResp =
        &((struct QmiCbkImsVoIPConfigInd *)pResp)->ESATlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lTemp  = 0;

    /* Extract the SCR AMR Enable */
    eRCode = GetByte( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->scrAmrEnable = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the SCR AMR WB Enable TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvSCRAMRWB(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct EnabSCRAMRWBTlv *lResp =
        &((struct QmiCbkImsVoIPConfigInd *)pResp)->ESAWTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lTemp  = 0;

    /* Extract the SCR AMR WB Enable */
    eRCode = GetByte( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->scrAmrWBEnable = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the AMR Mode TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvAMRMode(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct AMRModeTlv *lResp =
        &((struct QmiCbkImsVoIPConfigInd *)pResp)->AMTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lTemp  = 0;

    /* Extract the AMR Mode */
    eRCode = GetByte( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->amrMode = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the AMR WB Mode TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvAMRWBMode(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct AMRWBModeTlv *lResp =
        &((struct QmiCbkImsVoIPConfigInd *)pResp)->AWMTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              lTemp  = 0;

    /* Extract the AMR WB Mode */
    eRCode = GetWord( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->amrWBMode = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the AMR Octet Aligned TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvAMROctAlgn(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct AMROctAlgnTlv *lResp =
        &((struct QmiCbkImsVoIPConfigInd *)pResp)->AOATlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lTemp  = 0;

    /* Extract the AMR Octet Aligned */
    eRCode = GetByte( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->amrOctAlgn = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the AMR WB Octet Aligned TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvAMRWBOctAlgn(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct AMRWBOctAlgnTlv *lResp =
        &((struct QmiCbkImsVoIPConfigInd *)pResp)->AWOATlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lTemp  = 0;

    /* Extract the AMR WB Octet Aligned */
    eRCode = GetByte( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->amrWBOctAlgn = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the Ringing Timer Duration TLV parameters from the QMI
 * indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvRngTmrDur(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct RngTmrTlv *lResp =
        &((struct QmiCbkImsVoIPConfigInd *)pResp)->RTTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              lTemp  = 0;

    /* Extract the Ringing Timer Aligned */
    eRCode = GetWord( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->RingTmr = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the Ringback Timer Duration TLV parameters from
 * the QMI indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvRngbkTmr(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct RngBkTmrTlv *lResp =
        &((struct QmiCbkImsVoIPConfigInd *)pResp)->RBTTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              lTemp  = 0;

    /* Extract the Ringing Timer Aligned */
    eRCode = GetWord( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->RingBkTmr = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the RTP/RTCP Inact Tmr Duration TLV parameters from
 * the QMI indication message to a user provided response structure
 *
 * \param   pTlvData - Pointer to TLV data from which to unpack.
 *
 * \param   pResp    - Pointer to structure containing storage
 *                     to return data for this TLV.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UnpackCbkTlvRITD(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    struct RTPRTCPInactTmrDurTlv *lResp =
        &((struct QmiCbkImsVoIPConfigInd *)pResp)->RTIDTlv;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              lTemp  = 0;

    /* Extract the Ringing Timer Aligned */
    eRCode = GetWord( pTlvData, &lTemp );
    if( eRCode != eQCWWAN_ERR_NONE )
    {
        return eRCode;
    }

    lResp->InactTmr = lTemp;
    lResp->TlvPresent = TRUE;

    return eRCode;
}

/*
 * This function unpacks the VoIP Config Info Indication message to a
 * user-provided response structure.
 *
 * \param   pMdmResp - Pointer to packed response from the modem.
 *
 * \param   pApiResp - Pointer to storage to unpack into.
 *
 * \return: eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 */
package enum eQCWWANError UpkQmiCbkImsVoIPCfgInd(
    BYTE                          *pMdmResp,
    struct QmiCbkImsVoIPConfigInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_SESS_EXP_TMR,     &UnpackCbkTlvSsnDur },
        { eTLV_MN_SESS_EXP,      &UnpackCbkTlvMinSessExp },
        { eTLV_AMR_WB_EN,        &UnpackCbkTlvAMRWB },
        { eTLV_SCR_AMR_EN,       &UnpackCbkTlvSCRAMR },
        { eTLV_SCR_AMR_WB_EN,    &UnpackCbkTlvSCRAMRWB },
        { eTLV_AMR_MOD,          &UnpackCbkTlvAMRMode },
        { eTLV_AMR_WB_MOD,       &UnpackCbkTlvAMRWBMode },
        { eTLV_AMR_OCT_ALIGN,    &UnpackCbkTlvAMROctAlgn },
        { eTLV_AMR_WB_OCT_ALIGN, &UnpackCbkTlvAMRWBOctAlgn },
        { eTLV_RINGING_TMR,      &UnpackCbkTlvRngTmrDur },
        { eTLV_RINGBCK_TMR,      &UnpackCbkTlvRngbkTmr },
        { eTLV_RTPRTCP_INAC_TMR, &UnpackCbkTlvRITD },
        { eTLV_TYPE_INVALID,     NULL } /* Important. Sentinel.
                                         * Signifies last item in map.
                                         */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_IMS_VOIP_CONFIG_IND );
    return eRCode;
}

