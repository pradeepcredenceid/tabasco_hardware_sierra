/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaSlqsLocSetOpMode.h"

enum eQCWWANError BuildTlvOpMode( BYTE *pBuf, BYTE *pParam )
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    ULONG *mode = (ULONG*) pParam;
    eRCode = PutLong( pBuf, *mode);
    return eRCode;
}

enum eQCWWANError PkQmiLocSetOpMode(
    WORD                *pMlength,
    BYTE                *pParamField,
    ULONG               mode )
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_SET_OP_MODE,                    &BuildTlvOpMode },
        { eTLV_TYPE_INVALID,               NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&mode,
                      map,
                      eQMI_LOC_SET_OPERATION_MODE,
                      pMlength );

    return eRCode;
}


enum eQCWWANError UpkQmiLocSetOpMode(
                  BYTE                         *pMdmResp,
                  struct QmiLocSetOpModeResp       *pApiResp)
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
                           eQMI_LOC_SET_OPERATION_MODE);
    return eRCode;
}

