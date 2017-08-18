/*
 * \ingroup nas
 *
 * \file    qaNasSLQSSet3GPP2Subscription.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaNasSLQSSet3GPP2Subscription.c
 *
 * Copyright: © 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __NAS_SLQS_SET_3GPP2_SUBSCRIPTION_H__
#define __NAS_SLQS_SET_3GPP2_SUBSCRIPTION_H__

#include "qaGobiApiNas.h"

/* enum declarations */
/*
 * An enumeration of eNAS_SLQS_SET_3GPP2_SUBSCRIPTION_INFO_REQ response
 * TLVIDs
 */
enum eNAS_SLQS_SET_3GPP2_SUBSCRIPTION_REQ
{
    eTLV_NAM_ID_SET_REQ          = 0x01,
    eTLV_MDN_3GPP2_SET_INFO          = 0x16
};


/*
 * This structure contains the SLQSNas3GPP2SubscriptionInfo response parameters.
 */
struct QmiNasSLQSSet3GPP2SubscriptionInfoResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiNasSlqsSet3GPP2SubscriptionInfo(
    WORD                           *pMlength,
    BYTE                           *pBuffer,
    nasSet3GPP2SubscriptionInfoReq *pSet3GPP2SubsInfoReq );

extern enum eQCWWANError UpkQmiNasSlqsSet3GPP2SubscriptionInfo(
    BYTE                                          *pMdmResp,
    struct QmiNasSLQSSet3GPP2SubscriptionInfoResp *pApiResp );

#endif /*__NAS_SLQS_SET_3GPP2_SUBSCRIPTION_H__*/
