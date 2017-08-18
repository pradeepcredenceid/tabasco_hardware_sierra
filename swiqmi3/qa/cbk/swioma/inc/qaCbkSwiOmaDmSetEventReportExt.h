/*
 * \ingroup cbk
 *
 * \file qaSwiOmaDmSetEventReportExt.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaSwiOmaDmSetEventReport.c
 *
 * Copyright: © 2011 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __SWIOMA_DM_SET_EVENT_REPORTEXT_H__
#define __SWIOMA_DM_SET_EVENT_REPORTEXT_H__

#include "qaQmiNotify.h"

/*
 * This structure contains the SWIOMA_DM Set Event Report
 * response parameters
 *
 */
struct QmiCbkSwiOmaDmSetEventReportRespExt
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};


/*
 * Prototypes
 *
 */
extern enum eQCWWANError PkQmiCbkSwiOmaDmSetEventReportExt(
    WORD *pMlength,
    BYTE *pBuffer );

extern enum eQCWWANError UpkQmiCbkSwiOmaDmSetEventReportExt(
    BYTE                                    *pMdmResp,
    struct QmiCbkSwiOmaDmSetEventReportRespExt *pApiResp );

#endif /* __SWIOMA_DM_SET_EVENT_REPORT_H__ */
