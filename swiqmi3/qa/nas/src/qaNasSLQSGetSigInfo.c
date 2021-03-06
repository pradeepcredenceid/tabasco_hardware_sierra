/*
 * \ingroup nas
 *
 * \file qaNasSLQSGetSigInfo.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         eQMI_NAS_GET_SIG_INFO message.
 *
 * Copyright: © 2011 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaNasSLQSGetSigInfo.h"
#include "qaGobiApiNasCommon.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the GetSigInfo parameters to the QMI message SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 */
enum eQCWWANError PkQmiNasSlqsGetSigInfo (
    WORD *pMlength,
    BYTE *pParamField )
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID, NULL }  /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };

    enum eQCWWANError eRCode;
    eRCode = qmbuild( pParamField,
                      NULL,
                      map,
                      eQMI_NAS_GET_SIG_INFO,
                      pMlength );
    return eRCode;
}


/*****************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the CDMA Signal Strength Info from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 */
enum eQCWWANError UnpackTlvCDMASSSigInfo( BYTE *pTlvData, BYTE *pResp )
{
    CDMASSInfo *lResp = ((struct QmiNasSlqsGetSigInfoResp *)
                                 pResp)->pGetSigInfoResp->pCDMASSInfo;

    /* Check For Invalid Parameter */
    if ( NULL == lResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Extract the CDMA Signal Strength Info */
    return UnpackTlvCommonCDMASigInfo( pTlvData, lResp );
}

/*
 * This function unpacks the HDR Signal Strength Info from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 */
enum eQCWWANError UnpackTlvHDRSSSigInfo( BYTE *pTlvData, BYTE *pResp )
{
    HDRSSInfo *lResp = ((struct QmiNasSlqsGetSigInfoResp *)
                                   pResp)->pGetSigInfoResp->pHDRSSInfo;

    /* Check For Invalid Parameter */
    if ( NULL == lResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Extract the HDR Signal Strength Info */
    return UnpackTlvCommonHDRSigInfo( pTlvData, lResp );
}

/*
 * This function unpacks the GSM Signal Strength Info from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 */
enum eQCWWANError UnpackTlvGSMSSSigInfo( BYTE *pTlvData, BYTE *pResp )
{
    nasGetSigInfoResp *lResp = ((struct QmiNasSlqsGetSigInfoResp *)
                                               pResp)->pGetSigInfoResp;

    if ( NULL == lResp->pGSMSSInfo )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Extract the GSM Signal Strength Info */
    return UnpackTlvCommonGSMSigInfo( pTlvData, lResp->pGSMSSInfo);
}

/*
 * This function unpacks the WCDMA Signal Strength Info from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 */
enum eQCWWANError UnpackTlvWCDMASSSigInfo( BYTE *pTlvData, BYTE *pResp )
{
    CDMASSInfo *lResp = ((struct QmiNasSlqsGetSigInfoResp *)
                                   pResp)->pGetSigInfoResp->pWCDMASSInfo;

    /* Check For Invalid Parameter */
    if ( NULL == lResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Extract the WCDMA Signal Strength Info */
    return UnpackTlvCommonCDMASigInfo( pTlvData, lResp );
}

/*
 * This function unpacks the LTE Signal Strength Info from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 */
enum eQCWWANError UnpackTlvLTESSSigInfo( BYTE *pTlvData, BYTE *pResp )
{
    LTESSInfo *lResp = ((struct QmiNasSlqsGetSigInfoResp *)
                                  pResp)->pGetSigInfoResp->pLTESSInfo;

    /* Check For Invalid Parameter */
    if ( NULL == lResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Extract the LTE Signal Strength Info */
    return UnpackTlvCommonLTESigInfo( pTlvData, lResp );
}

/*
 * This function unpacks the TD-SCDMA Signal Strength Info from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 */
enum eQCWWANError UnpackTlvTDSCDMASSSigInfo( BYTE *pTlvData, BYTE *pResp )
{
    nasGetSigInfoResp *lResp = ((struct QmiNasSlqsGetSigInfoResp *)
                                               pResp)->pGetSigInfoResp;

    if ( NULL == lResp->pTDSCDMASigInfoRscp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Extract the GSM Signal Strength Info */
    return UnpackTlvCommonTDSCDMASigInfo( pTlvData, lResp->pTDSCDMASigInfoRscp);
}

/*
 * This function unpacks the GetSigInfo response message to to a
 * user-provided response structure.
 *
 * \param     MdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 *  \return eQCWWAN_ERR_NONE on success, eQCWWAN_xxx error value otherwise
 *
 */
enum eQCWWANError UpkQmiNasSlqsGetSigInfo(
    BYTE                            *pMdmResp,
    struct QmiNasSlqsGetSigInfoResp *pApiResp)
{
    enum eQCWWANError rCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,       &qmUnpackTlvResultCode },
        { eTLV_CDMA_SS_SIG_INFO,  &UnpackTlvCDMASSSigInfo },
        { eTLV_HDR_SS_SIG_INFO,   &UnpackTlvHDRSSSigInfo },
        { eTLV_GSM_SS_SIG_INFO,   &UnpackTlvGSMSSSigInfo },
        { eTLV_WCDMA_SS_SIG_INFO, &UnpackTlvWCDMASSSigInfo },
        { eTLV_LTE_SS_SIG_INFO,   &UnpackTlvLTESSSigInfo },
        { eTLV_TDSCDMA_SS_SIG_INFO,&UnpackTlvTDSCDMASSSigInfo},
        { eTLV_TYPE_INVALID,      NULL }  /* Important. Sentinel.
                                           * Signifies last item in map.
                                           */
    };
    rCode = qmunpackresp( pMdmResp,
                          (BYTE *)pApiResp,
                          map,
                          eQMI_NAS_GET_SIG_INFO );
    return rCode;
}
