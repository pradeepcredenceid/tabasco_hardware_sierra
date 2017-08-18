#include "SwiDataTypes.h"
#include "sludefs.h"
#include "qmudefs.h"
#include "qmerrno.h"
#include "qaQmiBasic.h"

enum
{
    eTLV_QOS_IDS = 0x01
};

enum
{
    eTLV_QOS_FLOW_IDX = 0x10,
    eTLV_QOS_FLOW_DATA_RATE = 0x12,
    eTLV_QOS_FLOW_BUCKET = 0x13,
    eTLV_QOS_FLOW_LATAENCY = 0x15,
    eTLV_QOS_FLOW_JITTER = 0x16,
    eTLV_QOS_FLOW_LTE_QCI = 0x1F,
};

enum eQCWWANError mBuildTlvQosIds(BYTE *pBuf, BYTE  *pParam);
enum eQCWWANError mBuildTlvId(BYTE *pBuf, BYTE  *pParam);
