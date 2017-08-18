/*
 * \ingroup cbk
 *
 * \file    qaCbkQosNetworkStatusInd.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaCbkQosNetworkStatusInd.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __QOS_NETWORK_STATUS_IND_H__
#define __QOS_NETWORK_STATUS_IND_H__

/* An enumeration of eQMI_QOS_NETWORK_STATUS_IND response TLV IDs */
enum eQMI_QOS_NETWORK_SUPPORT_STATUS_IND_TLV
{
    eTLV_NETWORK_QOS_SUPPORT_STATUS    = 0x01,
};

/*
 * Name:    QmiCbkQosNetworkStatusInd
 *
 * Purpose: Structure used to store all QMI Notification parameters.
 *
 * Members: Status                - Network QoS support status
 *
 * Note:    None
 */
struct QmiCbkQosNetworkStatusInd{
    BYTE Status;
};

/*************
 * Prototypes
 **************/
enum eQCWWANError UpkQmiCbkQosNetworkStatusInd (
    BYTE     *pMdmResp,
    struct QmiCbkQosNetworkStatusInd *pAipResp );

#endif /* __QOS_NETWORK_STATUS_IND_H__ */

