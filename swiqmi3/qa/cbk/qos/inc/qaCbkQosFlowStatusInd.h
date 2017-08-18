/*
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 */

enum
{
    eTLV_STAT_INFO = 0x01,
    eTLV_REASON_CODE = 0x10
};

struct QmiCbkQosFlowStatusInd{
    ULONG   id;
    BYTE    status;
    BYTE    event;
    BYTE    reason;
};

enum eQCWWANError UpkQmiCbkQosFlowStatusInd (
    BYTE     *pMdmResp,
    struct QmiCbkQosFlowStatusInd *pAipResp );

