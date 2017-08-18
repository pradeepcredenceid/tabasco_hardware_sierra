/*
 * \ingroup nas
 *
 * \file qaNasSLQSSwiGetHDRPersonality.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_NAS_SWI_HDR_PERSONALITY message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaGobiApiNas.h"
#include "qaNasSLQSSwiGetHDRPersonality.h"
#include "qaGobiApiNasCommon.h"

/*****************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the SLQSSwiGetHDRPersonality parameters
 * to the QMI message SDU
 *
 * \param  pParamField [OUT] - Pointer to storage into which the packed
 *                             data will be placed by this function.
 *
 * \param  pMlength    [OUT] - Total length of built message.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError PkQmiNasSlqsSwiGetHDRPersonality(
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
                      eQMI_NAS_SWI_HDR_PERSONALITY,
                      pMlength);
    return eRCode;
}

/*****************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the Current personality from the
 * QMI response message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvCurrentPersonality(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiNasSlqsSwiGetHDRPersonalityResp *lResp =
            ( struct QmiNasSlqsSwiGetHDRPersonalityResp * )pResp;
    HDRPersonalityResp *tResp = lResp->pHDRPersonalityResp;

    /*Check for In/Out parameters for NULL*/
    if( NULL == tResp->pCurrentPersonality )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get CurrentPersonality from the response */
    return GetWord( pTlvData, tResp->pCurrentPersonality );

}

/*
 * This function unpacks the Personality Protocol Subtype list from the
 * QMI response message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                     to return data for this TLV.
 *
 * \return      eQCWWAN_ERR_NONE, on success
 * \return      eQCWWAN_ERR_MEMORY, on error
 */
enum eQCWWANError UnpackTlvPersonalityProtoSubtypeList(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiNasSlqsSwiGetHDRPersonalityResp *lResp =
            ( struct QmiNasSlqsSwiGetHDRPersonalityResp * )pResp;

    HDRPersonalityResp     *tResp = lResp->pHDRPersonalityResp;
    protocolSubtypeElement *sResp = tResp->pProtocolSubtypeElement;
    enum eQCWWANError      eRCode =  eQCWWAN_ERR_NONE;
    BYTE                   lPersLstLen;

    /*Check In/Out parameters for NULL*/
    if( NULL == tResp->pProtocolSubtypeElement )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get Personality List length from response*/
    eRCode = GetByte( pTlvData,tResp->pPersonalityListLength);
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
       return eRCode;
    }
    lPersLstLen = *(tResp->pPersonalityListLength);
    return UnpackTlvCommonPersnltyLst(pTlvData,sResp,lPersLstLen);
}

/*
 * This function unpacks the SlqsSwiGetHDRPersonality response message to to a
 * user-provided response structure.
 *
 * \param     pMdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return    eQCWWAN_ERR_NONE, on success
 * \return    eQCWWAN_ERR_MEMORY, on error
 */
enum eQCWWANError UpkQmiNasSlqsSwiGetHDRPersonality(
    BYTE                                      *pMdmResp,
    struct QmiNasSlqsSwiGetHDRPersonalityResp *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,            &qmUnpackTlvResultCode },
        { eTLV_CURRENT_PERS_INDEX,     &UnpackTlvCurrentPersonality },
        { eTLV_PERS_PROT_SUBTYPE_LIST, &UnpackTlvPersonalityProtoSubtypeList },
        { eTLV_TYPE_INVALID,           NULL }  /* Important. Sentinel.
                                                * Signifies last item in map.
                                                */
    };
    eRCode = qmunpackresp( pMdmResp,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_NAS_SWI_HDR_PERSONALITY );
    return eRCode;
}
