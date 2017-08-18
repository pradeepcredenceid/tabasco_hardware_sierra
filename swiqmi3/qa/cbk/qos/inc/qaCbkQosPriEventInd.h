/*
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 */

enum
{
    eTLV_EVENT = 0x01,
};

struct QmiCbkQosPriEventInd{
    WORD    event;
};

enum eQCWWANError UpkQmiCbkQosPriEventInd (
    BYTE     *pMdmResp,
    struct QmiCbkQosPriEventInd *pAipResp );

