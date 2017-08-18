/*
 * \ingroup : loc
 *
 * \file    : qaSlqsLocStart.c
 *
 * \brief   : Contains Packing and UnPacking routines for the
 *            eQMI_LOC_START message.
 *
 * Copyright: © 2011-2015 Sierra Wireless, Inc. all rights reserved
 */

/* include files */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaSlqsLocStart.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/

/*
 * This function packs the LOC Start request Session Id  field
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvStartSessionId( BYTE *pBuf, BYTE *pParam )
{

    LOCStartReq *lReq = (LOCStartReq *)pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if(lReq==NULL)
        return eQCWWAN_ERR_INVALID_ARG;
    /* Insert Session Id */
    eRCode = PutByte( pBuf, lReq->SessionId );
    return eRCode;
}

/*
 * This function packs the LOC Start request Recurrence Type field
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvRecurrenceType( BYTE *pBuf, BYTE *pParam )
{
    LOCStartReq *lReq = (LOCStartReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    
    if(lReq==NULL)
        return eQCWWAN_ERR_INVALID_ARG;

    if( NULL == lReq->pRecurrenceType )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert Recurrence Type */
    eRCode = PutLong( pBuf, *(lReq->pRecurrenceType));
    return eRCode;
}

/*
 * This function packs the LOC Start request Horizontal Accuracy field
 * to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvHorizontalAccuracy( BYTE *pBuf, BYTE *pParam )
{
   
    LOCStartReq *lReq = (LOCStartReq *)pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    
    if(lReq==NULL)
        return eQCWWAN_ERR_INVALID_ARG;

    if( NULL == lReq->pHorizontalAccuracyLvl )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert Horizontal Accuracy Level */
    eRCode = PutLong( pBuf, *(lReq->pHorizontalAccuracyLvl));
    return eRCode;
}

/*
 * This function packs the LOC Start request Intermediate Report State
 *  field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvIntermediateRptState( BYTE *pBuf, BYTE *pParam )
{
    LOCStartReq *lReq = (LOCStartReq *)pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if( NULL == lReq->pIntermediateReportState )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert Intermediate Report State */
    eRCode = PutLong( pBuf, *(lReq->pIntermediateReportState));
    return eRCode;
}

/*
 * This function packs the LOC Start request Minimum Interval
 *  field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvMinimumInterval( BYTE *pBuf, BYTE *pParam )
{

    LOCStartReq *lReq = (LOCStartReq *)pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if(lReq==NULL)
        return eQCWWAN_ERR_INVALID_ARG;

    if( NULL == lReq->pMinIntervalTime )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert Minimum Interval Time */
    eRCode = PutLong( pBuf, *(lReq->pMinIntervalTime));
    return eRCode;
}

/*
 * This function packs the LOC start request Application Parameters
 * field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvAppParameters( BYTE *pBuf, BYTE *pParam )
{

    LOCStartReq *lReq = (LOCStartReq *)pParam;

    BYTE lCount = 0;
    struct LocApplicationInfo *pApplicationInfo;
    enum   eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    
    if(lReq==NULL)
        return eQCWWAN_ERR_INVALID_ARG;

    pApplicationInfo = lReq->pApplicationInfo;
    
    if( NULL == pApplicationInfo )
    {
        return eQCWWAN_ERR_NONE;
    }


    
    /* Insert Application parameter - Application Provider Length */
    eRCode = PutByte( pBuf, pApplicationInfo->appProviderLength);
    if( eQCWWAN_ERR_NONE != eRCode )
    {
        return eRCode;
    }

    /* Insert Application parameter - Application Provider  */
    while( lCount < pApplicationInfo->appProviderLength )
    {
    	eRCode = PutByte( pBuf, pApplicationInfo->pAppProvider[lCount] );
        if( eQCWWAN_ERR_NONE != eRCode )
        {
            return eRCode;
        }
        lCount++;
    }

    /* Insert Application parameter - Application Name Length */
       eRCode = PutByte( pBuf, pApplicationInfo->appNameLength);
       if( eQCWWAN_ERR_NONE != eRCode )
       {
           return eRCode;
       }

       lCount =0;
       /* Insert Application parameter - Application Name  */
       while( lCount < pApplicationInfo->appNameLength )
       {
       	eRCode = PutByte( pBuf, pApplicationInfo->pAppName[lCount] );
           if( eQCWWAN_ERR_NONE != eRCode )
           {
               return eRCode;
           }
           lCount++;
       }

       /* Insert Application parameter - Application Provider Length */
       eRCode = PutByte( pBuf, pApplicationInfo->appVersionValid);
       if( eQCWWAN_ERR_NONE != eRCode )
       {
           return eRCode;
       }

       /* Insert Application parameter - Application Version Length */
          eRCode = PutByte( pBuf, pApplicationInfo->appVersionLength);
          if( eQCWWAN_ERR_NONE != eRCode )
          {
              return eRCode;
          }

          lCount =0;
          /* Insert Application parameter - Application Version  */
          while( lCount < pApplicationInfo->appVersionLength )
          {
          	eRCode = PutByte( pBuf, pApplicationInfo->pAppVersion[lCount] );
              if( eQCWWAN_ERR_NONE != eRCode )
              {
                  return eRCode;
              }
              lCount++;
          }
    return eRCode;
}

/*
 * This function packs the LOC Start request Configure Altitude Assumed
 *  field to the QMI message SDU
 *
 * \param pBuf   - Pointer to storage into which the packed
 *                 data will be placed by this function.
 *
 * \param pParam - Pointer to structure containing data for this TLV.
 *
 */
enum eQCWWANError BuildTlvConfigAltitudeAsmd( BYTE *pBuf, BYTE *pParam )
{
    LOCStartReq *lReq = (LOCStartReq *)pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if(lReq==NULL)
        return eQCWWAN_ERR_INVALID_ARG;
    
    if( NULL == lReq->pConfigAltitudeAssumed )
    {
        return eQCWWAN_ERR_NONE;
    }

    /* Insert Altitude Assumed info */
    eRCode = PutLong( pBuf, *(lReq->pConfigAltitudeAssumed));
    return eRCode;
}

/*
 * Packs the PkQmiLocStartReq parameters to the QMI message SDU
 *
 * \param  pParamField[OUT]         - Pointer to storage into which the packed
 *                                    data will be placed by this function.
 *
 * \param  pMlength[OUT]            - Total length of built message.
 *
 * \param  pLOCStartReqReq[IN]     - LOC Start Request Parameters
 *
 * \return  eQCWWAN_ERR_NONE on success, eQCWWAN_ERR_XXX on error
 *
 */
enum eQCWWANError PkQmiLocStartReq(
    WORD                *pMlength,
    BYTE                *pParamField,
    LOCStartReq         *pLOCStartReq )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_START_SESSION_ID,          &BuildTlvStartSessionId },
        { eTLV_RECURRENCE_TYPE,           &BuildTlvRecurrenceType },
        { eTLV_HORIZONTAL_ACCURACY,       &BuildTlvHorizontalAccuracy },
        { eTLV_INTERMEDIATE_RPT_STATE,    &BuildTlvIntermediateRptState },
        { eTLV_MINIMUM_INTERVAL,          &BuildTlvMinimumInterval },
        { eTLV_APP_PARAMENTERS,           &BuildTlvAppParameters },
        { eTLV_CONFIG_ALTITUDE_ASMD,      &BuildTlvConfigAltitudeAsmd },
        { eTLV_TYPE_INVALID,               NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };
    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pLOCStartReq,
                      map,
                      eQMI_LOC_START,
                      pMlength );

    return eRCode;
}


/******************************************************************************
 * Response handling
 ******************************************************************************

 * This function unpacks the eQMI_LOC_START response message
 * to a user-provided response structure.
 *
 * \param  pMdmResp  [IN]  - Pointer to packed response from the modem.
 *
 * \param  pApiResp  [OUT] - Pointer to storage to unpack into.
 *
 * \return eQCWWAN_ERR_NONE  - on success;  eQCWWAN_ERR_XXX otherwise
 *
 */
enum eQCWWANError UpkQmiLocStartResp(
                  BYTE                         *pMdmResp,
                  struct QmiLocStartResp       *pApiResp)
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
                           eQMI_LOC_START );
    return eRCode;
}
