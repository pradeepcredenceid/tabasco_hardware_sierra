/*
 * \ingroup swioma
 *
 * \file    qaSwiOmaDmSessionGetInfo.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_SWIOMA_DM_SESSION_GET_INFO message.
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc. all rights reserved
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaSwiOmaDmSessionGetInfo.h"
#include "qaGobiApiSwiOmadms.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the Session Type to the QMI message SDU.
 *
 * \param  pBuf   [OUT] - Pointer to storage into which the packed
 *                        data will be placed by this function.
 *
 * \param  pParam [IN]  - Pointer to structure containing data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE, on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError BuildTlvGetSessionInfo( BYTE *pBuf, BYTE *pParam )
{
    struct QmiSwiOmaDmSessionGetInfoReq *pReq =
           ( struct QmiSwiOmaDmSessionGetInfoReq * )pParam;

    /* Insert Session Type */
    return PutByte( pBuf, *(pReq->pSessionType) );
}

/*
 * This function packs the SLQSOMADMGetSessionInfo parameters to the QMI message
 * SDU
 *
 * \param  pMlength     [OUT] - Total length of built message.
 *
 * \param  pParamField  [OUT] - Pointer to storage into which the packed
 *                              data will be placed by this function.
 *
 * \return eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX otherwise
 *
 * \sa qaGobiApiSwiOmadms.h for remaining parameter descriptions.
 */
enum eQCWWANError PkQmiSwiOmaDmSessionGetInfo(
    WORD  *pMlength,
    BYTE  *pParamField,
    ULONG *pSessionType)
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_GET_SESSION_INFO_TYPE, &BuildTlvGetSessionInfo },
        { eTLV_TYPE_INVALID,          NULL } /* Important. Sentinel.
                                              * Signifies last item in map.
                                              */
    };
    struct QmiSwiOmaDmSessionGetInfoReq req;
    slmemset( (char *)&req,
              0,
              sizeof (struct QmiSwiOmaDmSessionGetInfoReq) );

    req.pSessionType = (BYTE *)pSessionType;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      (BYTE*)&req,
                      map,
                      eQMI_SWIOMA_GET_SESSION_INFO,
                      pMlength );
    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************/

/*
 * This function unpacks the SLQSOMADMGetSessionInfo Session Info from the
 * QMI response message to a user provided response structure
 *
 * \param  pTlvData [IN/OUT]  - Pointer to TLV data from which to unpack.
 *
 * \param  pResp    [OUT]     - Pointer to structure containing storage
 *                              to return data for this TLV.
 *
 * \return eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UnpackTlvSessionInfo( BYTE* pTlvData, BYTE *pResp )
{
    struct QmiSwiOmaDmSessionGetInfoResp *lResp =
        (struct QmiSwiOmaDmSessionGetInfoResp *)pResp;

    SLQSOMADMSessionInfo *tResp =
            (SLQSOMADMSessionInfo *)lResp->pResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lByte  = 0;
    WORD              lWord = 0;
    BYTE              lCtr = 0;

    if ( NULL == tResp->pStatus               ||
         NULL == tResp->pUpdateCompleteStatus ||
         NULL == tResp->pSeverity             ||
         NULL == tResp->pSourceLength         ||
         NULL == tResp->pSource               ||
         NULL == tResp->pPkgNameLength        ||
         NULL == tResp->pPkgName              ||
         NULL == tResp->pPkgDescLength        ||
         NULL == tResp->pPkgDescription       ||
         NULL == tResp->pDateLength           ||
         NULL == tResp->pDate                 ||
         NULL == tResp->pTimeLength           ||
         NULL == tResp->pTime                 ||
         NULL == tResp->pSessionType          ||
         NULL == tResp->pSessionState         ||
         NULL == tResp->pRetryCount) {
         return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Get pStatus */
    eRCode = GetByte( pTlvData, &lByte );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    *(tResp->pStatus) = lByte;

    /* Get pUpdateCompleteStatus */
    eRCode = GetWord( pTlvData, &lWord );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    *(tResp->pUpdateCompleteStatus) = lWord;

    /* Get pSeverity */
    eRCode = GetByte( pTlvData, &lByte );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    *(tResp->pSeverity) = lByte;

    /* Get pSourceLength */
    eRCode = GetWord( pTlvData, &lWord );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    /* Check for buffer size error */
    if ( *(tResp->pSourceLength) < lWord )
    {
        *(tResp->pSourceLength) = lWord;
        return(eQCWWAN_ERR_BUFFER_SZ);
    }
    *(tResp->pSourceLength) = lWord;

    /* Get pSource */
    for( lCtr = 0; lCtr < *(tResp->pSourceLength); lCtr++ )
    {
        eRCode = GetByte( pTlvData, &lByte );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        tResp->pSource[lCtr] = lByte;
    }

    /* Get pPkgNameLength */
    eRCode = GetWord( pTlvData, &lWord );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    /* Check for buffer size error */
    if ( *(tResp->pPkgNameLength) < lWord )
    {
        *(tResp->pPkgNameLength) = lWord;
        return(eQCWWAN_ERR_BUFFER_SZ);
    }
    *(tResp->pPkgNameLength) = lWord;

    /* Get pPkgName */
    for( lCtr = 0; lCtr < *(tResp->pPkgNameLength); lCtr++ )
    {
        eRCode = GetByte( pTlvData, &lByte );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        tResp->pPkgName[lCtr] = lByte;
    }

    /* Get pPkgDescLength */
    eRCode = GetWord( pTlvData, &lWord );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    /* Check for buffer size error */
    if ( *(tResp->pPkgDescLength) < lWord )
    {
        *(tResp->pPkgDescLength) = lWord;
        return(eQCWWAN_ERR_BUFFER_SZ);
    }
    *(tResp->pPkgDescLength) = lWord;

    /* Get pPkgDescription */
    for( lCtr = 0; lCtr < *(tResp->pPkgDescLength); lCtr++ )
    {
        eRCode = GetByte( pTlvData, &lByte );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        tResp->pPkgDescription[lCtr] = lByte;
    }

    /* Get pDateLength */
    eRCode = GetWord( pTlvData, &lWord );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        *(tResp->pDateLength) = lWord;
        return eRCode;
    }
    /* Check for buffer size error */
    if ( *(tResp->pDateLength) < lWord )
    {
        return(eQCWWAN_ERR_BUFFER_SZ);
    }
    *(tResp->pDateLength) = lWord;

    /* Get pDate */
    for( lCtr = 0; lCtr < *(tResp->pDateLength); lCtr++ )
    {
        eRCode = GetByte( pTlvData, &lByte );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        tResp->pDate[lCtr] = lByte;
    }

    /* Get pTimeLength */
    eRCode = GetWord( pTlvData, &lWord );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    /* Check for buffer size error */
    if ( *(tResp->pTimeLength) < lWord )
    {
        *(tResp->pTimeLength) = lWord;
        return(eQCWWAN_ERR_BUFFER_SZ);
    }
    *(tResp->pTimeLength) = lWord;

    /* Get pTime */
    for( lCtr = 0; lCtr < *(tResp->pTimeLength); lCtr++ )
    {
        eRCode = GetByte( pTlvData, &lByte );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        tResp->pTime[lCtr] = lByte;
    }

    /* Get pSessionType */
    eRCode = GetByte( pTlvData, &lByte );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    *(tResp->pSessionType) = lByte;

    /* Get pSessionState */
    eRCode = GetByte( pTlvData, &lByte );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    *(tResp->pSessionState) = lByte;

    /* Get pRetryCount */
    eRCode = GetByte( pTlvData, &lByte );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    *(tResp->pRetryCount) = lByte;

    return eRCode;
}

/*
 * This function unpacks the SLQSOMADMGetSessionInfo response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp     [IN]  - Pointer to packed response from the modem.
 *
 * \param  pApiResp     [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiSwiOmaDmSessionGetInfo(
    BYTE                                 *pMdmResp,
    struct QmiSwiOmaDmSessionGetInfoResp *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,             &qmUnpackTlvResultCode },
        { eTLV_GET_OMA_DM_SESSION_INFO, &UnpackTlvSessionInfo },
        { eTLV_TYPE_INVALID,            NULL } /* Important. Sentinel.
                                                * Signifies last item in map.
                                                */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_SWIOMA_GET_SESSION_INFO );
    return eRCode;
}
