#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaQosCommon.h"
#include "qaQosGetFlowStatus.h"

enum
{
    eTLV_FLOW_STATUS = 0x01
};

enum eQCWWANError PkQmiQosGetFlowStatus (
    WORD    *pMlength,
    BYTE    *pParamField,
    ULONG    id)
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_QOS_IDS,    &mBuildTlvId},
        { eTLV_TYPE_INVALID, NULL }  /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      (BYTE*)&id,
                      map,
                      eQMI_QOS_GET_FLOW_STATUS,
                      pMlength );

    return eRCode;
}

static enum eQCWWANError UnpackTlvFlowStatus(
    BYTE* pTlvData,
    BYTE *pResp )
{
    struct QmiQosGetFlowStatusResp *lResp =
              (struct QmiQosGetFlowStatusResp *)pResp;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = GetByte( pTlvData, lResp->pStatus);

    return eRCode;
}

enum eQCWWANError UpkQmiQosGetFlowStatus (
    BYTE     *pMdmResp,
    struct   QmiQosGetFlowStatusResp *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,             &qmUnpackTlvResultCode },
        { eTLV_FLOW_STATUS,             &UnpackTlvFlowStatus },
        { eTLV_TYPE_INVALID,            NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };

    eRCode = qmunpackresp( pMdmResp,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_QOS_GET_FLOW_STATUS );
    return eRCode;
}
