/*
 * \ingroup cbk
 *
 * \file qaCbkNasSwiHDRPersonalityInd.c
 *
 * \brief  Contains UnPacking routines for the
 *         QMI_NAS_SWI_HDR_PERSONALITY_IND message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkNasSwiHDRPersonalityInd.h"
#include "qaGobiApiCbk.h"
#include "qaGobiApiNasCommon.h"

/*Locals*/
static HDRPersonalityInd       hdrprsnlty[3];
static protocolSubtypeElement  protsubtypeelmet;
static WORD                    currentpersonality;
static BYTE                    perslstlen;

/*****************************************************************************
 * Unpacking functions
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
enum eQCWWANError UnpackTlvCrrntPrsnlty(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiCbkNasSwiHDRPersonalityInd *lResp =
            ( struct QmiCbkNasSwiHDRPersonalityInd * )pResp;

    HDRPersonalityInd *tResp = lResp->pHDRPersInd;
    tResp->pCurrentPersonality=&currentpersonality;

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
enum eQCWWANError UnpackTlvPersProtoSubtypeList(
    BYTE *pTlvData,
    BYTE *pResp )
{
    struct QmiCbkNasSwiHDRPersonalityInd *lResp =
            ( struct QmiCbkNasSwiHDRPersonalityInd * )pResp;

    HDRPersonalityInd *tResp = lResp->pHDRPersInd;

    tResp->pProtocolSubtypeElement = &protsubtypeelmet;

    protocolSubtypeElement *sResp = tResp->pProtocolSubtypeElement;
    enum eQCWWANError      eRCode =  eQCWWAN_ERR_NONE;
    BYTE                   lPersLstLen;

    tResp->pPersonalityListLength = &perslstlen;

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
 * This function unpacks the HDR Personality Indication message to to a
 * user-provided response structure.
 *
 * \param     pMdmResp   [IN]  - Pointer to packed response from the modem.
 *
 * \param     pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return    eQCWWAN_ERR_NONE, on success
 * \return    eQCWWAN_ERR_MEMORY, on error
 */
enum eQCWWANError UpkQmiCbkNasSwiHDRPersonalityInd(
    BYTE                                 *pMdmResp,
    struct QmiCbkNasSwiHDRPersonalityInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_CURRENT_PERS_INDEX_IND,     &UnpackTlvCrrntPrsnlty },
        { eTLV_PERS_PROT_SUBTYPE_LIST_IND, &UnpackTlvPersProtoSubtypeList },
        { eTLV_TYPE_INVALID,               NULL }  /* Important. Sentinel.
                                                    * Signifies last item in map.
                                                    */
    };
    pApiResp->pHDRPersInd = hdrprsnlty;
    eRCode = qmunpackresp( pMdmResp,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_NAS_SWI_HDR_PERSONALITY_IND );
    return eRCode;
}
