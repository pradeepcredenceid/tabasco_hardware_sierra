/*
 * \ingroup cbk
 *
 * \file    qaCbkQosEventReportInd.c
 *
 * \brief   Contains UnPacking routines for the
 *          QMI_QOS_EVENT_REPORT_IND message.
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

/* include files */

#include "SwiDataTypes.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaCbkQosEventReportInd.h"
#include "qaQosCommon.h"

extern BYTE cached_qos_flow[255];

static enum eQCWWANError uFlowState(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    struct FlowInfoTlv *pFlowInfoTlv =
        &((struct QmiCbkQosEventReportInd *)pResp)->flowInfoTlv;

    //TODO proper error handling
    eRCode = GetLong( pTlvData, &pFlowInfoTlv->id);
    eRCode = GetByte( pTlvData, &pFlowInfoTlv->isNew);
    eRCode = GetByte( pTlvData, &pFlowInfoTlv->state);

    if ( eRCode == eQCWWAN_ERR_NONE )
    {
        pFlowInfoTlv->TlvPresent = TRUE;
    }

    return eRCode;
}

static enum eQCWWANError unpackFlowIdx(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    struct FlowGrantedTlv *pTxFlowTlv = (struct FlowGrantedTlv*) pResp;

    eRCode = GetByte( pTlvData, &pTxFlowTlv->index);

    return eRCode;
}

static enum eQCWWANError unpackFlowDataRate(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    struct DataRateTlv *pDataRateTlv =
        &((struct FlowGrantedTlv *)pResp)->dataRateTlv;

    eRCode = GetLong( pTlvData, &pDataRateTlv->dataRateMax);
    if ( eRCode == eQCWWAN_ERR_NONE)
    {
        eRCode = GetLong( pTlvData, &pDataRateTlv->guaranteedRate);
    }

    if ( eRCode == eQCWWAN_ERR_NONE )
    {
        pDataRateTlv->TlvPresent = TRUE;
    }

    return eRCode;
}

static enum eQCWWANError unpackFlowLatency(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    struct LatencyTlv *pLatencyTlv =
        &((struct FlowGrantedTlv *)pResp)->latencyTlv;

    eRCode = GetLong( pTlvData, &pLatencyTlv->latency);

    if ( eRCode == eQCWWAN_ERR_NONE )
    {
        pLatencyTlv->TlvPresent = TRUE;
    }

    return eRCode;
}

static enum eQCWWANError unpackFlowJitter(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    struct JitterTlv *pJitterTlv =
        &((struct FlowGrantedTlv *)pResp)->jitterTlv;

    eRCode = GetLong( pTlvData, &pJitterTlv->jitter);

    if ( eRCode == eQCWWAN_ERR_NONE )
    {
        pJitterTlv->TlvPresent = TRUE;
    }

    return eRCode;
}

static enum eQCWWANError unpackFlowLteQci(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    struct QciTlv *pQciTlv =
        &((struct FlowGrantedTlv *)pResp)->qciTlv;

    eRCode = GetByte( pTlvData, &pQciTlv->qci);

    if ( eRCode == eQCWWAN_ERR_NONE )
    {
        pQciTlv->TlvPresent = TRUE;
    }

    return eRCode;
}

static enum eQCWWANError uFlowTx(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    struct qmTBuffer *tbufp = (struct qmTBuffer *)pTlvData;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    struct FlowGrantedTlv *pTxFlowTlv =
        &((struct QmiCbkQosEventReportInd *)pResp)->txFlowTlv;

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
                (BYTE*)(pTxFlowTlv),
                map );

    if ( eRCode == eQCWWAN_ERR_NONE )
    {
        pTxFlowTlv->TlvPresent = TRUE;
    }

    return eRCode;
}

static enum eQCWWANError uFlowInfo(
    swi_uint8 *pTlvData,
    swi_uint8 *pResp)
{
    enum eQCWWANError eRCode;
    struct qmTBuffer *tbufp = (struct qmTBuffer *)pTlvData;

    struct QmiCbkQosEventReportInd *lResp =
        (struct QmiCbkQosEventReportInd *)pResp;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_FLOW_STATE,        uFlowState },
        { eTLV_FLOW_TX,           uFlowTx },
        { eTLV_TYPE_INVALID,            NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };

    eRCode = tlv_unpack(
                &tbufp->Datap[tbufp->Index],
                tbufp->Size - tbufp->Index,
                (BYTE*)(lResp),
                map );

    return eRCode;
}

package enum eQCWWANError UpkQmiCbkQosEventInd(
    BYTE    *pMdmResp,
    struct QmiCbkQosEventReportInd *pApiResp )
{
    enum eQCWWANError eRCode;

    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_FLOW_INFO,    &uFlowInfo },
        { eTLV_TYPE_INVALID,             NULL }  /* Important. Sentinel.
                                                  * Signifies last item in map.
                                                  */
    };

#ifdef QOS_SIMULATE
    int len = pMdmResp[2];

    BYTE response_hack[255];
    memcpy(response_hack, pMdmResp, len + 4);   //including message id & length
    //change type from 0x10 to 0x11 to satisfy EVENT_REPORT_IND
    cached_qos_flow[0] = 0x11;
    //append
    memcpy(&response_hack[len+4], cached_qos_flow, cached_qos_flow[1] + 3);
    //update original length
    response_hack[2] = len + cached_qos_flow[1] + 3;

    eRCode = qmunpackresp( response_hack,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_QOS_EVENT_IND );
#else
    eRCode = qmunpackresp( pMdmResp,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_QOS_EVENT_IND );
#endif

    return eRCode;
}

