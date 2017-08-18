/*
 * This structure contains the Get Network Status response parameters.
 *
 * \sa qaGobiApiQos.h for parameter descriptions
 *
 */

enum
{
    eTLV_QOS_SPEC = 0x10,
    eTLV_QOS_OPCODE = 0x11,
};

enum
{
    eTLV_QOS_NESTED_IDX = 0x10,
    eTLV_QOS_NESTED_TX_FLOW = 0x11,
    eTLV_QOS_NESTED_RX_FLOW = 0x12,
    eTLV_QOS_NESTED_TX_FILTER = 0x13,
    eTLV_QOS_NESTED_RX_FILTER = 0x14
};

enum
{
    eTLV_QOS_FILTER_IDX = 0x10,
    eTLV_QOS_FILTER_VER = 0x11
};

enum
{
    eIPV4 = 0x04,
    eIPV6 = 0x06
};

struct QmiQosRequestResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* Response parameters */
    swiQosIds   *pQosIds;
};

enum eQCWWANError PkQmiQosReq (
    WORD    *pMlength,
    BYTE    *pParamField,
    swiQosReq *pQosReq
    );

enum eQCWWANError UpkQmiQosRequest (
    BYTE     *pMdmResp,
    struct   QmiQosRequestResp *pAipResp );
