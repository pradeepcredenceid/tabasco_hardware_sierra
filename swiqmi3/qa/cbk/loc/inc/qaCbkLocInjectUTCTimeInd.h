/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 */

#ifndef __LOC_INJECT_UTC_TIME_IND_H__
#define __LOC_INJECT_UTC_TIME_IND_H__

#include "qaGobiApiCbk.h"

enum
{
    eTLV_INJECT_UTC_TIME_STATUS = 0x01
};

enum eQCWWANError UpkQmiCbkLocInjectUTCTimeInd(
        BYTE                        *pMdmResp,
        QmiCbkLocInjectUTCTimeInd   *pApiResp);
        
#endif /* __LOC_INJECT_UTC_TIME_IND_H__ */
