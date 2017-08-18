/*
 * \ingroup ims
 *
 * \file   qaImsSLQSSetIMSVoIPConfig.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_IMS_SETTINGS_SET_IMS_VOIP_CFG message.
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
#include "qaImsSLQSSetIMSVoIPConfig.h"
#include "qaImsSLQSSetSIPConfig.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetIMSVoIPConfig session duration to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvSessDuration(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSVoIPConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSVoIPConfigReq *)pParam;
    SetIMSVoIPConfigReq *tReq = pReq->pSetIMSVoIPConfigReq;

    if( NULL == tReq->pSessionExpiryTimer )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the Session Expiry Timer into the QMI message */
    return PutWord( pBuf, *(tReq->pSessionExpiryTimer) );
}

/*
 * This function packs the SLQSSetIMSVoIPConfig minimum session expiry to the
 * QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvSessExpiry(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSVoIPConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSVoIPConfigReq *)pParam;
    SetIMSVoIPConfigReq *tReq = pReq->pSetIMSVoIPConfigReq;

    if( NULL == tReq->pMinSessionExpiryTimer )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the Minimum Session Expiry Timer into the QMI message */
    return PutWord( pBuf, *(tReq->pMinSessionExpiryTimer) );
}

/*
 * This function packs the SLQSSetIMSVoIPConfig Enable AMR WB to the
 * QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvEnableAMRWB(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSVoIPConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSVoIPConfigReq *)pParam;
    SetIMSVoIPConfigReq *tReq = pReq->pSetIMSVoIPConfigReq;

    if( NULL == tReq->pAmrWbEnable )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the Enable AMR WB Flag into the QMI message */
    return PutByte( pBuf, *(tReq->pAmrWbEnable) );
}

/*
 * This function packs the SLQSSetIMSVoIPConfig Enable SCR for AMR to the
 * QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvEnableSCRAMR(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSVoIPConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSVoIPConfigReq *)pParam;
    SetIMSVoIPConfigReq *tReq = pReq->pSetIMSVoIPConfigReq;

    if( NULL == tReq->pScrAmrEnable )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the Enable SCR for AMR Flag into the QMI message */
    return PutByte( pBuf, *(tReq->pScrAmrEnable) );
}

/*
 * This function packs the SLQSSetIMSVoIPConfig Enable SCR for AMR WB to the
 * QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvEnableSCRAMRWB(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSVoIPConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSVoIPConfigReq *)pParam;
    SetIMSVoIPConfigReq *tReq = pReq->pSetIMSVoIPConfigReq;

    if( NULL == tReq->pScrAmrWbEnable )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the Enable SCR for AMR WB Flag into the QMI message */
    return PutByte( pBuf, *(tReq->pScrAmrWbEnable) );
}

/*
 * This function packs the SLQSSetIMSVoIPConfig AMR Mode to the
 * QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAMRMode(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSVoIPConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSVoIPConfigReq *)pParam;
    SetIMSVoIPConfigReq *tReq = pReq->pSetIMSVoIPConfigReq;

    if( NULL == tReq->pAmrMode )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the AMR Mode into the QMI message */
    return PutByte( pBuf, *(tReq->pAmrMode) );
}

/*
 * This function packs the SLQSSetIMSVoIPConfig AMR WB Mode to the
 * QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAMRWBMode(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSVoIPConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSVoIPConfigReq *)pParam;
    SetIMSVoIPConfigReq *tReq = pReq->pSetIMSVoIPConfigReq;

    if( NULL == tReq->pAmrWBMode )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the AMR WB Mode into the QMI message */
    return PutWord( pBuf, *(tReq->pAmrWBMode) );
}

/*
 * This function packs the SLQSSetIMSVoIPConfig AMR Octet Aligned to the
 * QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAMROctAlign(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSVoIPConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSVoIPConfigReq *)pParam;
    SetIMSVoIPConfigReq *tReq = pReq->pSetIMSVoIPConfigReq;

    if( NULL == tReq->pAmrOctetAligned )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the AMR Octet Aligned into the QMI message */
    return PutByte( pBuf, *(tReq->pAmrOctetAligned) );
}

/*
 * This function packs the SLQSSetIMSVoIPConfig AMR WB Octet Aligned to the
 * QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvAMRWBOctAlign(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSVoIPConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSVoIPConfigReq *)pParam;
    SetIMSVoIPConfigReq *tReq = pReq->pSetIMSVoIPConfigReq;

    if( NULL == tReq->pAmrWBOctetAligned )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the AMR WB Octet Aligned into the QMI message */
    return PutByte( pBuf, *(tReq->pAmrWBOctetAligned) );
}

/*
 * This function packs the SLQSSetIMSVoIPConfig Ringing Timer to the
 * QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvRingTmr(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSVoIPConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSVoIPConfigReq *)pParam;
    SetIMSVoIPConfigReq *tReq = pReq->pSetIMSVoIPConfigReq;

    if( NULL == tReq->pRingingTimer )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the Ringing Timer into the QMI message */
    return PutWord( pBuf, *(tReq->pRingingTimer) );
}

/*
 * This function packs the SLQSSetIMSVoIPConfig Ring Back Timer to the
 * QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvRingBkTmr(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSVoIPConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSVoIPConfigReq *)pParam;
    SetIMSVoIPConfigReq *tReq = pReq->pSetIMSVoIPConfigReq;

    if( NULL == tReq->pRingBackTimer )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the Ring Back Timer into the QMI message */
    return PutWord( pBuf, *(tReq->pRingBackTimer) );
}

/*
 * This function packs the SLQSSetIMSVoIPConfig RTP/RTCP Inact Timer Duration
 * to the QMI message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvRTPInactTmr(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetIMSVoIPConfigReq *pReq =
                        (struct QmiImsSLQSSetIMSVoIPConfigReq *)pParam;
    SetIMSVoIPConfigReq *tReq = pReq->pSetIMSVoIPConfigReq;

    if( NULL == tReq->pRTPRTCPInactTimer )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the RTP/RTCP Inactivity Timer into the QMI message */
    return PutWord( pBuf, *(tReq->pRTPRTCPInactTimer) );
}

/*
 * This function packs the SLQSSetIMSVoIPConfig parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \param  pSetIMSVoIPConfigReq [IN] - Request parameters for packing.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiImsSLQSSetIMSVoIPConfig(
    WORD                *pMlength,
    BYTE                *pParamField,
    SetIMSVoIPConfigReq *pSetIMSVoIPConfigReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_SESS_EXP_TIMER,    &BuildTlvSessDuration },
        { eTLV_MIN_SESS_EXP,      &BuildTlvSessExpiry },
        { eTLV_AMR_WB_ENABLE,     &BuildTlvEnableAMRWB },
        { eTLV_SCR_AMR_ENABLE,    &BuildTlvEnableSCRAMR },
        { eTLV_SCR_AMR_WB_ENABLE, &BuildTlvEnableSCRAMRWB },
        { eTLV_AMR_MODES,         &BuildTlvAMRMode },
        { eTLV_AMR_WB_MODES,      &BuildTlvAMRWBMode },
        { eTLV_AMR_OCTET_ALIGN,   &BuildTlvAMROctAlign },
        { eTLV_AM_WB_OCTET_ALIGN, &BuildTlvAMRWBOctAlign },
        { eTLV_RING_TIMER,        &BuildTlvRingTmr },
        { eTLV_RINGBACK_TIMER,    &BuildTlvRingBkTmr },
        { eTLV_RTPRTCPINACTTIMER, &BuildTlvRTPInactTmr },
        { eTLV_TYPE_INVALID,      NULL } /* Important. Sentinel.
                                          * Signifies last item in map
                                          */
    };

    struct QmiImsSLQSSetIMSVoIPConfigReq req;
    enum   eQCWWANError                  eRCode;

    /* Intialize the request structure */
    slmemset( (char *)&req,
              0,
              sizeof(struct QmiImsSLQSSetIMSVoIPConfigReq) );
    req.pSetIMSVoIPConfigReq = pSetIMSVoIPConfigReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_IMS_SETTINGS_SET_IMS_VOIP_CFG,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSSetIMSVoIPConfig Settings Resp from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvVoIPCfgSetResp(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSSetIMSVoIPConfigResp *lResp =
        (struct QmiImsSLQSSetIMSVoIPConfigResp *)pResp;
    SetIMSVoIPConfigResp *tResp = lResp->pSetIMSVoIPConfigResp;

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
 * This function unpacks the SLQSSetIMSVoIPConfig response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiImsSLQSSetIMSVoIPConfig(
    BYTE                                  *pMdmResp,
    struct QmiImsSLQSSetIMSVoIPConfigResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,   &qmUnpackTlvResultCode },
        { eTLV_SETTINGS_RESP, &UnpackTlvVoIPCfgSetResp },
        { eTLV_TYPE_INVALID,  NULL } /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_IMS_SETTINGS_SET_IMS_VOIP_CFG );
    return eRCode;
}

