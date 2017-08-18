/**
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 */

struct QmiQosResetResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

enum eQCWWANError PkQmiQosReset(
    WORD    *pMlength,
    BYTE    *pParamField );

enum eQCWWANError UpkQmiQosReset(
    BYTE   *pMdmResp,
    struct  QmiQosResetResp *pApiResp);
