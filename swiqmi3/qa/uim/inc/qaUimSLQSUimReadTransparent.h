/*
 * \ingroup uim
 *
 * \file    qaUimSLQSUimReadTransparent.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaUimSLQSUimReadTransparent.c
 *
 * Copyright: � 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __UIM_SLQS_UIM_READ_TRANSPARENT_H__
#define __UIM_SLQS_UIM_READ_TRANSPARENT_H__

#include "qaGobiApiUim.h"

/* enum declarations */
/*
 * An enumeration of eUIM_SLQS_READ_TRANSPARENT_REQ request TLV IDs
 */
enum eUIM_SLQS_READ_TRANSPARENT_REQ
{
    eTLV_GET_RT_SESS_INFO      = 0x01,
    eTLV_GET_RT_FILE_ID        = 0x02,
    eTLV_GET_READ_TRANSPARENT  = 0x03,
    eTLV_GET_RT_RESP_IND       = 0x10,
    eTLV_GET_RT_ENCRYPT_DATA   = 0x11
};

/*
 * An enumeration of eUIM_SLQS_READ_TRANSPARENT_RESP request TLV IDs
 */
enum eUIM_SLQS_READ_TRANSPARENT_RESP
{
    eTLV_GET_RT_CARD_RESULT    = 0x10,
    eTLV_GET_RT_RESULT         = 0x11,
    eTLV_GET_RT_RESP_IND_RESP  = 0x12,
    eTLV_GET_RT_ENCRYPTED_DATA = 0x13
};

/*
 * This structure contains the UimSLQSReadTransparent response parameters.
 */
struct QmiUimSLQSReadTransparentResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    UIMReadTransparentResp *pUIMReadTransparentResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiUimSlqsReadTransparent(
    WORD                    *pMlength,
    BYTE                    *pBuffer,
    UIMReadTransparentReq *pUIMReadTransparentReq);

extern enum eQCWWANError UpkQmiUimSlqsReadTransparent(
    BYTE                                   *pMdmResp,
    struct QmiUimSLQSReadTransparentResp *pApiResp );

#endif /* __UIM_SLQS_UIM_READ_TRANSPARENT_ATTRIBUTES_H__ */
