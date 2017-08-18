/*
 * \ingroup uim
 *
 * \file    qaUimSLQSUimWriteRecord.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaUimSLQSUimWriteRecord.c
 *
 * Copyright: � 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __UIM_SLQS_UIM_WRITE_RECORD_H__
#define __UIM_SLQS_UIM_WRITE_RECORD_H__

#include "qaGobiApiUim.h"

/* enum declarations */
/*
 * An enumeration of eUIM_SLQS_WRITE_RECORD_REQ request TLV IDs
 */
enum eUIM_SLQS_WRITE_RECORD_REQ
{
    eTLV_WRITE_RECORD_SESS_INFO = 0x01,
    eTLV_WRITE_RECORD_FILE_ID   = 0x02,
    eTLV_WRITE_RECORD_INFO      = 0x03,
    eTLV_WRITE_RECORD_RESP_IND  = 0x10
};

/*
 * An enumeration of eUIM_SLQS_WRITE_RECORD_RESP request TLV IDs
 */
enum eeUIM_SLQS_WRITE_RECORD_RESP
{
    eTLV_WRITE_RECORD_CARD_RESULT      = 0x10,
    eTLV_WRITE_RECORD_RESP_IND_RESULT  = 0x11
};

/*
 * This structure contains the UimSLQSWriteRecord response parameters.
 */
struct QmiUimSLQSWriteRecordResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    UIMWriteRecordResp *pUIMWriteRecordResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiUimSlqsWriteRecord(
    WORD                    *pMlength,
    BYTE                    *pBuffer,
    UIMWriteRecordReq       *pUIMWriteRecordReq);

extern enum eQCWWANError UpkQmiUimSlqsWriteRecord(
    BYTE                             *pMdmResp,
    struct QmiUimSLQSWriteRecordResp *pApiResp );

#endif /* __UIM_SLQS_UIM_WRITE_RECORD_H__ */
