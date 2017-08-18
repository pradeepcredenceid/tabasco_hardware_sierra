/*
 * \ingroup ims
 *
 * \file   qaImsSLQSGetSIPConfig.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_IMS_SETTINGS_GET_SIP_CONFIG message.
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
#include "qaImsSLQSGetSIPConfig.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSGetSIPConfig parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiImsSLQSGetSIPConfig(
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
                      eQMI_IMS_SETTINGS_GET_SIP_CONFIG,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSGetSIPConfig Settings Resp from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvSettngsResp(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetSIPConfigResp *lResp =
        (struct QmiImsSLQSGetSIPConfigResp *)pResp;
    GetSIPConfigResp *tResp = lResp->pGetSIPConfigResp;

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
 * This function unpacks the SLQSGetSIPConfig Port Number from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvPortNumb(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetSIPConfigResp *lResp =
        (struct QmiImsSLQSGetSIPConfigResp *)pResp;
    GetSIPConfigResp *tResp = lResp->pGetSIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    WORD              bTemp;

    if( NULL == tResp->pSIPLocalPort )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get local port number in local variable */
    eRCode = GetWord( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pSIPLocalPort) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetSIPConfig Registration Timer from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvTmrReg(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetSIPConfigResp *lResp =
        (struct QmiImsSLQSGetSIPConfigResp *)pResp;
    GetSIPConfigResp *tResp = lResp->pGetSIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    ULONG             bTemp;

    if( NULL == tResp->pTimerSIPReg )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get SIP Registration Timer in local variable */
    eRCode = GetLong( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pTimerSIPReg) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetSIPConfig Subscriber Timer from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvSubTmr(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetSIPConfigResp *lResp =
        (struct QmiImsSLQSGetSIPConfigResp *)pResp;
    GetSIPConfigResp *tResp = lResp->pGetSIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    ULONG             bTemp;

    if( NULL == tResp->pSubscribeTimer )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Subscriber Timer in local variable */
    eRCode = GetLong( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pSubscribeTimer) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetSIPConfig Timer T1 from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvTmrT1(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetSIPConfigResp *lResp =
        (struct QmiImsSLQSGetSIPConfigResp *)pResp;
    GetSIPConfigResp *tResp = lResp->pGetSIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    ULONG             bTemp;

    if( NULL == tResp->pTimerT1 )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Timer T1 in local variable */
    eRCode = GetLong( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pTimerT1) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetSIPConfig Timer T2 from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvTmrT2(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetSIPConfigResp *lResp =
        (struct QmiImsSLQSGetSIPConfigResp *)pResp;
    GetSIPConfigResp *tResp = lResp->pGetSIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    ULONG             bTemp;

    if( NULL == tResp->pTimerT2 )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Timer T2 in local variable */
    eRCode = GetLong( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pTimerT2) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetSIPConfig Timer Tf from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvTmrTf(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetSIPConfigResp *lResp =
        (struct QmiImsSLQSGetSIPConfigResp *)pResp;
    GetSIPConfigResp *tResp = lResp->pGetSIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    ULONG             bTemp;

    if( NULL == tResp->pTimerTf )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Timer Tf in local variable */
    eRCode = GetLong( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pTimerTf) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetSIPConfig SigComp Enabled from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvsigCompStat(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSGetSIPConfigResp *lResp =
        (struct QmiImsSLQSGetSIPConfigResp *)pResp;
    GetSIPConfigResp *tResp = lResp->pGetSIPConfigResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              bTemp;

    if( NULL == tResp->pSigCompEnabled )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get SigComp Enabled in local variable */
    eRCode = GetByte( pTlvData, &bTemp );
    if (eQCWWAN_ERR_NONE != eRCode)
    {
        return eRCode;
    }

    *(tResp->pSigCompEnabled) = bTemp;
    return eRCode;
}

/*
 * This function unpacks the SLQSGetSIPConfig response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiImsSLQSGetSIPConfig(
    BYTE                              *pMdmResp,
    struct QmiImsSLQSGetSIPConfigResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,     &qmUnpackTlvResultCode },
        { eTLV_SETT_RESP,       &UnpackTlvSettngsResp },
        { eTLV_SIP_PORT_NUMBER, &UnpackTlvPortNumb },
        { eTLV_TMR_REG_SIP,     &UnpackTlvTmrReg },
        { eTLV_SUBSCRIBE_TMR,   &UnpackTlvSubTmr },
        { eTLV_TMR_T1,          &UnpackTlvTmrT1 },
        { eTLV_TMR_T2,          &UnpackTlvTmrT2 },
        { eTLV_TMR_TF,          &UnpackTlvTmrTf },
        { eTLV_SIGCOMP_STAT,    &UnpackTlvsigCompStat },
        { eTLV_TYPE_INVALID,    NULL } /* Important. Sentinel.
                                        * Signifies last item in map.
                                        */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_IMS_SETTINGS_GET_SIP_CONFIG );
    return eRCode;
}
