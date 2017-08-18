/*
 * \ingroup cbk
 *
 * \file qaDmsSwiSetEventReport.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaDmsSwiSetEventReport.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __DMS_SWI_SET_EVENT_REPORT_H__
#define __DMS_SWI_SET_EVENT_REPORT_H__

#include "qaQmiNotify.h"

/*
 * An enumeration of eQMI_DMS_SWI_SET_EVENT_REPORT response TLV IDs
 *
 */
enum eDMS_SWI_SET_EVENT_REPORT_REQ
{
    eTLV_TEMPERATURE = 0x10,
    eTLV_VOLTAGE     = 0x11
};

/*
 * This structure contains the DMS SWI Set Event Report request parameters
 *
 * \param eTemperature
 *        - Temperature reporting
 *
 * \param eVoltage
 *        - Voltage reporting
 *
 */
struct QmiCbkDmsSwiSetEventReportReq
{
    /* Parameters to be passed in the SET_EVENT_REQ structure */
    enum eQmiCbkSetStatus eTemperature;
    enum eQmiCbkSetStatus eVoltage;
};

/*
 * This structure contains the DMS Swi Set Event Report
 * response parameters
 *
 */
struct QmiCbkDmsSwiSetEventReportResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 *
 */
extern enum eQCWWANError PkQmiCbkDmsSwiSetEventReport(
    WORD                  *pMlength,
    enum eQmiCbkSetStatus eTemperature,
    enum eQmiCbkSetStatus eVoltage,
    BYTE                  *pBuffer );

extern enum eQCWWANError UpkQmiCbkDmsSwiSetEventReport(
    BYTE                                  *pMdmResp,
    struct QmiCbkDmsSwiSetEventReportResp *pApiResp );

#endif /* __DMS_SWI_SET_EVENT_REPORT_H__ */
