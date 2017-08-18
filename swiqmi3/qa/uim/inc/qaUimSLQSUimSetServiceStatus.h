/**
 * \ingroup uim
 *
 * \file    qaUimSLQSUimSetServiceStatusReq.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaUimSLQSUimSetServiceStatus.c
 *
 * Copyright: © 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __UIM_SLQS_UIM_SET_SERVICE_STATUS_H__
#define __UIM_SLQS_UIM_SET_SERVICE_STATUS_H__

#include "qaGobiApiUim.h"

/* enum declarations */
/*
 * An enumeration of eUIM_SLQS_SET_SERVICE_STATUS_REQ request TLV IDs
 */
enum eUIM_SLQS_SET_SERVICE_STATUS_REQ
{
    eTLV_SS_SESSION_INFO   = 0x01,
    eTLV_SS_FDN_STATUS     = 0x10,
};


/*
 * This structure contains the UimSLQSSetServiceStatus response parameters.
 */
struct QmiUimSLQSSetServiceStatusResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiUimSlqsSetServiceStatus(
    WORD            *pMlength,
    BYTE            *pBuffer,
    UIMSetServiceStatusReq *pUIMSetServiceStatusReq);

extern enum eQCWWANError UpkQmiUimSlqsSetServiceStatus(
    BYTE                           *pMdmResp,
    struct QmiUimSLQSSetServiceStatusResp *pApiResp );

#endif /* __UIM_SLQS_UIM_SET_SERVICE_STATUS_H__ */
