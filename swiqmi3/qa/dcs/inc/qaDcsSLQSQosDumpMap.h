/**
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */
extern enum eQCWWANError PkQmiDcsSLQSQosDumpMap(
    WORD *pMlength,
    BYTE *pParamField);

extern enum eQCWWANError UpkQmiDcsSLQSQosDumpMap(
    BYTE   *pMdmResp,
    struct qm_qmi_response_tlvs_values *pApiResp);


