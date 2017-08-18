/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaSlqsLocGetOpMode.h"

enum eQCWWANError PkQmiLocGetOpModeReq(
    WORD                *pMlength,
    BYTE                *pParamField)
{
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID,                   NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      NULL,
                      map,
                      eQMI_LOC_GET_OPERATION_MODE,
                      pMlength );

    return eRCode;
}


enum eQCWWANError UpkQmiLocGetOpModeResp(
                  BYTE                         *pMdmResp,
                  struct QmiLocGetOpModeResp   *pApiResp)
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
                           eQMI_LOC_GET_OPERATION_MODE);
    return eRCode;
}

