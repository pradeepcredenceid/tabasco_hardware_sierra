#include "qaQosCommon.h"
#include "qaGobiApiQos.h"

enum eQCWWANError mBuildTlvQosIds(BYTE *pBuf, BYTE  *pParam)
{
    int i;
    swiQosIds  *pIds = (swiQosIds*)pParam;

    PutByte(pBuf, pIds->sz);

    for(i=0; i< pIds->sz; i++)
    {
        PutLong(pBuf, pIds->pIds[i]);
    }

    return eQCWWAN_ERR_NONE;
}

enum eQCWWANError mBuildTlvId(BYTE *pBuf, BYTE  *pParam)
{
    ULONG  *pId = (ULONG*)pParam;
    return PutLong(pBuf, *pId);
}

