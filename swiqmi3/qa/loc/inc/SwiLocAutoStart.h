/*
 * Copyright: © 2016 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_SWI_AUTO_START_H__
#define __LOC_SWI_AUTO_START_H__

#include "qaGobiApiLoc.h"

enum
{
    eTLV_FUNCTION = 0x10,
    eTLV_FIX_TYPE = 0x11,
    eTLV_MAX_TIME = 0x12,
    eTLV_MAX_DIST = 0x13,
    eTLV_FIX_RATE = 0x14
};

struct QmiSwiLocGetAutoStartResp
{
    struct qmTlvResult results;
    SwiLocGetAutoStartResp *resp;
};

enum eQCWWANError PkSwiLocGetAutoStart(
    WORD                *pMlength,
    BYTE                *pParamField);

enum eQCWWANError UpkSwiLocGetAutoStart(
                  BYTE                         *pMdmResp,
                  struct QmiSwiLocGetAutoStartResp       *pApiResp);

struct QmiSwiLocSetAutoStartResp
{
    struct qmTlvResult results;
};

enum eQCWWANError PkSwiLocSetAutoStart(
    WORD                *pMlength,
    BYTE                *pParamField,
    SwiLocSetAutoStartReq *pReq);

enum eQCWWANError UpkSwiLocSetAutoStart(
                  BYTE                         *pMdmResp,
                  struct QmiSwiLocSetAutoStartResp       *pApiResp);

#endif /* __LOC_SWI_AUTO_START_H__ */
