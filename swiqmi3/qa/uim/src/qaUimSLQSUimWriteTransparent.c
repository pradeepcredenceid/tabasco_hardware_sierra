/*
 * \ingroup uim
 *
 * \file    qaUimSLQSUimWriteTransparent.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_UIM_WRITE_TRANSPARENT message.
 *
 * Copyright: © 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaUimSLQSUimWriteTransparent.h"
#include "qaGobiApiUimCommon.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the Session Information parameters to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvWriteTransSessInfo( BYTE *pBuf, BYTE *pParam )
{
    UIMSessionInformation *pReq = &(( UIMWriteTransparentReq *)pParam)->sessionInfo;

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
enum eQCWWANError BuildTlvWriteTransFileId( BYTE *pBuf, BYTE *pParam )
{
    fileInfo *pReq = &(( UIMWriteTransparentReq *)pParam)->fileIndex;
    enum eQCWWANError eRCode;
    WORD              lCount;

    /* Check For Invalid Parameter */
    if ( NULL == pReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Insert File ID Parameter */
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
 * This function packs the Write Transparent data parameters to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvWriteTransData( BYTE *pBuf, BYTE *pParam )
{
    writetransparentInfo *pReq = &(( UIMWriteTransparentReq *)pParam)->writeTransparent;
    enum eQCWWANError eRCode;
    WORD              lCount;

    /* Check For Invalid Parameter */
    if ( NULL == pReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    if (pReq->dataLen >= MAX_CONTENT_LENGTH)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Insert Write Transparent Data */
    eRCode = PutWord( pBuf, pReq->offset );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    eRCode = PutWord( pBuf, pReq->dataLen );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    for ( lCount=0 ; lCount < pReq->dataLen ; lCount++ )
    {
        eRCode = PutByte( pBuf, pReq->data[lCount] );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
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
enum eQCWWANError BuildTlvWriteTransRespIndication( BYTE *pBuf, BYTE *pParam )
{
    UIMWriteTransparentReq *pReq = ( UIMWriteTransparentReq *)pParam;

    /* Check For Invalid Parameter */
    if ( NULL == pReq->pIndicationToken )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert Indication Information */
    return PutLong( pBuf, *pReq->pIndicationToken );
}

/*
 * This function packs the SLQS Write Transparent request parameters to the QMI message SDU.
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaUimSLQSUimWriteTransparent.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiUimSlqsWriteTransparent(
    WORD               *pMlength,
    BYTE               *pParamField,
    UIMWriteTransparentReq *pUIMWriteTransparentReq )
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_GET_WT_SESS_INFO,      &BuildTlvWriteTransSessInfo },
        { eTLV_GET_WT_FILE_ID,        &BuildTlvWriteTransFileId },
        { eTLV_GET_WRITE_TRANSPARENT, &BuildTlvWriteTransData },
        { eTLV_GET_WT_RESP_IND,       &BuildTlvWriteTransRespIndication },
        { eTLV_TYPE_INVALID,          NULL } /* Important. Sentinel.
                                              * Signifies last item in map
                                              */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pUIMWriteTransparentReq,
                      map,
                      eQMI_UIM_WRITE_TRANSPARENT,
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
enum eQCWWANError UnpackTlvWriteTransCardResult(
    BYTE *pTlvData,
    BYTE *pResp )
{
    cardResult *lResp = ((struct QmiUimSLQSWriteTransparentResp *)
                               pResp)->pUIMWriteTransparentResp->pCardResult;
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
enum eQCWWANError UnpackTlvWriteTransRespIndication(
    BYTE *pTlvData,
    BYTE *pResp )
{
    UIMWriteTransparentResp *lResp =
    ((struct QmiUimSLQSWriteTransparentResp *)pResp)->pUIMWriteTransparentResp;

    /* Check For Invalid Parameter */
    if ( NULL == lResp->pIndicationToken )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Copy the Indication Token into the List */
    return GetLong( pTlvData, lResp->pIndicationToken );
}

/*
 * This function unpacks the SLQS Write Transparent response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp           - Pointer to packed response from the modem.
 *
 * \param  pApiResp           - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiUimSlqsWriteTransparent(
    BYTE                                   *pMdmResp,
    struct QmiUimSLQSWriteTransparentResp *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,        &qmUnpackTlvResultCode },
        { eTLV_WT_CARD_RESULT,     &UnpackTlvWriteTransCardResult },
        { eTLV_WT_RESP_IND_RESP,   &UnpackTlvWriteTransRespIndication },
        { eTLV_TYPE_INVALID,       NULL } /* Important. Sentinel.
                                           * Signifies last item in map.
                                           */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_UIM_WRITE_TRANSPARENT );
    return eRCode;
}
