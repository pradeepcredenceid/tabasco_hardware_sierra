 /**
 * \ingroup cbk
 *
 * \file    qaSetQosEventReport.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaWdsSetEventReport.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __QOS_SET_EVENT_REPORT_H__
#define __QOS_SET_EVENT_REPORT_H__

#include "qaQmiNotify.h"

#define EXTEND_TECHNOLOGY_PREFERENCE_CDMA   0x8001

/**
 *
 * An enumeration of eQMI_SET_EVENT_REPORT request TLV IDs
 */
enum eQOS_SET_EVENT_REPORT_REQ
{
    eTLV_GLOBAL_FLOW_REPORT                     = 0x10,
    eTLV_NETWORK_SUPPORTED_QOS_PROFILE_CHANGE   = 0x11
};

/**
 *
 * This structure contains the QOS Set Event Status request parameters
 *
 * \param  eGlobalFlow[IN]
 *         - Global flow info indication
 *
 * \param  eQosProfChange[IN]
 *         - Network supported QoS Profile change Indicator
 *
 */
struct QmiCbkSetQosEventReportReq
{
    /* Parameters to be passed in the QOS_SET_EVET_REQ structure */
    enum eQmiCbkSetStatus eGlobalFlow;
    enum eQmiCbkSetStatus eQosProfChange;
};

/**
 * This structure contains the QOS Set Event Report
 * response parameters
 */
struct QmiCbkSetQosEventReportResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiCbkSetQosEventReport(
    WORD                  *pMlength,
    enum eQmiCbkSetStatus eGlobalFlow,
    enum eQmiCbkSetStatus eQosProfChange,
    BYTE                  *pBuffer );

extern enum eQCWWANError UpkQmiCbkSetQosEventReport(
    BYTE   *mdmresp,
    struct QmiCbkSetQosEventReportResp *apiresp);

#endif /* __QOS_SET_EVENT_REPORT_H__ */
