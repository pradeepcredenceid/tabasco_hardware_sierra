/*
 * Copyright: © 2016 Sierra Wireless, Inc. all rights reserved
 */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "SwiLocAutoStart.h"
#include "qaGobiApiLoc.h"

enum eQCWWANError PkSwiLocGetAutoStart(
    WORD                *pMlength,
    BYTE                *pParamField)

{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID,                     NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      NULL,
                      map,
                      eQMI_SWI_LOC_GET_AUTO_START,
                      pMlength );

    return eRCode;
}

enum eQCWWANError UpFunc( BYTE* pTlvData, BYTE *pResp )
{
    struct QmiSwiLocGetAutoStartResp *lResp =
        (struct QmiSwiLocGetAutoStartResp *)pResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = GetByte( pTlvData, &lResp->resp->function );
    if ( eRCode == eQCWWAN_ERR_NONE )
        lResp->resp->function_reported = 1;

    return eRCode;
}

enum eQCWWANError UpFixType( BYTE* pTlvData, BYTE *pResp )
{
    struct QmiSwiLocGetAutoStartResp *lResp =
        (struct QmiSwiLocGetAutoStartResp *)pResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = GetByte( pTlvData, &lResp->resp->fix_type );
    if ( eRCode == eQCWWAN_ERR_NONE )
        lResp->resp->fix_type_reported = 1;

    return eRCode;
}

enum eQCWWANError UpMaxTime( BYTE* pTlvData, BYTE *pResp )
{
    struct QmiSwiLocGetAutoStartResp *lResp =
        (struct QmiSwiLocGetAutoStartResp *)pResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = GetByte( pTlvData, &lResp->resp->max_time );
    if ( eRCode == eQCWWAN_ERR_NONE )
        lResp->resp->max_time_reported = 1;

    return eRCode;
}

enum eQCWWANError UpMaxDist( BYTE* pTlvData, BYTE *pResp )
{
    struct QmiSwiLocGetAutoStartResp *lResp =
        (struct QmiSwiLocGetAutoStartResp *)pResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = GetLong( pTlvData, &lResp->resp->max_dist );
    if ( eRCode == eQCWWAN_ERR_NONE )
        lResp->resp->max_dist_reported = 1;

    return eRCode;
}

enum eQCWWANError UpFixRate( BYTE* pTlvData, BYTE *pResp )
{
    struct QmiSwiLocGetAutoStartResp *lResp =
        (struct QmiSwiLocGetAutoStartResp *)pResp;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = GetLong( pTlvData, &lResp->resp->fix_rate );
    if ( eRCode == eQCWWAN_ERR_NONE )
        lResp->resp->fix_rate_reported = 1;

    return eRCode;
}

enum eQCWWANError UpkSwiLocGetAutoStart(
                  BYTE                         *pMdmResp,
                  struct QmiSwiLocGetAutoStartResp       *pApiResp)
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,  &qmUnpackTlvResultCode },
        { eTLV_FUNCTION, &UpFunc },
        { eTLV_FIX_TYPE, &UpFixType },
        { eTLV_MAX_TIME, &UpMaxTime },
        { eTLV_MAX_DIST, &UpMaxDist },
        { eTLV_FIX_RATE, &UpFixRate },
        { eTLV_TYPE_INVALID, NULL } /* Important. Sentinel.
                                     * Signifies last item in map.
                                     */
    };

    eRCode = qmunpackresp( pMdmResp,
                           (BYTE *)pApiResp,
                           map,
                           eQMI_SWI_LOC_GET_AUTO_START);
    return eRCode;
}

enum eQCWWANError pkFunc( BYTE *pBuf, BYTE *pParam )
{
    SwiLocSetAutoStartReq *pReq =
        (SwiLocSetAutoStartReq *)pParam;

    if ( pReq->set_function )
        return PutByte(pBuf, pReq->function);
    else
        return eQCWWAN_ERR_NONE;
}

enum eQCWWANError pkFixType( BYTE *pBuf, BYTE *pParam )
{
    SwiLocSetAutoStartReq *pReq =
        (SwiLocSetAutoStartReq *)pParam;

    if ( pReq->set_fix_type )
        return PutByte(pBuf, pReq->fix_type);
    else
        return eQCWWAN_ERR_NONE;
}

enum eQCWWANError pkMaxTime( BYTE *pBuf, BYTE *pParam )
{
    SwiLocSetAutoStartReq *pReq =
        (SwiLocSetAutoStartReq *)pParam;

    if ( pReq->set_max_time )
        return PutByte(pBuf, pReq->max_time);
    else
        return eQCWWAN_ERR_NONE;
}

enum eQCWWANError pkMaxDist( BYTE *pBuf, BYTE *pParam )
{
    SwiLocSetAutoStartReq *pReq =
        (SwiLocSetAutoStartReq *)pParam;

    if ( pReq->set_max_dist )
        return PutLong(pBuf, pReq->max_dist);
    else
        return eQCWWAN_ERR_NONE;
}

enum eQCWWANError pkFixRate( BYTE *pBuf, BYTE *pParam )
{
    SwiLocSetAutoStartReq *pReq =
        (SwiLocSetAutoStartReq *)pParam;

    if ( pReq->set_fix_rate )
        return PutLong(pBuf, pReq->fix_rate);
    else
        return eQCWWAN_ERR_NONE;
}

enum eQCWWANError PkSwiLocSetAutoStart(
    WORD                *pMlength,
    BYTE                *pParamField,
    SwiLocSetAutoStartReq *pReq)

{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_FUNCTION, &pkFunc },
        { eTLV_FIX_TYPE, &pkFixType },
        { eTLV_MAX_TIME, &pkMaxTime },
        { eTLV_MAX_DIST, &pkMaxDist },
        { eTLV_FIX_RATE, &pkFixRate },
        { eTLV_TYPE_INVALID,                     NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      (swi_uint8*) pReq,
                      map,
                      eQMI_SWI_LOC_SET_AUTO_START,
                      pMlength );

    return eRCode;
}

enum eQCWWANError UpkSwiLocSetAutoStart(
                  BYTE                         *pMdmResp,
                  struct QmiSwiLocSetAutoStartResp       *pApiResp)
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
                           eQMI_SWI_LOC_SET_AUTO_START);
    return eRCode;
}
