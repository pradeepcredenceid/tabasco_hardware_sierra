/*
 * \ingroup : nas
 *
 * \file    : qaNasSLQSConfigSigInfo.c
 *
 * \brief   : Contains Packing and UnPacking routines for the
 *            eQMI_NAS_CONFIG_SIG_INFO message.
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"
#include "qaNasSLQSConfigSigInfo.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the SLQSConfigSigInfo RSSI threshold
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvRSSIThresh( BYTE *pBuf, BYTE *pParam )
{
    sigInfo    *pReq = (sigInfo *)pParam;
    RSSIThresh *pRSReq = pReq->pRSSIThresh;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lCtr   = 0;

    if( NULL == pRSReq )
    {
        return eRCode;
    }

    /* Add TLV data */
    eRCode = PutByte( pBuf, pRSReq->RSSIThresListLen );
    if( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;

    for( lCtr = 0; lCtr < pRSReq->RSSIThresListLen; lCtr++ )
    {
        eRCode = PutByte( pBuf, pRSReq->pRSSIThresList[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
            return eRCode;
    }

    return eRCode;
}

/*
 * This function packs the SLQSConfigSigInfo ECIO threshold
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvECIOThresh( BYTE *pBuf, BYTE *pParam )
{
    sigInfo    *pReq = (sigInfo *)pParam;
    ECIOThresh *pECReq = pReq->pECIOThresh;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lCtr   = 0;

    if( NULL == pECReq )
    {
        return eRCode;
    }

    /* Add TLV data */
    eRCode = PutByte( pBuf, pECReq->ECIOThresListLen );
    if( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;

    for( lCtr = 0; lCtr < pECReq->ECIOThresListLen; lCtr++ )
    {
        eRCode = PutWord( pBuf, pECReq->pECIOThresList[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
            return eRCode;
    }

    return eRCode;
}

/*
 * This function packs the SLQSConfigSigInfo HDR threshold
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvHDRThresh( BYTE *pBuf, BYTE *pParam )
{
    sigInfo       *pReq = (sigInfo *)pParam;
    HDRSINRThresh *pHSReq = pReq->pHDRSINRThresh;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lCtr   = 0;

    if( NULL == pHSReq )
    {
        return eRCode;
    }

    /* Add TLV data */
    eRCode = PutByte( pBuf, pHSReq->HDRSINRThresListLen );
    if( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;

    for( lCtr = 0; lCtr < pHSReq->HDRSINRThresListLen; lCtr++ )
    {
        eRCode = PutByte( pBuf, pHSReq->pHDRSINRThresList[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
            return eRCode;
    }

    return eRCode;
}

/*
 * This function packs the SLQSConfigSigInfo LTE SNR threshold
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvLTESNRThresh( BYTE *pBuf, BYTE *pParam )
{
    sigInfo      *pReq = (sigInfo *)pParam;
    LTESNRThresh *pLSReq = pReq->pLTESNRThresh;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lCtr   = 0;

    if( NULL == pLSReq )
    {
        return eRCode;
    }

    /* Add TLV data */
    eRCode = PutByte( pBuf, pLSReq->LTESNRThresListLen );
    if( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;

    for( lCtr = 0; lCtr < pLSReq->LTESNRThresListLen; lCtr++ )
    {
        eRCode = PutWord( pBuf, pLSReq->pLTESNRThresList[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
            return eRCode;
    }

    return eRCode;
}

/*
 * This function packs the SLQSConfigSigInfo IO threshold
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvIOThresh( BYTE *pBuf, BYTE *pParam )
{
    sigInfo  *pReq = (sigInfo *)pParam;
    IOThresh *pITReq = pReq->pIOThresh;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lCtr   = 0;

    if( NULL == pITReq )
    {
        return eRCode;
    }

    /* Add TLV data */
    eRCode = PutByte( pBuf, pITReq->IOThresListLen );
    if( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;

    for( lCtr = 0; lCtr < pITReq->IOThresListLen; lCtr++ )
    {
        eRCode = PutLong( pBuf, pITReq->pIOThresList[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
            return eRCode;
    }

    return eRCode;
}

/*
 * This function packs the SLQSConfigSigInfo RSRQ threshold
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvRSRQThresh( BYTE *pBuf, BYTE *pParam )
{
    sigInfo    *pReq = (sigInfo *)pParam;
    RSRQThresh *pRSReq = pReq->pRSRQThresh;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lCtr   = 0;

    if( NULL == pRSReq )
    {
        return eRCode;
    }

    /* Add TLV data */
    eRCode = PutByte( pBuf, pRSReq->RSRQThresListLen );
    if( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;

    for( lCtr = 0; lCtr < pRSReq->RSRQThresListLen; lCtr++ )
    {
        eRCode = PutByte( pBuf, pRSReq->pRSRQThresList[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
            return eRCode;
    }

    return eRCode;
}

/*
 * This function packs the SLQSConfigSigInfo RSRP threshold
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvRSRPThresh( BYTE *pBuf, BYTE *pParam )
{
    sigInfo    *pReq = (sigInfo *)pParam;
    RSRPThresh *pRSReq = pReq->pRSRPThresh;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lCtr   = 0;

    if( NULL == pRSReq )
    {
        return eRCode;
    }

    /* Add TLV data */
    eRCode = PutByte( pBuf, pRSReq->RSRPThresListLen );
    if( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;

    for( lCtr = 0; lCtr < pRSReq->RSRPThresListLen; lCtr++ )
    {
        eRCode = PutWord( pBuf, pRSReq->pRSRPThresList[lCtr] );
        if( eQCWWAN_ERR_NONE != eRCode )
            return eRCode;
    }

    return eRCode;
}

/*
 * This function packs the SLQSConfigSigInfo LTE Signal report config
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvLTESigReportCfg( BYTE *pBuf, BYTE *pParam )
{
    sigInfo      *pReq = (sigInfo *)pParam;
    LTESigRptCfg *pLSReq = pReq->pLTESigRptCfg;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if( NULL == pLSReq )
    {
        return eRCode;
    }

    /* Add TLV data */
    eRCode = PutByte( pBuf, pLSReq->rptRate );
    if( eQCWWAN_ERR_NONE != eRCode )
        return eRCode;

    return PutByte( pBuf, pLSReq->avgPeriod );
}

/*
 * This function packs the SLQSConfigSigInfo parameters to the QMI message
 * SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                                    data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \param  pSigInfo    [IN] - Data to be packed
 *
 * \return eQCWWAN_ERR_NONE      - Increment successful
 * \return eQCWWAN_ERR_NO_MEMORY - Access beyond allowed size attempted
 *
 * \sa     qaGobiApiNas.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiNasSLQSConfigSigInfo(
    WORD    *pMlength,
    BYTE    *pParamField,
    sigInfo *pSigInfo )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_RSSI_THRESHOLD,        &BuildTlvRSSIThresh },
        { eTLV_ECIO_THRESHOLD,        &BuildTlvECIOThresh },
        { eTLV_HDR_SINR_THRESHOLD,    &BuildTlvHDRThresh },
        { eTLV_LTE_SNR_THRESHOLD,     &BuildTlvLTESNRThresh },
        { eTLV_IO_THRESHOLD,          &BuildTlvIOThresh },
        { eTLV_RSRQ_THRESHOLD,        &BuildTlvRSRQThresh },
        { eTLV_RSRP_THRESHOLD,        &BuildTlvRSRPThresh },
        { eTLV_LTE_SIGNAL_REPORT_CFG, &BuildTlvLTESigReportCfg },
        { eTLV_TYPE_INVALID,          NULL } /* Important. Sentinel.
                                              * Signifies last item in map.
                                              */
    };

    enum eQCWWANError           eRCode;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pSigInfo,
                      map,
                      eQMI_NAS_CONFIG_SIG_INFO,
                      pMlength );

    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************/

/* This function unpacks the SLQSConfigSigInfo response
 * message to a user-provided response structure.
 *
 * \param  pMdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param  pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE      - Increment successful
 * \return eQCWWAN_ERR_NO_MEMORY - Access beyond allowed size attempted
 *
 */
enum eQCWWANError UpkQmiNasSLQSConfigSigInfo(
    BYTE                               *pMdmResp,
    struct QmiNasSLQSConfigSigInfoResp *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,  &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map.
                                     */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_NAS_CONFIG_SIG_INFO );
    return eRCode;
}
