/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaSlqsLocGetFixCriteria.h"

enum eQCWWANError PkQmiLocGetFixCriteriaReq(
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
                      eQMI_LOC_GET_FIX_CRITERIA,
                      pMlength );

    return eRCode;
}


enum eQCWWANError UpkQmiLocGetFixCriteriaResp(
                  BYTE                              *pMdmResp,
                  struct QmiLocGetFixCriteriaResp   *pApiResp)
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
                           eQMI_LOC_GET_FIX_CRITERIA);
    return eRCode;
}

