/**
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */
extern enum eQCWWANError PkQmiDcsSLQSQosEditMap(
    WORD *pMlength,
    BYTE *pParamField,
    BYTE dscp,
    ULONG qos_id );

extern enum eQCWWANError UpkQmiDcsSLQSQosEditMap(
    BYTE   *pMdmResp,
    struct qm_qmi_response_tlvs_values *pApiResp);
