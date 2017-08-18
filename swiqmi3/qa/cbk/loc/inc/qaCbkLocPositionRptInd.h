/*
 * \ingroup cbk
 *
 * \file qaCbkLocPositionRptInd.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaCbkLocPositionRptInd.c
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_POSITION_RPT_IND_H__
#define __LOC_POSITION_RPT_IND_H__

#include "qaGobiApiCbk.h"
/*
 * An enumeration of eQMI_LOC_POSITION_REPORT_IND response TLV IDs
 *
 */
enum eQMI_LOC_POSITION_REPORT_IND
{
    eTLV_IND_SESSION_STATUS       = 0x01,
    eTLV_IND_SESSION_ID           = 0x02,
    eTLV_IND_LATITUDE             = 0x10,
    eTLV_IND_LONGITUDE            = 0x11,
    eTLV_IND_CIR_HOR_POS_UNC      = 0x12,
    eTLV_IND_HOR_ELLIPSE_UNC_MIN  = 0x13,
    eTLV_IND_HOR_ELLIPSE_UNC_MAJ  = 0x14,
    eTLV_IND_HOR_ELLIPSE_UNC_AZI  = 0x15,
    eTLV_IND_HOR_CONFIDENCE       = 0x16,
    eTLV_IND_HOR_RELIABILITY      = 0x17,
    eTLV_IND_HOR_SPEED            = 0x18,
    eTLV_IND_SPEED_UNC            = 0x19,
    eTLV_IND_ALTITUDE_WRT_ELLIPSE = 0x1A,
    eTLV_IND_ALTITUDE_WRT_SEA_LVL = 0x1B,
    eTLV_IND_VERT_UNC             = 0x1C,
    eTLV_IND_VERT_CONFIDENCE      = 0x1D,
    eTLV_IND_VERT_RELIABILITY     = 0x1E,
    eTLV_IND_VERT_SPEED           = 0x1F,
    eTLV_IND_HEADING              = 0x20,
    eTLV_IND_HEADING_UNC          = 0x21,
    eTLV_IND_MAGNETIC_DEVIATION   = 0x22,
    eTLV_IND_TECH_MASK            = 0x23,
    eTLV_IND_DILUTION_PERCISION   = 0x24,
    eTLV_IND_UTC_TIME_STAMP       = 0x25,
    eTLV_IND_LEAP_SECONDS         = 0x26,
    eTLV_IND_GPS_TIME             = 0x27,
    eTLV_IND_TIME_UNC             = 0x28,
    eTLV_IND_TIME_SRC             = 0x29,
    eTLV_IND_SENSOR_DATA_USAGE    = 0x2A,
    eTLV_IND_FIX_ID               = 0x2B,
    eTLV_IND_SV_LIST              = 0x2C,
    eTLV_IND_ALTITUDE_ASSUMED     = 0x2D
};

/*
 * Prototypes
 *
 */
enum eQCWWANError UpkQmiCbkLocPositionRptInd(
    BYTE                              *pMdmResp,
    QmiCbkLocPositionReportInd        *pApiResp );

#endif /* __LOC_POSITION_RPT_IND_H__ */
