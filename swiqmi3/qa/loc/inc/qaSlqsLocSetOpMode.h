/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_SET_OP_MODE_H__
#define __LOC_SET_OP_MODE_H__

#include "qaGobiApiLoc.h"

enum
{
    eTLV_SET_OP_MODE        = 0x01,
};

struct QmiLocSetOpModeResp
{
    struct qmTlvResult results;
};

extern enum eQCWWANError PkQmiLocSetOpMode(
            WORD                *pMlength,
            BYTE                *pParamField,
            ULONG               mode );

extern enum eQCWWANError UpkQmiLocSetOpMode(
            BYTE                              *pMdmResp,
            struct QmiLocSetOpModeResp        *pApiResp);

#endif /* __LOC_SET_OP_MODE_H__ */
