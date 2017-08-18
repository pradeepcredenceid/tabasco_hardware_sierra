/*
 * \ingroup ims
 *
 * \file   qaImsSLQSGetIMSVoIPConfig.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_IMS_SETTINGS_GET_VOIP_CONFIG message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"
#include "qaGobiApiIms.h"
#include "qaImsSLQSGetIMSVoIPConfig.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSGetIMSVoIPConfig parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiImsSLQSGetIMSVoIPConfig(
    WORD            *pMlength,
    BYTE            *pParamField )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map
                                     */
    };

    enum   eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      NULL,
                      map,
                      eQMI_IMS_SETTINGS_GET_IMS_VOIP_CFG,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSGetIMSVoIPConfig Settings Resp from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvSettgResp(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSVoIPConfigResp *lResp =
        (struct QmiImsSLQSGetIMSVoIPConfigResp *)pResp;
    GetIMSVoIPConfigResp *tResp = lResp->pGetIMSVoIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pSettingResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Settings Response in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pSettingResp) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSVoIPConfig session duration from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvSessDuration(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSVoIPConfigResp *lResp =
        (struct QmiImsSLQSGetIMSVoIPConfigResp *)pResp;
    GetIMSVoIPConfigResp *tResp = lResp->pGetIMSVoIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    if( NULL == tResp->pSessionExpiryTimer )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get session expiry timer in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pSessionExpiryTimer) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSVoIPConfig Min. Session Expiry Timer
 * from the QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvMinSessExpTmr(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSVoIPConfigResp *lResp =
        (struct QmiImsSLQSGetIMSVoIPConfigResp *)pResp;
    GetIMSVoIPConfigResp *tResp = lResp->pGetIMSVoIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    if( NULL == tResp->pMinSessionExpiryTimer )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get min. session expiry timer in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pMinSessionExpiryTimer) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSVoIPConfig Enable AMR WB from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvEnabAMRWB(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSVoIPConfigResp *lResp =
        (struct QmiImsSLQSGetIMSVoIPConfigResp *)pResp;
    GetIMSVoIPConfigResp *tResp = lResp->pGetIMSVoIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pAmrWbEnable )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Enable AMR WB in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pAmrWbEnable) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSVoIPConfig Enable SCR AMR from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvEnabSCRAMR(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSVoIPConfigResp *lResp =
        (struct QmiImsSLQSGetIMSVoIPConfigResp *)pResp;
    GetIMSVoIPConfigResp *tResp = lResp->pGetIMSVoIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pScrAmrEnable )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Enable AMR WB in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pScrAmrEnable) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSVoIPConfig Enable SCR AMR WB from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvEnabSCRAMRWB(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSVoIPConfigResp *lResp =
        (struct QmiImsSLQSGetIMSVoIPConfigResp *)pResp;
    GetIMSVoIPConfigResp *tResp = lResp->pGetIMSVoIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pScrAmrWbEnable )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Enable SCR AMR WB in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pScrAmrWbEnable) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSVoIPConfig AMR Mode from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvAMRMode(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSVoIPConfigResp *lResp =
        (struct QmiImsSLQSGetIMSVoIPConfigResp *)pResp;
    GetIMSVoIPConfigResp *tResp = lResp->pGetIMSVoIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pAmrMode )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get AMR Mode in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pAmrMode) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSVoIPConfig AMR WB Mode from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvAMRWBMode(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSVoIPConfigResp *lResp =
        (struct QmiImsSLQSGetIMSVoIPConfigResp *)pResp;
    GetIMSVoIPConfigResp *tResp = lResp->pGetIMSVoIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    if( NULL == tResp->pAmrWBMode )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get AMR WB Mode in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pAmrWBMode) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSVoIPConfig AMR Octet Align from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvAMROctetAlign(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSVoIPConfigResp *lResp =
        (struct QmiImsSLQSGetIMSVoIPConfigResp *)pResp;
    GetIMSVoIPConfigResp *tResp = lResp->pGetIMSVoIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pAmrOctetAligned )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get AMR Octet Align in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pAmrOctetAligned) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSVoIPConfig AMR WB Octet Align from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvAMRWBOctetAlign(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSVoIPConfigResp *lResp =
        (struct QmiImsSLQSGetIMSVoIPConfigResp *)pResp;
    GetIMSVoIPConfigResp *tResp = lResp->pGetIMSVoIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pAmrWBOctetAligned )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get AMR WB Octet Align in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pAmrWBOctetAligned) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSVoIPConfig Ringing Timer from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvRingingTmr(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSVoIPConfigResp *lResp =
        (struct QmiImsSLQSGetIMSVoIPConfigResp *)pResp;
    GetIMSVoIPConfigResp *tResp = lResp->pGetIMSVoIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    if( NULL == tResp->pRingingTimer )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Ringing Timer in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pRingingTimer) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSVoIPConfig Ringback Timer from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvRingbackTmr(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSVoIPConfigResp *lResp =
        (struct QmiImsSLQSGetIMSVoIPConfigResp *)pResp;
    GetIMSVoIPConfigResp *tResp = lResp->pGetIMSVoIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    if( NULL == tResp->pRingBackTimer )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Ringback Timer in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pRingBackTimer) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetIMSVoIPConfig RTP/RTCP Inactivity Timer
 * from the QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvRTPRTCPInactTmr(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetIMSVoIPConfigResp *lResp =
        (struct QmiImsSLQSGetIMSVoIPConfigResp *)pResp;
    GetIMSVoIPConfigResp *tResp = lResp->pGetIMSVoIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    if( NULL == tResp->pRTPRTCPInactTimer )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get RTP/RTCP Inactivity Timer in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pRTPRTCPInactTimer) = bTemp;
    return eRCode;
}
/*
 * This function unpacks the SLQSGetIMSVoIPConfig response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiImsSLQSGetIMSVoIPConfig(
    BYTE                                  *pMdmResp,
    struct QmiImsSLQSGetIMSVoIPConfigResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,         &qmUnpackTlvResultCode },
        { eTLV_SETTING_RSP,         &UnpackTlvSettgResp },
        { eTLV_SESS_EXPIRY_TMR,     &UnpackTlvSessDuration },
        { eTLV_MIN_SESS_EXPIRY_TMR, &UnpackTlvMinSessExpTmr },
        { eTLV_AMR_WB_ENAB,         &UnpackTlvEnabAMRWB },
        { eTLV_ENABLE_SCR_AMR,      &UnpackTlvEnabSCRAMR },
        { eTLV_ENABLE_SCR_AMR_WB,   &UnpackTlvEnabSCRAMRWB },
        { eTLV_AMR_MODE,            &UnpackTlvAMRMode },
        { eTLV_AMR_WB_MODE,         &UnpackTlvAMRWBMode },
        { eTLV_AMR_OCTET_ALGN,      &UnpackTlvAMROctetAlign },
        { eTLV_AMR_WB_OCTET_ALIGN,  &UnpackTlvAMRWBOctetAlign },
        { eTLV_RINGING_TIMER,       &UnpackTlvRingingTmr },
        { eTLV_RINGBK_TIMER,        &UnpackTlvRingbackTmr },
        { eTLV_RTPRTCP_TIMER,       &UnpackTlvRTPRTCPInactTmr },
        { eTLV_TYPE_INVALID,        NULL } /* Important. Sentinel.
                                            * Signifies last item in map.
                                            */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_IMS_SETTINGS_GET_IMS_VOIP_CFG );
    return eRCode;
}
