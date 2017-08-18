/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_GET_OP_MODE_H__
#define __LOC_GET_OP_MODE_H__

#include "qaGobiApiLoc.h"


struct QmiLocGetOpModeResp
{
    struct qmTlvResult results;
};

extern enum eQCWWANError PkQmiLocGetOpModeReq(
            WORD                *pMlength,
            BYTE                *pParamField);

extern enum eQCWWANError UpkQmiLocGetOpModeResp(
            BYTE                          *pMdmResp,
            struct QmiLocGetOpModeResp    *pApiResp);

#endif /* __LOC_GET_OP_MODE_H__ */