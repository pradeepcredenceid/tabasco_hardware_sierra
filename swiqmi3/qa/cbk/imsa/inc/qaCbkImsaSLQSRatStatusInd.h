/*
 * \ingroup cbk
 *
 * \file    qaCbkImsaSLQSRatStatusInd.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaCbkImsSLQSRatStatusInd.c
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMSA_RAT_STATUS_IND_H_
#define __IMSA_RAT_STATUS_IND_H

#include "qaGobiApiImsa.h"
#include "qaGobiApiCbk.h"

/*
 * An enumeration of eQMI_IMSA_RAT_HANDOVER_STATUS_IND response TLV IDs
 */
enum eQMI_IMSA_RAT_HANDOVER_STATUS_IND_TLV
{
    eTLV_RAT_HANDOVER_STATUS = 0x10
};

/*
 * eQMI_IMSA_RAT_HANDOVER_STATUS_IND TLV defined below
 */

/*
 * Name:    RatHandoverStatusTlv
 *
 * Purpose: Structure used to store RAT Handover Status TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *          RatHandoverStatus - RAT Handover Status
 *          SourceRAT - Source RAT Info
 *          TargetRAT - Target RAT Info
 *          ErrorCodeLen - Length of Error code string
 *          ErrorCodeData - Error Code String          
 */
struct RatHandoverStatusTlv
{
    BYTE TlvPresent;
    ULONG RatHandoverStatus;
    ULONG SourceRAT;
    ULONG TargetRAT;
    BYTE  ErrorCodeLen;
    BYTE  ErrorCodeData[256];
};

/*
 * Name:    QmiCbkImsaRatStatusInd
 *
 * Purpose: Structure used to store RAT Handover Status Indication Parameters.
 *
 * Members: RatHandoverStatusTlv
 *
 * Note:    None
 */
struct QmiCbkImsaRatStatusInd{
    struct RatHandoverStatusTlv  RatHandover;
};

/*
 * Prototypes
 */
enum eQCWWANError UpkQmiCbkImsaRatStatusInd(
    BYTE                            *pMdmResp,
    struct QmiCbkImsaRatStatusInd   *pApiResp );

#endif /* __IMSA_RAT_STATUS_IND_H_ */
