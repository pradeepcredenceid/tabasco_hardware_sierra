/*
 * \ingroup uim
 *
 * \file    qaUimSLQSUimWriteRecord.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_UIM_WRITE_RECORD message.
 *
 * Copyright: � 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaUimSLQSUimGetFileAttributes.h"
#include "qaUimSLQSUimWriteRecord.h"
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
enum eQCWWANError BuildTlvWriteRecordSessInfo( BYTE *pBuf, BYTE *pParam )
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
enum eQCWWANError BuildTlvWriteRecordFileID( BYTE *pBuf, BYTE *pParam )
{
    fileInfo *pReq = &(( UIMWriteRecordReq *)pParam)->fileIndex;
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
    for ( lCount=0 ; lCount < ((pReq->pathLen)/2)  ; lCount++ )
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
 * This function packs the Write Record parameter to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvWriteRecord( BYTE *pBuf, BYTE *pParam )
{
    writeRecordInfo *pReq = &(( UIMWriteRecordReq *)pParam)->writeRecord;
    enum eQCWWANError eRCode;
    WORD              lcount;

    /* Check For Invalid Parameter */
    if ( NULL == pReq )
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    if (pReq->dataLen >= MAX_DESCRIPTION_LENGTH)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    } 

    /* Insert record info */
    eRCode = PutWord( pBuf, pReq->record );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    eRCode = PutWord( pBuf, pReq->dataLen );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    for ( lcount=0; lcount < pReq->dataLen ; lcount++ )
    {
         eRCode = PutByte( pBuf, pReq->data[lcount] );
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
enum eQCWWANError BuildTlvWriteRecordRespIndication( BYTE *pBuf, BYTE *pParam )
{
    UIMWriteRecordReq *pReq = ( UIMWriteRecordReq *)pParam;

    /* Check For Invalid Parameter */
    if ( NULL == pReq->pIndicationToken )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert Indication Information */
    return PutLong( pBuf, *pReq->pIndicationToken );
}
/*
 * This function packs the SLQS WriteRecord request parameters to the QMI message SDU.
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaUimSLQSUimGetFileAttributes.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiUimSlqsWriteRecord(
    WORD                    *pMlength,
    BYTE                    *pParamField,
    UIMWriteRecordReq *pUIMWriteRecordReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_WRITE_RECORD_SESS_INFO, &BuildTlvWriteRecordSessInfo },
        { eTLV_WRITE_RECORD_FILE_ID,   &BuildTlvWriteRecordFileID },
        { eTLV_WRITE_RECORD_INFO,      &BuildTlvWriteRecord},
        { eTLV_WRITE_RECORD_RESP_IND,  &BuildTlvWriteRecordRespIndication},
        { eTLV_TYPE_INVALID,           NULL } /* Important. Sentinel.
                                         * Signifies last item in map
                                         */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pUIMWriteRecordReq,
                      map,
                      eQMI_UIM_WRITE_RECORD,
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
enum eQCWWANError UnpackTlvWRCardResult(
    BYTE *pTlvData,
    BYTE *pResp )
{
    cardResult *lResp = ((struct QmiUimSLQSWriteRecordResp *)
                               pResp)->pUIMWriteRecordResp->pCardResult;
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
enum eQCWWANError UnpackTlvWRRespIndication(
    BYTE *pTlvData,
    BYTE *pResp )
{
    UIMWriteRecordResp *lResp =
    ((struct QmiUimSLQSWriteRecordResp *)pResp)->pUIMWriteRecordResp;

    /* Check For Invalid Parameter */
    if ( NULL == lResp->pIndicationToken )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Copy the Indication Token into the List */
    return GetLong( pTlvData, lResp->pIndicationToken );
}

/*
 * This function unpacks the SLQS WriteRecord response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp           - Pointer to packed response from the modem.
 *
 * \param  pApiResp           - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiUimSlqsWriteRecord(
    BYTE                                   *pMdmResp,
    struct QmiUimSLQSWriteRecordResp *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,                  &qmUnpackTlvResultCode },
        { eTLV_WRITE_RECORD_CARD_RESULT,     &UnpackTlvWRCardResult },
        { eTLV_WRITE_RECORD_RESP_IND_RESULT, &UnpackTlvWRRespIndication},
        { eTLV_TYPE_INVALID,                 NULL } /* Important. Sentinel.
                                                     * Signifies last item in map.
                                                     */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_UIM_WRITE_RECORD);
    return eRCode;
}


