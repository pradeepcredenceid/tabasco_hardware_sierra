/*
 * \ingroup uim
 *
 * \file    qaUimSLQSUimReadRecord.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaUimSLQSUimReadRecord.c
 *
 * Copyright: � 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __UIM_SLQS_UIM_READ_RECORD_H__
#define __UIM_SLQS_UIM_READ_RECORD_H__

#include "qaGobiApiUim.h"

/* enum declarations */
/*
 * An enumeration of eUIM_SLQS_READ_RECORD_REQ request TLV IDs
 */
enum eUIM_SLQS_READ_RECORD_REQ
{
    eTLV_GET_RR_SESS_INFO = 0x01,
    eTLV_GET_RR_FILE_ID   = 0x02,
    eTLV_GET_READ_RECORD  = 0x03,
    eTLV_GET_LAST_RECORD  = 0x10,
    eTLV_GET_RR_RESP_IND  = 0x11
};

/*
 * An enumeration of eUIM_SLQS_READ_RECORD_RESP request TLV IDs
 */
enum eUIM_SLQS_READ_RECORD_RESP
{
    eTLV_GET_RR_CARD_RESULT   = 0x10,
    eTLV_GET_READ_RESULT  = 0x11,
    eTLV_GET_ADDITIONAL_READ_RESULT = 0x12,
    eTLV_GET_RR_RESP_IND_RESP = 0x13
};

/*
 * This structure contains the UimSLQSReadRecord response parameters.
 */
struct QmiUimSLQSReadRecordResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    UIMReadRecordResp *pUIMReadRecordResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiUimSlqsReadRecord(
    WORD                    *pMlength,
    BYTE                    *pBuffer,
    UIMReadRecordReq *pUIMReadRecordReq);

extern enum eQCWWANError UpkQmiUimSlqsReadRecord(
    BYTE                                   *pMdmResp,
    struct QmiUimSLQSReadRecordResp *pApiResp );

#endif /* __UIM_SLQS_UIM_READ_RECORD_H__ */
