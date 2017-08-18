/*
 * \ingroup sms
 *
 * \file    qaSmsSLQSGetMessageWaiting.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaSmsSLQSGetTransLayerInfo.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __SMS_SLQS_GET_MESSAGE_WAITING_H__
#define __SMS_SLQS_GET_MESSAGE_WAITING_H__

#include "qaGobiApiSms.h"

/* enum declarations */

/*
 * An enumeration of eQMI_SMS_SLQS_GET_MESSAGE_WAITING_RESP response TLV IDs
 */
enum eQMI_SMS_SLQS_GET_MESSAGE_WAITING_RESP
{
    eTLV_MESSAGE_WAITING_INFO = 0x01,
};

/*
 * This structure contains the SLQSGetMessageWaiting response parameters.
 */
struct QmiSmsSLQSGetMessageWaitingResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    getMsgWaitingInfo *pGetMsgWaitingResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiSmsSLQSGetMessageWaiting(
    WORD *pMlength,
    BYTE *pBuffer );

extern enum eQCWWANError UpkQmiSmsSLQSGetMessageWaiting(
    BYTE                                   *pMdmResp,
    struct QmiSmsSLQSGetMessageWaitingResp *pApiResp );

#endif /* __SMS_SLQS_GET_MESSAGE_WAITING_H__ */

