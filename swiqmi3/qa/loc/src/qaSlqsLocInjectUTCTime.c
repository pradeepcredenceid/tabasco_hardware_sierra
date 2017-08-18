/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaSlqsLocInjectUTCTime.h"

/******************************************************************************
 * Request handling
 ******************************************************************************/

enum eQCWWANError BuildTlvUTCTime( BYTE *pBuf, BYTE *pParam )
{
    struct QmiLocInjectUTCTimeReq *pReq =
        (struct QmiLocInjectUTCTimeReq *)pParam;
    
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    eRCode = Put64( pBuf, pReq->timeMsec);
    return eRCode;
}

enum eQCWWANError BuildTlvUncertaintyTime( BYTE *pBuf, BYTE *pParam )
{
    struct QmiLocInjectUTCTimeReq *pReq =
        (struct QmiLocInjectUTCTimeReq *)pParam;
    
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    eRCode = PutLong( pBuf, pReq->timeUncMsec);
    return eRCode;
}

enum eQCWWANError PkQmiLocInjectUTCTime(
    WORD                *pMlength,
    BYTE                *pParamField,
    ULONGLONG           timeMsec,
    ULONG               timeUncMsec)
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_UTC_TIME,                    &BuildTlvUTCTime },
        { eTLV_UNCERTAINTY_TIME,            &BuildTlvUncertaintyTime },
        { eTLV_TYPE_INVALID,               NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };
    struct QmiLocInjectUTCTimeReq req;
    
    enum eQCWWANError eRCode;

    slmemset((CHAR *)&req, 0, sizeof(struct QmiLocInjectUTCTimeReq));
    req.timeMsec    = timeMsec;
    req.timeUncMsec = timeUncMsec;
    
    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_LOC_INJECT_UTC_TIME,
                      pMlength );

    return eRCode;
}

/******************************************************************************
 * Response handling
 ******************************************************************************/

enum eQCWWANError UpkQmiLocInjectUTCTime(
                  BYTE                              *pMdmResp,
                  struct QmiLocInjectUTCTimeResp    *pApiResp)
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
                           eQMI_LOC_INJECT_UTC_TIME);
    return eRCode;
}


