/**
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 */

#include "qaGobiApiQos.h"

enum
{
    eTLV_NW_PROF  = 0x10,
    eTLV_EXT_ERR  = 0xE0,
};

struct QmiQosGetNWProfResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
    BYTE        *pSz;
    NWProfile   *pProfile;
};

enum eQCWWANError PkQmiQosGetNWProf(
    WORD    *pMlength,
    BYTE    *pParamField );

enum eQCWWANError UpkQmiQosGetNWProf(
    BYTE   *pMdmResp,
    struct  QmiQosGetNWProfResp *pApiResp);

