#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaQosCommon.h"

#include "qaGobiApiQos.h"
#include "qaQosRel.h"

enum eQCWWANError PkQmiQosRel (
    WORD    *pMlength,
    BYTE    *pParamField,
    swiQosIds *pQosIds
    )
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_QOS_IDS,    &mBuildTlvQosIds},
        { eTLV_TYPE_INVALID, NULL }  /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      (BYTE*)pQosIds,
                      map,
                      eQMI_QOS_REL,
                      pMlength );

    return eRCode;
}

enum eQCWWANError UpkQmiQosRel(
    BYTE    *pMdmResp,
    struct  QmiQosRelResp *pApiResp )
{
    enum eQCWWANError eRCode;

    //TODO simulate there aer qos ids returned
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
                          eQMI_QOS_REQ );
    return eRCode;
}

