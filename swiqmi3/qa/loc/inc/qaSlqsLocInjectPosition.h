/*
 * \ingroup loc
 *
 * \file  qaSlqsLocInjectPosition.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaSlqsLocInjectPosition.c
 *
 * Copyright: © 2011-2016 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_INJECT_POSITION_H__
#define __LOC_INJECT_POSITION_H__

#include "qaGobiApiLoc.h"

enum eQMI_LOC_INJECT_POSITION_REQ
{
    eTLV_INJECT_LATITUDE_DATA             = 0x10,
    eTLV_INJECT_LONGITUDE_DATA            = 0X11,
    eTLV_INJECT_CIR_HOR_UNC               = 0X12,
    eTLV_INJECT_HOR_CONFIDENCE            = 0X13,
    eTLV_INJECT_HOR_RELIABILITY           = 0X14,
    eTLV_INJECT_ALTITUDE_WRT_ELLIPSOID    = 0X15,
    eTLV_INJECT_ALTITUDE_WRT_MEAN_SEA_LVL = 0X16,
    eTLV_INJECT_VERT_UNCERTAINTY          = 0X17,
    eTLV_INJECT_VERT_CONFIDENCE           = 0X18,
    eTLV_INJECT_VERT_RELIABILITY          = 0X19,
    eTLV_INJECT_ALTITUDE_SOURCE_INFO      = 0X1A,
    eTLV_INJECT_UTC_TIMESTAMP             = 0X1B,
    eTLV_INJECT_POSITION_AGE              = 0X1C,
    eTLV_INJECT_POSITION_SOURCE           = 0X1D,
    eTLV_INJECT_RAW_HOR_UNC_CIRCULAR      = 0X1E,
    eTLV_INJECT_RAW_HOR_CONFIDENCE        = 0X1F
};

struct QmiLocInjectPositionResp
{
    struct qmTlvResult results;
};

enum eQCWWANError PkQmiLocInjectPosition(
    WORD                   *pMlength,
    BYTE                   *pParamField,
    LocInjectPositionReq   *pReq);

enum eQCWWANError UpkQmiLocInjectPosition(
                  BYTE                                 *pMdmResp,
                  struct QmiLocInjectPositionResp       *pApiResp);

#endif /* __LOC_INJECT_POSITION_H__ */
