/*
 * \ingroup : loc
 *
 * \file    : qaSlqsLocStart.c
 *
 * \brief   : Contains Packing and UnPacking routines for the
 *            eQMI_LOC_SET_SERVER message.
 *
 * Copyright: © 2011-2015 Sierra Wireless, Inc. all rights reserved
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaSlqsLocSetServer.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the LOC SetServer request Server type field
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvSetServerType( BYTE *pBuf, BYTE *pParam )
{

    LOCSetServerReq *pReq = (LOCSetServerReq *)pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if(pReq==NULL)
        return eQCWWAN_ERR_INVALID_ARG;
    /* Insert server type */
    eRCode = PutLong( pBuf, pReq->serverType );
    return eRCode;
}

/*
 * This function packs the LOC SetServer request IPV4 address field
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvIPv4Address( BYTE *pBuf, BYTE *pParam )
{
    LOCSetServerReq *pReq = (LOCSetServerReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    
    if(pReq==NULL)
        return eQCWWAN_ERR_INVALID_ARG;

    if( NULL == pReq->pIPv4Address )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert IPV4 Address */
    eRCode = PutLong( pBuf, pReq->pIPv4Address->ipv4Addr);
    if ( eRCode != eQCWWAN_ERR_NONE )
        return eRCode;
    
    /* Insert IPV4 port */
    eRCode = PutWord( pBuf, pReq->pIPv4Address->port);
    if ( eRCode != eQCWWAN_ERR_NONE )
        return eRCode;
        
    return eRCode;
}


/*
 * This function packs the LOC SetServer request IPV6 address field
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvIPv6Address( BYTE *pBuf, BYTE *pParam )
{
    LOCSetServerReq *pReq = (LOCSetServerReq *)pParam;
    USHORT idx;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    
    if(pReq==NULL)
        return eQCWWAN_ERR_INVALID_ARG;

    if( NULL == pReq->pIPv6Address )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert IPV6 Address */
    for (idx = 0; idx < 8; idx++)
    {
        eRCode = PutWord( pBuf, pReq->pIPv6Address->ipv6Addr[idx]);
        if ( eRCode != eQCWWAN_ERR_NONE )
            return eRCode;
    }        
    
    /* Insert IPV6 port */
    eRCode = PutLong( pBuf, pReq->pIPv6Address->port);
    if ( eRCode != eQCWWAN_ERR_NONE )
        return eRCode;
        
    return eRCode;
}

/*
 * This function packs the LOC SetServer request URL field
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvURLAddress( BYTE *pBuf, BYTE *pParam )
{
    LOCSetServerReq *pReq = (LOCSetServerReq *)pParam;
    USHORT idx;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    
    if(pReq==NULL)
        return eQCWWAN_ERR_INVALID_ARG;

    if( NULL == pReq->pURLAddress )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert URL */
    for (idx = 0; pReq->pURLAddress->url[idx]!=EOS ; idx++)
    {
        PutByte(pBuf, pReq->pURLAddress->url[idx]);
        if( eQCWWAN_ERR_NONE != eRCode )
            break;
    }
        
    return eRCode;
}
/*
 * Packs the PkQmiLOCSetServerReq parameters to the QMI message SDU
 *
 * \param  pParamField[OUT]         - Pointer to storage into which the packed
 *                                    data will be placed by this function.
 *
 * \param  pMlength[OUT]            - Total length of built message.
 *
 * \param  pLOCSetServerReqReq[IN]     - LOC SetServer Request Parameters
 *
 * \return  eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX on error
 *
 */
enum eQCWWANError PkQmiLocSetServerReq(
    WORD                *pMlength,
    BYTE                *pParamField,
    LOCSetServerReq     *pLOCSetServerReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_SETSERVER_TYPE,       &BuildTlvSetServerType },
        { eTLV_IPV4_ADDRESS,         &BuildTlvIPv4Address },
        { eTLV_IPV6_ADDRESS,         &BuildTlvIPv6Address },
        { eTLV_URL_ADDRESS,          &BuildTlvURLAddress },
        { eTLV_TYPE_INVALID,               NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };
    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pLOCSetServerReq,
                      map,
                      eQMI_LOC_SET_SERVER,
                      pMlength );

    return eRCode;
}


/******************************************************************************
 * Response handling
 ******************************************************************************

 * This function unpacks the eQMI_LOC_SET_SERVER response message
 * to a user-provided response structure.
 *
 * \param  pMdmResp  [IN]  - Pointer to packed response from the modem.
 *
 * \param  pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE  - on success;  eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiLocSetServerResp(
                  BYTE                         *pMdmResp,
                  struct QmiLocSetServerResp   *pApiResp)
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
                           eQMI_LOC_SET_SERVER );
    return eRCode;
}
