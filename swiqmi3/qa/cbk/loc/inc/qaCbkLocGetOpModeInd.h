/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 */

#ifndef __LOC_GET_OP_MODE_IND_H__
#define __LOC_GET_OP_MODE_IND_H__

#include "qaGobiApiCbk.h"

enum
{
    eTLV_GET_OP_MODE_STATUS = 0x01,
    eTLV_GET_OP_MODE_CURMODE = 0x10,
};

enum eQCWWANError UpkQmiCbkLocGetOpModeInd(
        BYTE                        *pMdmResp,
        QmiCbkLocGetOpModeInd       *pApiResp);

#endif /* __LOC_GET_OP_MODE_IND_H__ */
