/*
 * \ingroup uim
 *
 * \file    qaUimSLQSUimWriteTransparent.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaUimSLQSUimWriteTransparent.c
 *
 * Copyright: © 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __UIM_SLQS_UIM_WRITE_TRANSPARENT_H__
#define __UIM_SLQS_UIM_WRITE_TRANSPARENT_H__

#include "qaGobiApiUim.h"

/* enum declarations */
/*
 * An enumeration of eUIM_SLQS_WRITE_TRANSPARENT_REQ request TLV IDs
 */
enum eUIM_SLQS_WRITE_TRANSPARENT_REQ
{
    eTLV_GET_WT_SESS_INFO      = 0x01,
    eTLV_GET_WT_FILE_ID        = 0x02,
    eTLV_GET_WRITE_TRANSPARENT = 0x03,
    eTLV_GET_WT_RESP_IND       = 0x10
};

/*
 * An enumeration of eUIM_SLQS_WRITE_TRANSPARENT_RESP request TLV IDs
 */
enum eUIM_SLQS_WRITE_TRANSPARENT_RESP
{
    eTLV_WT_CARD_RESULT   = 0x10,
    eTLV_WT_RESP_IND_RESP = 0x11
};

/*
 * This structure contains the UimSLQSWriteTransparent response parameters.
 */
struct QmiUimSLQSWriteTransparentResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    UIMWriteTransparentResp *pUIMWriteTransparentResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiUimSlqsWriteTransparent(
    WORD               *pMlength,
    BYTE               *pBuffer,
    UIMWriteTransparentReq *pUIMWriteTransparentReq);

extern enum eQCWWANError UpkQmiUimSlqsWriteTransparent(
    BYTE                                  *pMdmResp,
    struct QmiUimSLQSWriteTransparentResp *pApiResp );

#endif /* __UIM_SLQS_UIM_WRITE_TRANSPARENT_H__ */
