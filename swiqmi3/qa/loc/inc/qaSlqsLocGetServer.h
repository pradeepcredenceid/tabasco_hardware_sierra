/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_GET_SERVER_H__
#define __LOC_GET_SERVER_H__

#include "qaGobiApiLoc.h"

enum
{
    eTLV_GETSERVER_TYPE      = 0x01,
    eTLV_ADDRESS_TYPE        = 0x10
};

struct QmiLocGetServerResp
{
    struct qmTlvResult results;
};

extern enum eQCWWANError PkQmiLocGetServerReq(
            WORD                *pMlength,
            BYTE                *pParamField,
            LOCGetServerReq     *pLOCGetServerReq);

extern enum eQCWWANError UpkQmiLocGetServerResp(
            BYTE                          *pMdmResp,
            struct QmiLocGetServerResp    *pApiResp);

#endif /* __LOC_GET_SERVER_H__ */