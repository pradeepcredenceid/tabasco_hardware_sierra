
/*
 * \ingroup uim
 *
 * \file    qaUimSLQSUimGetServiceStatusReq.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaUimSLQSUimGetServiceStatus.c
 *
 * Copyright: © 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __UIM_SLQS_UIM_GET_SERVICE_STATUS_H__
#define __UIM_SLQS_UIM_GET_SERVICE_STATUS_H__

#include "qaGobiApiUim.h"

/* enum declarations */
/*
 * An enumeration of eUIM_SLQS_GET_SERVICE_STATUS_REQ request TLV IDs
 */
enum eUIM_SLQS_GET_SERVICE_STATUS_REQ
{
    eTLV_GS_SESSION_INFO  = 0x01,
    eTLV_CAP_MASK         = 0x02
};

/* enum declarations */
/*
 * An enumeration of eUIM_SLQS_GET_SERVICE_STATUS_RESP request TLV IDs
 */
enum eUIM_SLQS_GET_SERVICE_STATUS_RESP
{
    eTLV_GS_FDN_STATUS        = 0x10,
    eTLV_HIDDEN_KEY_STATUS = 0x11,
    eTLV_INDEX             = 0x12
};


/*
 * This structure contains the UimSLQSGetServiceStatus response parameters.
 */
struct QmiUimSLQSGetServiceStatusResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    UIMGetServiceStatusResp *pUIMGetServiceStatusResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiUimSlqsGetServiceStatus(
    WORD            *pMlength,
    BYTE            *pBuffer,
    UIMGetServiceStatusReq *pUIMGetServiceStatusReq);

extern enum eQCWWANError UpkQmiUimSlqsGetServiceStatus(
    BYTE                           *pMdmResp,
    struct QmiUimSLQSGetServiceStatusResp *pApiResp );

#endif /* __UIM_SLQS_UIM_GET_SERVICE_STATUS_H__ */
