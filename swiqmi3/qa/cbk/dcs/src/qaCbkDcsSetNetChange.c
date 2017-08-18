#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkDcsSetNetChange.h"

local enum eQCWWANError BuildTlvWaterMarks(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiCbkDcsSetNetChangeReq *pReq =
        (struct QmiCbkDcsSetNetChangeReq *)pParam;

    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = PutLong ( pBuf, pReq->hiMark );
    if (eRCode == eQCWWAN_ERR_NONE)
    {
        eRCode = PutLong ( pBuf, pReq->loMark );
    }
    return eRCode;
}

local enum eQCWWANError BuildTlvPeriod(
    BYTE    *pBuf,
    BYTE    *pParam )
{
    struct QmiCbkDcsSetNetChangeReq *pReq =
        (struct QmiCbkDcsSetNetChangeReq *)pParam;

    return PutLong ( pBuf, pReq->period );
}

package enum eQCWWANError PkQmiCbkDcsSetNetChange(
    WORD                   *pMlength,
    ULONG                   hiMark,
    ULONG                   loMark,
    ULONG                   period,
    BYTE                   *pParamField )
{
    static struct qmTlvBuilderItem map[] =
    {
         { eTLV_WATERMARKS,  BuildTlvWaterMarks },
         { eTLV_PERIOD,     BuildTlvPeriod },
         { eTLV_TYPE_INVALID,             NULL }  /* Important. Sentinel.
                                                   * Signifies last item in map.
                                                   */
    };

    struct QmiCbkDcsSetNetChangeReq req;
    slmemset( (char *)&req,
              0,
              sizeof (struct QmiCbkDcsSetNetChangeReq) );

    req.hiMark = hiMark;
    req.loMark = loMark;
    req.period = period;

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      (BYTE *)&req,
                      map,
                      eQMI_DCS_SET_NET_CHANGE,
                      pMlength );
    return eRCode;
}

package enum eQCWWANError UpkQmiCbkDcsSetNetChange(
    BYTE    *pMdmResp,
    struct  QmiCbkDcsSetNetChangeCbkResp *pApiResp)
{
    enum eQCWWANError eRCode;

    /* The unpack of the Callback contains only result code */
    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE       ,&qmUnpackTlvResultCode },
        { eTLV_TYPE_INVALID      ,NULL }  /* Important. Sentinel.
                                           * Signifies last item in map.
                                           */
    };
    eRCode =  qmunpackresp( pMdmResp,
                           (BYTE*)pApiResp,
                           map,
                           eQMI_WDS_SET_EVENT);
    return eRCode;
}

