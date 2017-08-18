/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_DELETE_ASSIST_DATA_H__
#define __LOC_DELETE_ASSIST_DATA_H__

#include "qaGobiApiLoc.h"

enum eQMI_LOC_DELETE_ASSIST_DATA_REQ
{
    eTLV_DELETE_ASSIST_DATA_ALL        = 0x01,
    eTLV_DELETE_ASSIST_DATA_SV_INFO = 0X10,
    eTLV_DELETE_ASSIST_DATA_GNSS_DATA = 0X11,
    eTLV_DELETE_ASSIST_DATA_CELL_DATABASE = 0X12,
    eTLV_DELETE_ASSIST_DATA_CLOCK_INFO = 0X13,
    eTLV_DELETE_ASSIST_DATA_BDS_SV_INFO = 0X14
};

struct QmiLocDelAssDataResp
{
    struct qmTlvResult results;
};

enum eQCWWANError PkQmiLocDelAssData(
    WORD                *pMlength,
    BYTE                *pParamField,
    LocDelAssDataReq    *pReq);

enum eQCWWANError UpkQmiLocDelAssData(
                  BYTE                         *pMdmResp,
                  struct QmiLocDelAssDataResp       *pApiResp);

#endif /* __LOC_DELETE_ASSIST_DATA_H__ */
