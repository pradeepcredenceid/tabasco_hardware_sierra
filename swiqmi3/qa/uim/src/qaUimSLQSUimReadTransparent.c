/*
 * \ingroup uim
 *
 * \file    qaUimSLQSUimReadTransparen.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_UIM_READ_TRANSPARENT message.
 *
 * Copyright: � 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaUimSLQSUimReadTransparent.h"
#include "qaGobiApiUimCommon.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the Session Information parameter to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvReadTransparentSessInfo( BYTE *pBuf, BYTE *pParam )
{
    UIMSessionInformation *pReq =
                       &(( UIMReadTransparentReq *)pParam)->sessionInfo;

    /* Check For Invalid Parameter */
    if ( NULL == pReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Insert Session Information */
    return PackTlvCommonSessionInfo( pBuf, pReq );
}

/*
 * This function packs the File ID parameter to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvReadTransparentFileID( BYTE *pBuf, BYTE *pParam )
{
    fileInfo *pReq = &(( UIMReadTransparentReq *)pParam)->fileIndex;
    enum eQCWWANError eRCode;
    WORD              lCount;

    /* Check For Invalid Parameter */
    if ( NULL == pReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Insert File ID */
    eRCode = PutWord( pBuf, pReq->fileID );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert path length */
    eRCode = PutByte( pBuf, pReq->pathLen );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert path, as it is of type WORD so path length is divided by 2 */
    for ( lCount=0 ; lCount < (pReq->pathLen)/2 ; lCount++ )
    {
        eRCode = PutWord( pBuf, pReq->path[lCount] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
    }

    return eRCode;
}

/*
 * This function packs the read transparent parameter to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvReadTransparent( BYTE *pBuf, BYTE *pParam )
{
    readTransparentInfo *pReq = &(( UIMReadTransparentReq *)pParam)->readTransparent;
    enum eQCWWANError eRCode;
    WORD              lCount;

    /* Check For Invalid Parameter */
    if ( NULL == pReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Insert offset for the read operation */
    eRCode = PutWord( pBuf, pReq->offset );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert content */
    eRCode = PutWord( pBuf, pReq->length );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    return eRCode;
}

/*
 * This function packs the Response Indication parameter to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvReadTransparentRespIndication( BYTE *pBuf, BYTE *pParam )
{
    UIMReadTransparentReq *pReq = ( UIMReadTransparentReq *)pParam;

    /* Check For Invalid Parameter */
    if ( NULL == pReq->pIndicationToken )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert indication token */
    return PutLong( pBuf, *pReq->pIndicationToken );
}

/*
 * This function packs the Encrypt data parameter to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvEncryptData( BYTE *pBuf, BYTE *pParam )
{
    UIMReadTransparentReq *pReq = ( UIMReadTransparentReq *)pParam;
    enum eQCWWANError eRCode;
    WORD              lCount;

    /* Check For Invalid Parameter */
    if ( NULL == pReq->pEncryptData )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert encrypt data */
    eRCode = PutByte( pBuf, *pReq->pEncryptData );

    return eRCode;
}

/*
 * This function packs the SLQS ReadTransparent request parameters to the QMI message SDU.
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaUimSLQSUimReadTransparent.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiUimSlqsReadTransparent(
    WORD                    *pMlength,
    BYTE                    *pParamField,
    UIMReadTransparentReq *pUIMReadTransparentReq )
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_GET_RT_SESS_INFO,     &BuildTlvReadTransparentSessInfo },
        { eTLV_GET_RT_FILE_ID,       &BuildTlvReadTransparentFileID },
        { eTLV_GET_READ_TRANSPARENT, &BuildTlvReadTransparent },
        { eTLV_GET_RT_RESP_IND,      &BuildTlvReadTransparentRespIndication },
        { eTLV_GET_RT_ENCRYPT_DATA,  &BuildTlvEncryptData },
        { eTLV_TYPE_INVALID,         NULL } /* Important. Sentinel.
                                             * Signifies last item in map
                                             */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pUIMReadTransparentReq,
                      map,
                      eQMI_UIM_READ_TRANSPARENT,
                      pMlength );
    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the Card Result Values from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvRTCardResult(
    BYTE *pTlvData,
    BYTE *pResp )
{
    cardResult *lResp = ((struct QmiUimSLQSReadTransparentResp *)
                               pResp)->pUIMReadTransparentResp->pCardResult;
    enum eQCWWANError eRCode;

    /* Check For Invalid Parameter */
    if ( NULL == lResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Copy the card result values into the List */
    eRCode = GetByte( pTlvData, &lResp->sw1 );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    return GetByte( pTlvData, &lResp->sw2 );
}

/******************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the Read Result Values from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvRTReadResult(
    BYTE *pTlvData,
    BYTE *pResp )
{
    readResult *lResp = ((struct QmiUimSLQSReadTransparentResp *)
                               pResp)->pUIMReadTransparentResp->pReadResult;
    enum eQCWWANError eRCode;
    BYTE              lcount;

    /* Check For Invalid Parameter */
    if ( NULL == lResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Copy the read result values into the List */
    eRCode = GetWord( pTlvData, &lResp->contentLen );
    for ( lcount=0; lcount < lResp->contentLen ; lcount++ )
    {
         eRCode = GetByte( pTlvData, &lResp->content[lcount] );
         if ( eQCWWAN_ERR_NONE != eRCode )
         {
             return eRCode;
         }
    }

    return eRCode;
}

/*
 * This function unpacks the Indication Token from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvRTRespIndication(
    BYTE *pTlvData,
    BYTE *pResp )
{
    UIMReadTransparentResp *lResp =
    ((struct QmiUimSLQSReadTransparentResp *)pResp)->pUIMReadTransparentResp;

    /* Check For Invalid Parameter */
    if ( NULL == lResp->pIndicationToken )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Copy the Indication Token into the List */
    return GetLong( pTlvData, lResp->pIndicationToken );
}

/******************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the Encrypted data from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvRTEncryptedData(
    BYTE *pTlvData,
    BYTE *pResp )
{
    UIMReadTransparentResp *lResp = ((struct QmiUimSLQSReadTransparentResp *)
	                               pResp)->pUIMReadTransparentResp;

    enum eQCWWANError eRCode;
    BYTE              lcount;

    /* Check For Invalid Parameter */
    if ( NULL == lResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Copy the encrypted data into the List */
    eRCode = GetByte( pTlvData, lResp->pEncryptedData );
    return eRCode;
}

/*
 * This function unpacks the SLQS ReadTransparent response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp           - Pointer to packed response from the modem.
 *
 * \param  pApiResp           - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiUimSlqsReadTransparent(
    BYTE                                   *pMdmResp,
    struct QmiUimSLQSReadTransparentResp *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,                 &qmUnpackTlvResultCode },
        { eTLV_GET_RT_CARD_RESULT,          &UnpackTlvRTCardResult },
        { eTLV_GET_RT_RESULT,               &UnpackTlvRTReadResult },
        { eTLV_GET_RT_RESP_IND_RESP,        &UnpackTlvRTRespIndication},
        { eTLV_GET_RT_ENCRYPTED_DATA,       &UnpackTlvRTEncryptedData},
        { eTLV_TYPE_INVALID,                NULL } /* Important. Sentinel.
                                             * Signifies last item in map.
                                             */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_UIM_READ_TRANSPARENT);
    return eRCode;
}


