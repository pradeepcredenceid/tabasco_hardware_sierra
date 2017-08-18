/*
 * \ingroup cbk
 *
 * \file    qaCbkQosEventStatusInd.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaCbkQosEventStatusInd.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __QOS_EVENT_REPORT_IND_H__
#define __QOS_EVENT_REPORT_IND_H__

/* An enumeration of eQMI_QOS_SET_EVENT_REPORT_IND response TLV IDs */
enum eQMI_QOS_SET_EVENT_REPORT_IND
{
    eTLV_FLOW_INFO            = 0x10
};

enum
{
    eTLV_FLOW_STATE = 0x10,
    eTLV_FLOW_TX    = 0x11
};

struct FlowInfoTlv
{
    BYTE    TlvPresent;
    ULONG   id;
    BYTE    isNew;
    BYTE    state;
};

struct DataRateTlv
{
    BYTE    TlvPresent;
    ULONG   dataRateMax;
    ULONG   guaranteedRate;
};

struct LatencyTlv
{
    BYTE    TlvPresent;
    ULONG   latency;
};

struct JitterTlv
{
    BYTE    TlvPresent;
    ULONG   jitter;
};

struct QciTlv
{
    BYTE    TlvPresent;
    BYTE    qci;
};

struct FlowGrantedTlv
{
    BYTE    TlvPresent;
    BYTE    index;
    struct  DataRateTlv dataRateTlv;
    struct  LatencyTlv  latencyTlv;
    struct  JitterTlv   jitterTlv;
    struct  QciTlv      qciTlv;
};

/*
 * Name:    QmiCbkQosEventReportInd
 *
 * Purpose: Structure used to store all QMI Notification parameters.
 *
 */
struct QmiCbkQosEventReportInd{
    ULONG   id;
    BYTE    isNew;
    BYTE    state;
    struct  FlowInfoTlv flowInfoTlv;
    struct  FlowGrantedTlv txFlowTlv;
};

/*************
 * Prototypes
 **************/
enum eQCWWANError UpkQmiCbkQosEventInd (
    BYTE     *pMdmResp,
    struct QmiCbkQosEventReportInd *pAipResp );

#endif /* __QOS_EVENT_REPORT_IND_H__ */
