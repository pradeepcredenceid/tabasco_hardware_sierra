/*
 * \ingroup uim
 *
 * \file    qaUimSLQSUimReadRecord.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_UIM_READ_RECORD message.
 *
 * Copyright: � 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaUimSLQSUimReadRecord.h"
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
enum eQCWWANError BuildTlvReadRecordSessInfo( BYTE *pBuf, BYTE *pParam )
{
    UIMSessionInformation *pReq =
                       &(( UIMReadRecordReq *)pParam)->sessionInfo;

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
enum eQCWWANError BuildTlvReadRecordFileID( BYTE *pBuf, BYTE *pParam )
{
    fileInfo *pReq = &(( UIMReadRecordReq *)pParam)->fileIndex;
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
    for ( lCount=0 ; lCount < ((pReq->pathLen)/2) ; lCount++ )
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
 * This function packs the Read Record parameter to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvReadRecord( BYTE *pBuf, BYTE *pParam )
{
    readRecordInfo *pReq = &(( UIMReadRecordReq *)pParam)->readRecord;
    enum eQCWWANError eRCode;
    WORD              lCount;

    /* Check For Invalid Parameter */
    if ( NULL == pReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Insert Record Info */
    eRCode = PutWord( pBuf, pReq->record );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    eRCode = PutWord( pBuf, pReq->length );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    return eRCode;
}

/*
 * This function packs the Last Record parameter to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvLastRecord( BYTE *pBuf, BYTE *pParam )
{
    UIMReadRecordReq *pReq = ( UIMReadRecordReq *)pParam;
    enum eQCWWANError eRCode;
    WORD              lCount;

    /* Check For Invalid Parameter */
    if ( NULL == pReq->pLastRecord )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert Last Record */
    eRCode = PutWord( pBuf, *pReq->pLastRecord );

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
enum eQCWWANError BuildTlvReadRecordRespIndication( BYTE *pBuf, BYTE *pParam )
{
    UIMReadRecordReq *pReq = ( UIMReadRecordReq *)pParam;

    /* Check For Invalid Parameter */
    if ( NULL == pReq->pIndicationToken )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert indication token */
    return PutLong( pBuf, *pReq->pIndicationToken );
}
/*
 * This function packs the SLQS ReadRecord request parameters to the QMI message SDU.
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaUimSLQSUimReadRecord.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiUimSlqsReadRecord(
    WORD                    *pMlength,
    BYTE                    *pParamField,
    UIMReadRecordReq *pUIMReadRecordReq )
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_GET_RR_SESS_INFO, &BuildTlvReadRecordSessInfo },
        { eTLV_GET_RR_FILE_ID,   &BuildTlvReadRecordFileID },
        { eTLV_GET_READ_RECORD,  &BuildTlvReadRecord},
        { eTLV_GET_LAST_RECORD,  &BuildTlvLastRecord},
        { eTLV_GET_RR_RESP_IND,  &BuildTlvReadRecordRespIndication},
        { eTLV_TYPE_INVALID,     NULL } /* Important. Sentinel.
                                         * Signifies last item in map
                                         */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pUIMReadRecordReq,
                      map,
                      eQMI_UIM_READ_RECORD,
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
enum eQCWWANError UnpackTlvRRCardResult(
    BYTE *pTlvData,
    BYTE *pResp )
{
    cardResult *lResp = ((struct QmiUimSLQSReadRecordResp *)
                               pResp)->pUIMReadRecordResp->pCardResult;
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
enum eQCWWANError UnpackTlvRRReadResult(
    BYTE *pTlvData,
    BYTE *pResp )
{
    readResult *lResp = ((struct QmiUimSLQSReadRecordResp *)
                               pResp)->pUIMReadRecordResp->pReadResult;
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

/******************************************************************************
 * Response handling
 ******************************************************************************/
/*
 * This function unpacks the Additional read Result Values from the QMI response
 * message to a user provided response structure
 *
 * \param       pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param       pResp    [OUT]     - Pointer to structure containing storage
 *                                   to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 */
enum eQCWWANError UnpackTlvRRAdditionalReadResult(
    BYTE *pTlvData,
    BYTE *pResp )
{
    additionalReadResult *lResp = ((struct QmiUimSLQSReadRecordResp *)
                               pResp)->pUIMReadRecordResp->pAdditionalReadResult;
    enum eQCWWANError eRCode;
    BYTE              lcount;

    /* Check For Invalid Parameter */
    if ( NULL == lResp )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Copy the Additional read result values into the List */
    eRCode = GetWord( pTlvData, &lResp->additionalRecordLen );
    for ( lcount=0; lcount < lResp->additionalRecordLen ; lcount++ )
    {
         eRCode = GetByte( pTlvData, &lResp->additionalRecord[lcount]);
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
enum eQCWWANError UnpackTlvRRRespIndication(
    BYTE *pTlvData,
    BYTE *pResp )
{
    UIMReadRecordResp *lResp =
    ((struct QmiUimSLQSReadRecordResp *)pResp)->pUIMReadRecordResp;

    /* Check For Invalid Parameter */
    if ( NULL == lResp->pIndicationToken )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Copy the Indication Token into the List */
    return GetLong( pTlvData, lResp->pIndicationToken );
}

/*
 * This function unpacks the SLQS ReadRecord response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp           - Pointer to packed response from the modem.
 *
 * \param  pApiResp           - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiUimSlqsReadRecord(
    BYTE                                   *pMdmResp,
    struct QmiUimSLQSReadRecordResp *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,                 &qmUnpackTlvResultCode },
        { eTLV_GET_RR_CARD_RESULT,          &UnpackTlvRRCardResult },
        { eTLV_GET_READ_RESULT,             &UnpackTlvRRReadResult },
        { eTLV_GET_ADDITIONAL_READ_RESULT,  &UnpackTlvRRAdditionalReadResult},
        { eTLV_GET_RR_RESP_IND_RESP,        &UnpackTlvRRRespIndication},
        { eTLV_TYPE_INVALID,                NULL } /* Important. Sentinel.
                                             * Signifies last item in map.
                                             */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_UIM_READ_RECORD);
    return eRCode;
}


