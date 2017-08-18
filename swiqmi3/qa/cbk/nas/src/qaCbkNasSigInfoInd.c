/*
 * \ingroup nas
 *
 * \file    qaCbkNasSigInfoInd.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          eQMI_NAS_SIG_INFO_IND message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkNasSigInfoInd.h"
#include "qaGobiApiNasCommon.h"

/* Locals*/
static CDMASSInfo        cdmasiginfo;
static HDRSSInfo         hdrsiginfo;
static BYTE              gsmsiginfo;
static CDMASSInfo        wcdmasiginfo;
static LTESSInfo         ltesiginfo;
static TDSCDMASigInfo    tdscdmasiginfo;
static TDSCDMASigInfoExt tdscdmaextsiginfo;

/* Functions */
/*
 * This function unpacks the CDMA Signal strength Information from the QMI
 * indication message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UnpackTlvIndCDMASSigInfo(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    nasSigInfo *lResp = ( nasSigInfo *)pResp;

    /* Allocate memory to the pointer */
    lResp->pCDMASigInfo = &cdmasiginfo;

    /* Extract the CDMA Signal Strength Status Info */
    return UnpackTlvCommonCDMASigInfo( pTlvData, lResp->pCDMASigInfo );
}

/*
 * This function unpacks the HDR Signal Strength Information from the QMI
 * indication message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UnpackTlvIndHDRSSigInfo(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    nasSigInfo *lResp = ( nasSigInfo *)pResp;

    /* Allocate memory to the pointer */
    lResp->pHDRSigInfo=&hdrsiginfo;

    /* Extract the HDR Signal Strength Status Info */
    return UnpackTlvCommonHDRSigInfo( pTlvData, lResp->pHDRSigInfo );
}

/*
 * This function unpacks the GSM Signal Strength Information from the QMI
 * indication message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UnpackTlvIndGSMSSigInfo(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    nasSigInfo *lResp = ( nasSigInfo *)pResp;

    /* Allocate memory to the pointer */
    lResp->pGSMSigInfo=&gsmsiginfo;

    /* Extract the GSM Signal Strength Status Info */
    return UnpackTlvCommonGSMSigInfo( pTlvData, lResp->pGSMSigInfo );
}

/*
 * This function unpacks the WCDMA Signal Strength Information from the QMI
 * indication message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UnpackTlvIndWCDMASSigInfo(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    nasSigInfo *lResp = ( nasSigInfo *)pResp;

    /* Allocate memory to the pointer */
    lResp->pWCDMASigInfo=&wcdmasiginfo;

    /* Extract the WCDMA Signal Strength Status Info */
    return UnpackTlvCommonCDMASigInfo( pTlvData, lResp->pWCDMASigInfo );
}

/*
 * This function unpacks the LTE Signal Strength Information from the QMI
 * indication message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UnpackTlvIndLTESSigInfo(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    nasSigInfo *lResp = ( nasSigInfo *)pResp;

    /* Allocate memory to the pointer */
    lResp->pLTESigInfo = &ltesiginfo;

    /* Extract the LTE Signal Strength Status Info */
    return UnpackTlvCommonLTESigInfo( pTlvData, lResp->pLTESigInfo );
}

/*
 * This function unpacks the TDSCDMA Signal Strength Information from the QMI
 * indication message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UnpackTlvIndTDSCDMASSigInfo(
    BYTE *pTlvData,
    BYTE *pResp )
{
    /* Get the required TLV structure to the local pointer */
    nasSigInfo *lResp = ( nasSigInfo *)pResp;

    /* Allocate memory to the pointer */
    lResp->pTDSCDMASigInfo = &tdscdmasiginfo;

    /* Extract the LTE Signal Strength Status Info */
    return UnpackTlvCommonTDSCDMASigInfo( pTlvData, lResp->pTDSCDMASigInfo );
}

/*
 * This function unpacks the TDSCDMA extended Signal Strength Information
 * from the QMI indication message to a user provided response structure
 * 
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 * 
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 * 
 */
enum eQCWWANError UnpackTlvIndTDSCDMAExtSSigInfo(
    BYTE *pTlvData,
    BYTE *pResp )
{   
    /* Get the required TLV structure to the local pointer */
    nasSigInfo *lResp = ( nasSigInfo *)pResp;

    /* Allocate memory to the pointer */
    lResp->pTDSCDMASigInfoExt = &tdscdmaextsiginfo;
        
    /* Extract the LTE Signal Strength Status Info */
    return UnpackTlvCommonTDSCDMAExtSigInfo( pTlvData, lResp->pTDSCDMASigInfoExt );
}

/*
 * This function unpacks the System Information indication message to to a
 * user-provided response structure.
 *
 * \param     pMdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT]  - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiCbkNasSigInfoInd(
    BYTE       *pMdmResp,
    nasSigInfo *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_IND_CDMA_SIG_S_INFO,       &UnpackTlvIndCDMASSigInfo },
        { eTLV_IND_HDR_SIG_S_INFO,        &UnpackTlvIndHDRSSigInfo },
        { eTLV_IND_GSM_SIG_S_INFO,        &UnpackTlvIndGSMSSigInfo },
        { eTLV_IND_WCDMA_SIG_S_INFO,      &UnpackTlvIndWCDMASSigInfo },
        { eTLV_IND_LTE_SIG_S_INFO,        &UnpackTlvIndLTESSigInfo },
        { eTLV_IND_TDSCDMA_SIG_INFO,      &UnpackTlvIndTDSCDMASSigInfo }, 
        { eTLV_IND_TDSCDMA_EXT_SIG_INFO,  &UnpackTlvIndTDSCDMAExtSSigInfo }, 
        { eTLV_TYPE_INVALID,              NULL }  /* Important. Sentinel.
                                                   * Signifies last item in map.
                                                   */
    };
    eRCode = qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_NAS_SIG_INFO_IND );
    return eRCode;
}
