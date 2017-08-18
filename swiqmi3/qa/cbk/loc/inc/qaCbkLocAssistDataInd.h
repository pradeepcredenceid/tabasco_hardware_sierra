/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 */

#ifndef __LOC_ASSIST_DATA_IND_H__
#define __LOC_ASSIST_DATA_IND_H__

#include "qaGobiApiCbk.h"

enum
{
    eTLV_ASSIST_DATA_STATUS = 0x01
};

enum eQCWWANError UpkQmiCbkLocAssistDataInd(
        BYTE                              *pMdmResp,
        delAssistDataStatus *pStatus);

#endif /* __LOC_ASSIST_DATA_IND_H__ */
