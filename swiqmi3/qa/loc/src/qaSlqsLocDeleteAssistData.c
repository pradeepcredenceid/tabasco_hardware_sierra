/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaSlqsLocDeleteAssistData.h"

enum eQCWWANError BuildTlvDeleteAll( BYTE *pBuf, BYTE *pParam )
{
    LocDelAssDataReq *pReq = (LocDelAssDataReq*) pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pSVInfo ||
         pReq->pGnssData ||
         pReq->pCellDb ||
         pReq->pClkInfo||
         pReq->pBdsSVInfo )
        eRCode = PutByte( pBuf, 0);
    else
        eRCode = PutByte( pBuf, 1);

    return eRCode;
}

enum eQCWWANError BuildTlvDelSVInfo( BYTE *pBuf, BYTE *pParam )
{
    BYTE i;
    LocDelAssDataReq *pReq = (LocDelAssDataReq*) pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pSVInfo == NULL )
        return eRCode;

    eRCode = PutByte( pBuf, pReq->pSVInfo->len);
    for(i=0; i< pReq->pSVInfo->len; i++)
    {
        eRCode = PutWord( pBuf, pReq->pSVInfo->pSV[i].id);
        eRCode = PutLong( pBuf, pReq->pSVInfo->pSV[i].system);
        eRCode = PutByte( pBuf, pReq->pSVInfo->pSV[i].mask);
    }

    return eRCode;
}

enum eQCWWANError BuildTlvDelBdsSVInfo( BYTE *pBuf, BYTE *pParam )
{
    BYTE i;
    LocDelAssDataReq *pReq = (LocDelAssDataReq*) pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pBdsSVInfo == NULL )
        return eRCode;

    eRCode = PutByte( pBuf, pReq->pBdsSVInfo->len);
    for(i=0; i< pReq->pBdsSVInfo->len; i++)
    {
        eRCode = PutWord( pBuf, pReq->pBdsSVInfo->pSV[i].id);
        eRCode = PutByte( pBuf, pReq->pBdsSVInfo->pSV[i].mask);
    }

    return eRCode;
}

enum eQCWWANError BuildTlvDelGnssData( BYTE *pBuf, BYTE *pParam )
{
    LocDelAssDataReq *pReq = (LocDelAssDataReq*) pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pGnssData == NULL)
        return eRCode;

    eRCode = Put64( pBuf, pReq->pGnssData->mask);

    return eRCode;
}

enum eQCWWANError BuildTlvDelCellDb( BYTE *pBuf, BYTE *pParam )
{
    LocDelAssDataReq *pReq = (LocDelAssDataReq*) pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pCellDb == NULL)
        return eRCode;

    eRCode = PutLong( pBuf, pReq->pCellDb->mask);

    return eRCode;
}

enum eQCWWANError BuildTlvDelClkInfo( BYTE *pBuf, BYTE *pParam )
{
    LocDelAssDataReq *pReq = (LocDelAssDataReq*) pParam;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    if ( pReq->pClkInfo == NULL)
        return eRCode;

    eRCode = PutLong( pBuf, pReq->pClkInfo->mask);

    return eRCode;
}

enum eQCWWANError PkQmiLocDelAssData(
    WORD                *pMlength,
    BYTE                *pParamField,
    LocDelAssDataReq    *pReq)

{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_DELETE_ASSIST_DATA_ALL,           &BuildTlvDeleteAll },
        { eTLV_DELETE_ASSIST_DATA_SV_INFO,       &BuildTlvDelSVInfo },
        { eTLV_DELETE_ASSIST_DATA_GNSS_DATA,     &BuildTlvDelGnssData },
        { eTLV_DELETE_ASSIST_DATA_CELL_DATABASE, &BuildTlvDelCellDb },
        { eTLV_DELETE_ASSIST_DATA_CLOCK_INFO,    &BuildTlvDelClkInfo },
        { eTLV_DELETE_ASSIST_DATA_BDS_SV_INFO,   &BuildTlvDelBdsSVInfo },
        { eTLV_TYPE_INVALID,                     NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      (BYTE *)pReq,
                      map,
                      eQMI_LOC_DELETE_ASSIST_DATA,
                      pMlength );

    return eRCode;
}


enum eQCWWANError UpkQmiLocDelAssData(
                  BYTE                         *pMdmResp,
                  struct QmiLocDelAssDataResp       *pApiResp)
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
                           eQMI_LOC_DELETE_ASSIST_DATA);
    return eRCode;
}
