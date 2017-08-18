#include "qaQosCommon.h"
#include "qaGobiApiQos.h"
#include "qaQosReq.h"
#include <string.h> 

BYTE cached_qos_flow[255];


enum eQCWWANError BuildFilterIdx(BYTE *pBuf, BYTE  *pParam)
{
    UNUSEDPARAM(pParam);
    return PutByte(pBuf, 1);
}

enum eQCWWANError BuildFilterVer(BYTE *pBuf, BYTE  *pParam)
{
    UNUSEDPARAM(pParam);
    return PutByte(pBuf, eIPV4);
}

enum eQCWWANError BuildNestedTxFilter(BYTE *pBuf, BYTE  *pParam)
{
    UNUSEDPARAM(pParam);
    swi_uint8 nested_tlvs[255];
    swi_uint16 nested_tlvs_len;

    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_QOS_FILTER_IDX,    &BuildFilterIdx},
        { eTLV_QOS_FILTER_VER,    &BuildFilterVer},
        { eTLV_TYPE_INVALID, NULL }  /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };

    tlvbuild( nested_tlvs, NULL, map, &nested_tlvs_len );

    return PutStream(pBuf, nested_tlvs, nested_tlvs_len);
}

enum eQCWWANError BuildWrapNestedTxFilter(BYTE *pBuf, BYTE  *pParam)
{
    swiQosFlow  *pTxFlow = (swiQosFlow*)pParam;

    swi_uint8 nested_tlvs[255];
    swi_uint16 nested_tlvs_len;

    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { 0x10,    &BuildNestedTxFilter},
        { eTLV_TYPE_INVALID, NULL }  /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };

    tlvbuild( nested_tlvs, (BYTE*)pTxFlow, map, &nested_tlvs_len );

    return PutStream(pBuf, nested_tlvs, nested_tlvs_len);
}

enum eQCWWANError BuildFlowDataRate(BYTE *pBuf, BYTE  *pParam)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    swiQosFlow  *pTxFlow = (swiQosFlow*)pParam;

    if (!pTxFlow->pDataRate)
    {
        return eRCode;
    }

    eRCode = PutLong(pBuf, pTxFlow->pDataRate->dataRateMax);
    if (eRCode == eQCWWAN_ERR_NONE)
    {
        return PutLong(pBuf, pTxFlow->pDataRate->guaranteedRate);
    }
    else
    {
        return eRCode;
    }
}

enum eQCWWANError BuildFlowBucket(BYTE *pBuf, BYTE  *pParam)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    swiQosFlow  *pTxFlow = (swiQosFlow*)pParam;

    if (!pTxFlow->pTokenBucket)
    {
        return eRCode;
    }

    eRCode = PutLong(pBuf, pTxFlow->pTokenBucket->peakRate);
    if (eRCode == eQCWWAN_ERR_NONE)
    {
        eRCode = PutLong(pBuf, pTxFlow->pTokenBucket->tokenRate);
        if (eRCode == eQCWWAN_ERR_NONE)
        {
            return PutLong(pBuf, pTxFlow->pTokenBucket->bucketSz);
        }
        else
        {
            return eRCode;
        }
    }
    else
    {
        return eRCode;
    }
}

enum eQCWWANError BuildFlowLatency(BYTE *pBuf, BYTE  *pParam)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    swiQosFlow  *pTxFlow = (swiQosFlow*)pParam;
    if (!pTxFlow->pLatency)
    {
        return eRCode;
    }

    return PutLong(pBuf, *(pTxFlow->pLatency));
}
enum eQCWWANError BuildFlowJitter(BYTE *pBuf, BYTE  *pParam)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    swiQosFlow  *pTxFlow = (swiQosFlow*)pParam;
    if (!pTxFlow->pJitter)
    {
        return eRCode;
    }

    return PutLong(pBuf, *(pTxFlow->pJitter));
}
enum eQCWWANError BuildFlowLteQci(BYTE *pBuf, BYTE  *pParam)
{
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;
    swiQosFlow  *pTxFlow = (swiQosFlow*)pParam;
    if (!pTxFlow->pLteQci)
    {
        return eRCode;
    }

    return PutByte(pBuf, *(pTxFlow->pLteQci));
}

enum eQCWWANError BuildFlowIdx(BYTE *pBuf, BYTE  *pParam)
{
    swiQosFlow  *pTxFlow = (swiQosFlow*)pParam;
    return PutByte(pBuf, pTxFlow->index);
}

enum eQCWWANError BuildNestedTxFlow(BYTE *pBuf, BYTE  *pParam)
{
    swiQosReq *pQosReq = (swiQosReq*)pParam;

    swi_uint8 nested_tlvs[255];
    swi_uint16 nested_tlvs_len;

    if (!pQosReq->pTxFlow)
    {
        return eQCWWAN_ERR_INVALID_ARG;
    }

    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        //TODO prefixing to identify between mandatory & optional TLVs
        { eTLV_QOS_FLOW_IDX,    &BuildFlowIdx},
        { eTLV_QOS_FLOW_DATA_RATE,    &BuildFlowDataRate},
        { eTLV_QOS_FLOW_BUCKET,    &BuildFlowBucket},
        { eTLV_QOS_FLOW_LATAENCY,    &BuildFlowLatency},
        { eTLV_QOS_FLOW_JITTER,    &BuildFlowJitter},
        { eTLV_QOS_FLOW_LTE_QCI,    &BuildFlowLteQci},
        { eTLV_TYPE_INVALID, NULL }  /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };

    tlvbuild( nested_tlvs, (BYTE*)pQosReq->pTxFlow, map, &nested_tlvs_len );

    return PutStream(pBuf, nested_tlvs, nested_tlvs_len);
}

enum eQCWWANError BuildWrapNestedTxFlow(BYTE *pBuf, BYTE  *pParam)
{
    swiQosReq  *pQosReq = (swiQosReq*)pParam;
    swi_uint8 nested_tlvs[255];
    swi_uint16 nested_tlvs_len;

    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { 0x10,    &BuildNestedTxFlow},
        { eTLV_TYPE_INVALID, NULL }  /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };

    tlvbuild( nested_tlvs, (BYTE*)pQosReq, map, &nested_tlvs_len );

    /*
     * cache qos flow requested for simulating response handling
     */
    if (nested_tlvs_len < 0xff )
    {
        memcpy( cached_qos_flow, nested_tlvs, nested_tlvs_len);
    }


    return PutStream(pBuf, nested_tlvs, nested_tlvs_len);
}


enum eQCWWANError BuildNestedIdx(BYTE *pBuf, BYTE  *pParam)
{
    UNUSEDPARAM(pParam);
    return PutByte(pBuf, 1);
}

enum eQCWWANError BuildTlvQosSpec(
        BYTE *pBuf,
        BYTE *pParam
        )
{
    swiQosReq  *pQosReq = (swiQosReq*)pParam;
    swi_uint8 nested_tlvs[255];
    swi_uint16 nested_tlvs_len;

    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_QOS_NESTED_IDX,    &BuildNestedIdx},
        { eTLV_QOS_NESTED_TX_FLOW,    &BuildWrapNestedTxFlow},
        { eTLV_QOS_NESTED_TX_FILTER,    &BuildWrapNestedTxFilter},
        { eTLV_TYPE_INVALID, NULL }  /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };

    tlvbuild( nested_tlvs, (BYTE*)pQosReq, map, &nested_tlvs_len );

    return PutStream(pBuf, nested_tlvs, nested_tlvs_len);
}

enum eQCWWANError PkQmiQosReq (
    WORD    *pMlength,
    BYTE    *pParamField,
    swiQosReq *pQosReq
    )
{
    /* There is no mandatory TLV and hence the map is empty */
    static struct qmTlvBuilderItem map[] =
    {
        { eTLV_QOS_SPEC,    &BuildTlvQosSpec},
        { eTLV_TYPE_INVALID, NULL }  /* Important. Sentinel.
                                      * Signifies last item in map.
                                      */
    };

    enum eQCWWANError eRCode;

    eRCode = qmbuild( pParamField,
                      (BYTE*)pQosReq,
                      map,
                      eQMI_QOS_REQ,
                      pMlength );

    return eRCode;
}

static enum eQCWWANError mUnpackIds(
    BYTE* pTlvData,
    BYTE *pResp )
{
    BYTE nIds;
    struct QmiQosRequestResp *lResp =
              (struct QmiQosRequestResp *)pResp;
    enum eQCWWANError eRCode = eQCWWAN_ERR_NONE;

    eRCode = GetByte( pTlvData, &nIds);

    if ( (eRCode == eQCWWAN_ERR_NONE)
            && (lResp->pQosIds->sz >= nIds) )
    { 
        int i;
        lResp->pQosIds->sz = nIds;
        for(i=0;i<nIds;i++)
        {
            eRCode = GetLong( pTlvData, &lResp->pQosIds->pIds[i]);
            if (eRCode != eQCWWAN_ERR_NONE)
            {
                return eRCode;
            }
        }
    }

    return eRCode;
}

enum eQCWWANError UpkQmiQosRequest(
    BYTE    *pMdmResp,
    struct  QmiQosRequestResp *pApiResp )
{
    enum eQCWWANError eRCode;

    //TODO simulate there aer qos ids returned
    static struct qmTlvUnpackerItem map[] =
    {
        { eTLV_RESULT_CODE,             &qmUnpackTlvResultCode },
        { eTLV_QOS_IDS,                 &mUnpackIds },
        { eTLV_TYPE_INVALID,            NULL }  /* Important. Sentinel.
                                                 * Signifies last item in map.
                                                 */
    };

    eRCode = qmunpackresp( pMdmResp,
                          (BYTE*)pApiResp,
                          map,
                          eQMI_QOS_REQ );
    return eRCode;
}

