/*
 * \ingroup cbk
 *
 * \file    qaCbkWmsMessageWaitingInd.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaCbkWmsMessageWaitingInd.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __WMS_MESSAGE_WAITING_IND_H__
#define __WMS_MESSAGE_WAITING_IND_H__

/* An enumeration of eQMI_WMS_MESSAGE_WAITING_IND response TLV IDs */
enum eQMI_WMS_MESSAGE_WAITING_IND_TLV
{
    eTLV_MESSAGE_WAITING_INFO = 0x01
};

/*
 * Prototypes
 */
enum eQCWWANError UpkQmiCbkWmsMessageWaitingInfoInd(
    BYTE          *pMdmResp,
    msgWaitingInfo *pApiResp );

#endif /* __WMS_MESSAGE_WAITING_IND_H__ */
