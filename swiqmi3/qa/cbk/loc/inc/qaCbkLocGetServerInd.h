/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 */

#ifndef __LOC_GET_SERVER_IND_H__
#define __LOC_GET_SERVER_IND_H__

#include "qaGobiApiCbk.h"

enum
{
    eTLV_GET_SERVER_STATUS = 0x01,
    eTLV_GET_SERVER_TYPE   = 0x02,
    eTLV_GET_SERVER_IPV4   = 0x10,
    eTLV_GET_SERVER_IPV6   = 0x11,
    eTLV_GET_SERVER_URL    = 0x12,
};

enum eQCWWANError UpkQmiCbkLocGetServerInd(
        BYTE                        *pMdmResp,
        QmiCbkLocGetServerInd       *pApiResp);

#endif /* __LOC_GET_SERVER_IND_H__ */
