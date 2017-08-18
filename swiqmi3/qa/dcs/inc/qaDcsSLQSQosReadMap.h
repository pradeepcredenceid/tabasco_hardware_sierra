/**
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */
extern enum eQCWWANError PkQmiDcsSLQSQosReadMap(
    WORD *pMlength,
    BYTE *pParamField,
    BYTE dscp);

extern enum eQCWWANError UpkQmiDcsSLQSQosReadMap(
    BYTE   *pMdmResp,
    struct qm_qmi_response_tlvs_values *pApiResp);

