/*
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 */

#ifndef __LOC_GET_FIX_CRITERIA_IND_H__
#define __LOC_GET_FIX_CRITERIA_IND_H__

#include "qaGobiApiCbk.h"

enum
{
    eTLV_GET_FIX_CRITERIA_STATUS = 0x01,
    eTLV_GET_FIX_CRITERIA_ACCURACY = 0x10,
    eTLV_GET_FIX_CRITERIA_INTERMEDIATE  = 0x11,
    eTLV_GET_FIX_CRITERIA_MIN_INTERVAL  = 0x12,
    eTLV_GET_FIX_CRITERIA_APP_ID        = 0x13,
};

enum eQCWWANError UpkQmiCbkLocGetFixCriteriaInd(
        BYTE                            *pMdmResp,
        QmiCbkLocGetFixCriteriaInd      *pApiResp);

#endif /* __LOC_GET_FIX_CRITERIA_IND_H__ */
