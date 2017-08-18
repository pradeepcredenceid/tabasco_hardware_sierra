/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 */

#ifndef __LOC_SET_SERVER_IND_H__
#define __LOC_SET_SERVER_IND_H__

#include "qaGobiApiCbk.h"

enum
{
    eTLV_SET_SERVER_STATUS = 0x01
};

enum eQCWWANError UpkQmiCbkLocSetServerInd(
        BYTE                        *pMdmResp,
        QmiCbkLocSetServerInd       *pApiResp );        

#endif /* __LOC_SET_SERVER_IND_H__ */
