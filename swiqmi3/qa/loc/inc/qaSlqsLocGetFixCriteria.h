/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_GET_FIX_CRITERIA_H__
#define __LOC_GET_FIX_CRITERIA_H__

#include "qaGobiApiLoc.h"


struct QmiLocGetFixCriteriaResp
{
    struct qmTlvResult results;
};
                         
extern enum eQCWWANError PkQmiLocGetFixCriteriaReq(
            WORD                *pMlength,
            BYTE                *pParamField);

extern enum eQCWWANError UpkQmiLocGetFixCriteriaResp(
            BYTE                               *pMdmResp,
            struct QmiLocGetFixCriteriaResp    *pApiResp);

#endif /* __LOC_GET_FIX_CRITERIA_H__ */