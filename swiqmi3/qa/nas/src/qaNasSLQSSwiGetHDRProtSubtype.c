/*
 * \ingroup nas
 *
 * \file qaNasSLQSSwiGetHDRProtSubtype.c
 *
 * \brief  Contains Packing and UnPacking routines for the
 *         QMI_NAS_SWI_HDR_PROT_SUBTYPE message.
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
#include "qaNasSLQSSwiGetHDRProtSubtype.h"
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
enum eQCWWANError PkQmiNasSlqsSwiGetHDRProtSubtype(
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
                      eQMI_NAS_SWI_HDR_PROT_SUBTYPE,
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
enum eQCWWANError UnpackTlvCurrentPrsnlty(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiNasSlqsSwiGetHDRProtSubtypeResp *lResp =
            ( struct QmiNasSlqsSwiGetHDRProtSubtypeResp * )pResp;
    HDRProtSubtypResp *tResp = lResp->pHDRProtSubtypResp;

    /*Check for In/Out parameters for NULL*/
    if( NULL == tResp->pCurrentPrsnlty )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get CurrentPersonality from the response */
    return GetWord( pTlvData, tResp->pCurrentPrsnlty );

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
 * \return      eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvPrsnltyProtoSbtypLst(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiNasSlqsSwiGetHDRProtSubtypeResp *lResp =
            ( struct QmiNasSlqsSwiGetHDRProtSubtypeResp * )pResp;

    HDRProtSubtypResp      *tResp = lResp->pHDRProtSubtypResp;
    protocolSubtypeElement *sResp = tResp->pProtoSubTypElmnt;
    enum eQCWWANError      eRCode =  eQCWWAN_ERR_NONE;
    BYTE                   lPersLstLen;

    /*Check In/Out parameters for NULL*/
    if( NULL == tResp->pProtoSubTypElmnt )
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

    /*Unpack Personality List*/
    return UnpackTlvCommonPersnltyLst(pTlvData,sResp,lPersLstLen);
}

/*
 * This function unpacks the App Subtype from the
 * QMI response message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvAppSubtype(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiNasSlqsSwiGetHDRProtSubtypeResp *lResp =
            ( struct QmiNasSlqsSwiGetHDRProtSubtypeResp * )pResp;

    HDRProtSubtypResp *tResp = lResp->pHDRProtSubtypResp;

    enum eQCWWANError eRCode =  eQCWWAN_ERR_NONE;

    /*Check for In/Out parameters for NULL*/
    if( NULL == tResp->pAppSubType )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Get App Subtype from the response */
    eRCode =  GetLongLong( pTlvData, tResp->pAppSubType );

    return eRCode;
}

/*
 * This function unpacks the SlqsSwiGetHDRPersonality response message to to a
 * user-provided response structure.
 *
 * \param     pMdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return    eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UpkQmiNasSlqsSwiGetHDRProtSubtype(
    BYTE                                      *pMdmResp,
    struct QmiNasSlqsSwiGetHDRProtSubtypeResp *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,                  &qmUnpackTlvResultCode },
        { eTLV_PROTO_CURRENT_PERS_INDEX,     &UnpackTlvCurrentPrsnlty },
        { eTLV_PROTO_PERS_PROT_SUBTYPE_LIST, &UnpackTlvPrsnltyProtoSbtypLst },
        { eTLV_PROTO_APP_SUBTYPES,           &UnpackTlvAppSubtype },
        { eTLV_TYPE_INVALID,                 NULL }  /* Important. Sentinel.
                                                      * Signifies last item in map.
                                                      */
    };
    eRCode = qmunpackresp( pMdmResp,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_NAS_SWI_HDR_PROT_SUBTYPE );
    return eRCode;
}
