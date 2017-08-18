/*
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 */

#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaQosCommon.h"
#include "qaQosReset.h"

enum eQCWWANError PkQmiQosReset (
    WORD    *pMlength,
    BYTE    *pParamField)
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_TYPE_INVALID, NULL }  /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      NULL,
                      map,
                      eQMI_QOS_RESET,
                      pMlength );

    return eRCode;
}

enum eQCWWANError UpkQmiQosReset(
    BYTE    *pMdmResp,
    struct  QmiQosResetResp *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,             &qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID,            NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };

    eRCode = qmunpackresp( pMdmResp,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_QOS_RESET );
    return eRCode;
}


