#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

#include "qaQosCommon.h"
#include "qaGobiApiQos.h"
#include "qaQosGetGranted.h"
enum {
    eTLV_TX_GRANTED = 0x11
};

enum eQCWWANError PkQmiQosGetGranted (
    WORD    *pMlength,
    BYTE    *pParamField,
    ULONG    id
    )
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
                      eQMI_QOS_GET_GRANTED,
                      pMlength );

    return eRCode;
}

static enum eQCWWANError unpackFlowDataRate(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    swiQosGranted *pGranted = (swiQosGranted*) pResp;
    swiQosFlow *pTxFlow = (swiQosFlow*) pGranted->pTxFlow;

    if (pTxFlow->pDataRate)
    {
        eRCode = GetLong( pTlvData, &pTxFlow->pDataRate->dataRateMax);
        if ( eRCode == eQCWWAN_ERR_NONE)
        {
            return GetLong( pTlvData, &pTxFlow->pDataRate->guaranteedRate);
        }
        else
        {
            return eRCode;
        }
    }

    return eRCode;
}

static enum eQCWWANError unpackFlowLatency(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    swiQosGranted *pGranted = (swiQosGranted*) pResp;
    swiQosFlow *pTxFlow = (swiQosFlow*) pGranted->pTxFlow;

    if (pTxFlow->pLatency)
    {
        eRCode = GetLong( pTlvData, pTxFlow->pLatency);
    }

    return eRCode;
}

static enum eQCWWANError unpackFlowJitter(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    swiQosGranted *pGranted = (swiQosGranted*) pResp;
    swiQosFlow *pTxFlow = (swiQosFlow*) pGranted->pTxFlow;

    if (pTxFlow->pJitter)
    {
        eRCode = GetLong( pTlvData, pTxFlow->pJitter);
    }

    return eRCode;
}

static enum eQCWWANError unpackFlowLteQci(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    swiQosGranted *pGranted = (swiQosGranted*) pResp;
    swiQosFlow *pTxFlow = (swiQosFlow*) pGranted->pTxFlow;

    if (pTxFlow->pLteQci)
    {
        eRCode = GetByte( pTlvData, pTxFlow->pLteQci);
    }

    return eRCode;
}

static enum eQCWWANError unpackFlowIdx(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    swiQosGranted *pGranted = (swiQosGranted*) pResp;
    swiQosFlow *pTxFlow = (swiQosFlow*) pGranted->pTxFlow;

    eRCode = GetByte( pTlvData, &pTxFlow->index);

    return eRCode;
}

static enum eQCWWANError qmUnpackTlvTxGranted(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode;
    struct qmTBuffer *tbufp = (struct qmTBuffer *)pTlvData;

    struct QmiQosGetGrantedResp *lResp =
        (struct QmiQosGetGrantedResp *)pResp;

    if (lResp->pGranted == NULL)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_QOS_FLOW_IDX,        unpackFlowIdx },
        { eTLV_QOS_FLOW_DATA_RATE,  unpackFlowDataRate },
        { eTLV_QOS_FLOW_LATAENCY,   unpackFlowLatency },
        { eTLV_QOS_FLOW_JITTER,     unpackFlowJitter },
        { eTLV_QOS_FLOW_LTE_QCI,    unpackFlowLteQci },
        { eTLV_TYPE_INVALID,            NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };

    eRCode = tlv_unpack(
                &tbufp->Datap[tbufp->Index],
                tbufp->Size - tbufp->Index,
                (BYTE*)(lResp->pGranted),
                map );

    return eRCode;
}

enum eQCWWANError UpkQmiQosGetGranted(
    BYTE    *pMdmResp,
    struct  QmiQosGetGrantedResp *pApiResp )
{
    enum eQCWWANError eRCode;

    //TODO simulate there are qos ids returned
    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,             &qmUnpackTlvResultCode },
        { eTLV_TX_GRANTED,              &qmUnpackTlvTxGranted },
        { eTLV_TYPE_INVALID,            NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };

    eRCode = qmunpackresp( pMdmResp,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_QOS_GET_GRANTED );
    return eRCode;
}

