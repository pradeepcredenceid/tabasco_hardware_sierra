/*
 * \ingroup ims
 *
 * \file   qaImsSLQSSetSIPConfig.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_IMS_SETTINGS_SET_SIP_CONFIG message.
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
#include "qaImsSLQSSetSIPConfig.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSetSIPConfig local port number to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvSIPPortNum(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetSIPConfigReq *pReq =
                        (struct QmiImsSLQSSetSIPConfigReq *)pParam;
    SetSIPConfigReq *tReq = pReq->pSetSIPConfigReq;

    if( NULL == tReq->pSIPLocalPort )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the SIP local port into the QMI message */
    return PutWord( pBuf, *(tReq->pSIPLocalPort) );
}

/*
 * This function packs the SLQSSetSIPConfig timer SIP registration to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvTimerRegSIP(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetSIPConfigReq *pReq =
                        (struct QmiImsSLQSSetSIPConfigReq *)pParam;
    SetSIPConfigReq *tReq = pReq->pSetSIPConfigReq;

    if( NULL == tReq->pTimerSIPReg )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the Timer SIP registration into the QMI message */
    return PutLong( pBuf, *(tReq->pTimerSIPReg) );
}

/*
 * This function packs the SLQSSetSIPConfig subscribe timer to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvSubsTimer(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetSIPConfigReq *pReq =
                        (struct QmiImsSLQSSetSIPConfigReq *)pParam;
    SetSIPConfigReq *tReq = pReq->pSetSIPConfigReq;

    if( NULL == tReq->pSubscribeTimer )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the subscribe timer into the QMI message */
    return PutLong( pBuf, *(tReq->pSubscribeTimer) );
}

/*
 * This function packs the SLQSSetSIPConfig Timer T1 to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvTimerT1(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetSIPConfigReq *pReq =
                        (struct QmiImsSLQSSetSIPConfigReq *)pParam;
    SetSIPConfigReq *tReq = pReq->pSetSIPConfigReq;

    if( NULL == tReq->pTimerT1 )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the Timer T1 into the QMI message */
    return PutLong( pBuf, *(tReq->pTimerT1) );
}

/*
 * This function packs the SLQSSetSIPConfig Timer T2 to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvTimerT2(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetSIPConfigReq *pReq =
                        (struct QmiImsSLQSSetSIPConfigReq *)pParam;
    SetSIPConfigReq *tReq = pReq->pSetSIPConfigReq;

    if( NULL == tReq->pTimerT2 )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the Timer T2 into the QMI message */
    return PutLong( pBuf, *(tReq->pTimerT2) );
}

/*
 * This function packs the SLQSSetSIPConfig Timer Tf to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvTimerTf(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetSIPConfigReq *pReq =
                        (struct QmiImsSLQSSetSIPConfigReq *)pParam;
    SetSIPConfigReq *tReq = pReq->pSetSIPConfigReq;

    if( NULL == tReq->pTimerTf )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the Timer Tf into the QMI message */
    return PutLong( pBuf, *(tReq->pTimerTf) );
}

/*
 * This function packs the SLQSSetSIPConfig Sig Comp status to the QMI
 * message SDU
 *
 * \param  pBuf   [OUT]  - Pointer to storage into which the packed data will
 *                         be placed by this function.
 *
 * \param  pParam [OUT]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError BuildTlvSigCompStatus(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiImsSLQSSetSIPConfigReq *pReq =
                        (struct QmiImsSLQSSetSIPConfigReq *)pParam;
    SetSIPConfigReq *tReq = pReq->pSetSIPConfigReq;

    if( NULL == tReq->pSigCompEnabled )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert the Sig Comp Status into the QMI message */
    return PutByte( pBuf, *(tReq->pSigCompEnabled) );
}

/*
 * This function packs the SLQSSetSIPConfig parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \param  pSetSIPConfigReq [IN] - Request parameters for packing.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiImsSLQSSetSIPConfig(
    WORD            *pMlength,
    BYTE            *pParamField,
    SetSIPConfigReq *pSetSIPConfigReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_SIP_PORT_NUM,    &BuildTlvSIPPortNum },
        { eTLV_TIMER_REG_SIP,   &BuildTlvTimerRegSIP },
        { eTLV_SUBSCRIBE_TIMER, &BuildTlvSubsTimer },
        { eTLV_TIMER_T1,        &BuildTlvTimerT1 },
        { eTLV_TIMER_T2,        &BuildTlvTimerT2 },
        { eTLV_TIMER_TF,        &BuildTlvTimerTf },
        { eTLV_SIGCOMP_STATUS,  &BuildTlvSigCompStatus },
        { eTLV_TYPE_INVALID,    NULL } /* Important. Sentinel.
                                        * Signifies last item in map
                                        */
    };

    struct QmiImsSLQSSetSIPConfigReq req;
    enum   eQCWWANError              eRCode;

    /* Intialize the request structure */
    slmemset( (char *)&req,
              0,
              sizeof(struct QmiImsSLQSSetSIPConfigReq) );
    req.pSetSIPConfigReq = pSetSIPConfigReq;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_IMS_SETTINGS_SET_SIP_CONFIG,
                      pMlength );
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSSetSIPConfig Settings Resp from the
 * QMI response message to a user provided response structure.
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvSettingsResp(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiImsSLQSSetSIPConfigResp *lResp =
        (struct QmiImsSLQSSetSIPConfigResp *)pResp;
    SetSIPConfigResp *tResp = lResp->pSetSIPConfigResp;

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
 * This function unpacks the SLQSSetSIPConfig response message to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiImsSLQSSetSIPConfig(
    BYTE                              *pMdmResp,
    struct QmiImsSLQSSetSIPConfigResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,   &qmUnpackTlvResultCode },
        { eTLV_SETTINGS_RESP, &UnpackTlvSettingsResp },
        { eTLV_TYPE_INVALID,  NULL } /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_IMS_SETTINGS_SET_SIP_CONFIG );
    return eRCode;
}
