/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 */

#ifndef __LOC_OP_MODE_IND_H__
#define __LOC_OP_MODE_IND_H__

#include "qaGobiApiCbk.h"

enum
{
    eTLV_OP_MODE = 0x01
};

enum eQCWWANError UpkQmiCbkLocOpModeInd(
        BYTE                              *pMdmResp,
        ULONG *mode);

#endif /* __LOC_OP_MODE_IND_H__ */
