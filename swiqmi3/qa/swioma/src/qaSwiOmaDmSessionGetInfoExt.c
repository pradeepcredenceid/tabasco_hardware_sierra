/*
 * \ingroup swioma
 *
 * \file    qaSwiOmaDmSessionGetInfoExt.c
 *
 * \brief   Contains Packing and UnPacking routines for the
 *          QMI_SWIOMA_DM_SESSION_GET_INFO message.
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaSwiOmaDmSessionGetInfoExt.h"
#include "qaGobiApiSwiOmadmsExt.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/
/*
 * This function packs the SLQSOMADMGetSessionInfoExt parameters to the QMI message
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
enum eQCWWANError PkQmiSwiOmaDmSessionGetInfoExt(
    WORD  *pMlength,
    BYTE  *pParamField )
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map.
                                     */
    };

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = qmbuild( pParamField,
                      NULL,
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
enum eQCWWANError UnpackTlvSessionInfoExt( BYTE* pTlvData, BYTE *pResp )
{
    struct QmiSwiOmaDmSessionGetInfoExtResp *lResp =
        (struct QmiSwiOmaDmSessionGetInfoExtResp *)pResp;

    SLQSOMADMSessionInfoExt *tResp =
            (SLQSOMADMSessionInfoExt *)lResp->pResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    BYTE              lByte  = 0;
    WORD              lWord = 0;
    BYTE              lCtr = 0;
    ULONG             lUlong = 0;

    if ( NULL == tResp->pOMAModInfoResult    ||
         NULL == tResp->pSessionState        ||
         NULL == tResp->pHFAStatus           ||
         NULL == tResp->pHFAMaxretryTime     ||
         NULL == tResp->pRetryInterval       ||
         NULL == tResp->pRetryingTimes       ||
         NULL == tResp->pFUMOState           ||
         NULL == tResp->pFUMOResultCode      ||
         NULL == tResp->pPkgVendorNameLength ||
         NULL == tResp->pPkgVendorName       ||
         NULL == tResp->pPkgSize             ||
         NULL == tResp->pPkgVersionLength    ||
         NULL == tResp->pPkgVersion          ||
         NULL == tResp->pPkgNameLength       ||
         NULL == tResp->pPkgName             ||
         NULL == tResp->pPkgDescLength       ||
         NULL == tResp->pPkgDescription      ||
         NULL == tResp->pPkgDateLength       ||
         NULL == tResp->pPkgDate             ||
         NULL == tResp->pPkgInstallTimeLength||
         NULL == tResp->pPkgInstallTime )
    {
         return eQCWWAN_ERR_INVALID_ARG;
    }

    /* Get pStatus */
    eRCode = GetByte( pTlvData, &lByte );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    *(tResp->pOMAModInfoResult) = lByte;

    /* Get pSessionState */
    eRCode = GetByte( pTlvData, &lByte );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    *(tResp->pSessionState) = lByte;

    /* Get pHFAStatus */
    eRCode = GetByte( pTlvData, &lByte );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    *(tResp->pHFAStatus) = lByte;

    /* Get pHFAMaxretryTime */
    eRCode = GetWord( pTlvData, &lWord );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    *(tResp->pHFAMaxretryTime) = lWord;

    /* Get pRetryInterval */
    eRCode = GetWord( pTlvData, &lWord );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    *(tResp->pRetryInterval) = lWord;

    /* Get pRetryingTimes */
    eRCode = GetWord( pTlvData, &lWord );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    *(tResp->pRetryingTimes) = lWord;

    /* Get pFUMOState */
    eRCode = GetByte( pTlvData, &lByte );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    *(tResp->pFUMOState) = lByte;


    /* Get pFUMOResultCode */
    eRCode = GetLong( pTlvData, &lUlong );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    *(tResp->pFUMOResultCode) = lUlong;

    /* Get pPkgVendorNameLengthh */
    eRCode = GetWord( pTlvData, &lWord );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    /* Check for buffer size error */
    if ( *(tResp->pPkgVendorNameLength) < lWord )
    {
        *(tResp->pPkgVendorNameLength) = lWord;
        return(eQCWWAN_ERR_BUFFER_SZ);
    }
    *(tResp->pPkgVendorNameLength) = lWord;

    /* Get pPkgVendorName */
    for( lCtr = 0; lCtr < (*(tResp->pPkgVendorNameLength)); lCtr++ )
    {
        eRCode = GetByte( pTlvData, &lByte );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        tResp->pPkgVendorName[lCtr] = lByte;
    }

    /* Get pPkgSize */
    eRCode = GetLong( pTlvData, &lUlong );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    *(tResp->pPkgSize) = lUlong;

    /* Get pPkgVersionLength */
    eRCode = GetWord( pTlvData, &lWord );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    /* Check for buffer size error */
    if ( *(tResp->pPkgVersionLength) < lWord )
    {
        *(tResp->pPkgVersionLength) = lWord;
        return(eQCWWAN_ERR_BUFFER_SZ);
    }
    *(tResp->pPkgVersionLength) = lWord;

    /* Get pPkgVersion */
    for( lCtr = 0; lCtr < (*(tResp->pPkgVersionLength)); lCtr++ )
    {
        eRCode = GetByte( pTlvData, &lByte );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        tResp->pPkgVersion[lCtr] = lByte;
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

    /* Get pPkgDateLength */
    eRCode = GetWord( pTlvData, &lWord );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        *(tResp->pPkgDateLength) = lWord;
        return eRCode;
    }
    /* Check for buffer size error */
    if ( *(tResp->pPkgDateLength) < lWord )
    {
        return(eQCWWAN_ERR_BUFFER_SZ);
    }
    *(tResp->pPkgDateLength) = lWord;

    /* Get pPkgDate */
    for( lCtr = 0; lCtr < *(tResp->pPkgDateLength); lCtr++ )
    {
        eRCode = GetByte( pTlvData, &lByte );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        tResp->pPkgDate[lCtr] = lByte;
    }

    /* Get pPkgInstallTimeLength */
    eRCode = GetWord( pTlvData, &lWord );
    if ( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }
    /* Check for buffer size error */
    if ( *(tResp->pPkgInstallTimeLength) < lWord )
    {
        *(tResp->pPkgInstallTimeLength) = lWord;
        return(eQCWWAN_ERR_BUFFER_SZ);
    }
    *(tResp->pPkgInstallTimeLength) = lWord;

    /* Get pPkgInstallTime */
    for( lCtr = 0; lCtr < *(tResp->pPkgInstallTimeLength); lCtr++ )
    {
        eRCode = GetByte( pTlvData, &lByte );
        if ( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        tResp->pPkgInstallTime[lCtr] = lByte;
    }

    return eRCode;
}

/*
 * This function unpacks the SLQSOMADMGetSessionInfoExt response message to a
 * user-provided response structure.
 *
 * \param  pMdmResp     [IN]  - Pointer to packed response from the modem.
 *
 * \param  pApiResp     [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiSwiOmaDmSessionGetInfoExt(
    BYTE                                 *pMdmResp,
    struct QmiSwiOmaDmSessionGetInfoExtResp *pApiResp )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,             &qmUnpackTlvResultCode },
        { eTLV_GET_OMA_DM_SESSION_INFO_EXT,     &UnpackTlvSessionInfoExt },
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
