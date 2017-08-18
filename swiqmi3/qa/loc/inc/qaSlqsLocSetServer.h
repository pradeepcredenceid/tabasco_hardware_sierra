/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_SET_SERVER_H__
#define __LOC_SET_SERVER_H__

#include "qaGobiApiLoc.h"

enum
{
    eTLV_SETSERVER_TYPE      = 0x01,
    eTLV_IPV4_ADDRESS        = 0x10,
    eTLV_IPV6_ADDRESS        = 0x11,
    eTLV_URL_ADDRESS         = 0x12
};

struct QmiLocSetServerResp
{
    struct qmTlvResult results;
};

extern enum eQCWWANError PkQmiLocSetServerReq(
            WORD                *pMlength,
            BYTE                *pParamField,
            LOCSetServerReq     *pLOCSetServerReq);

extern enum eQCWWANError UpkQmiLocSetServerResp(
            BYTE                          *pMdmResp,
            struct QmiLocSetServerResp    *pApiResp);

#endif /* __LOC_SET_SERVER_H__ */