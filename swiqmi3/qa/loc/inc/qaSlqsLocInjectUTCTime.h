/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_INJECT_UTC_TIME_H__
#define __LOC_INJECT_UTC_TIME_H__

#include "qaGobiApiLoc.h"

enum
{
    eTLV_UTC_TIME           = 0x01,
    eTLV_UNCERTAINTY_TIME   = 0x02
};

struct QmiLocInjectUTCTimeReq
{
    ULONGLONG timeMsec;
    ULONG     timeUncMsec;
};

struct QmiLocInjectUTCTimeResp
{
    struct qmTlvResult results;
};

extern enum eQCWWANError PkQmiLocInjectUTCTime(
            WORD                *pMlength,
            BYTE                *pParamField,
            ULONGLONG           timeMsec,
            ULONG               timeUncMsec);

extern enum eQCWWANError UpkQmiLocInjectUTCTime(
            BYTE                              *pMdmResp,
            struct QmiLocInjectUTCTimeResp    *pApiResp);

#endif /* __LOC_INJECT_UTC_TIME_H__ */