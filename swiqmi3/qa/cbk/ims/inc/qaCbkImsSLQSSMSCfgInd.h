/*
 * \ingroup cbk
 *
 * \file    qaCbkImsSLQSSMSCfgInd.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaCbkImsSLQSSMSCfgInd.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_SMS_CONFIG_IND_H_
#define __IMS_SMS_CONFIG_IND_H_

#include "qaGobiApiIms.h"
#include "qaGobiApiCbk.h"

/* Constant Defs */
#define MAX_PHONE_CTXT_URI_LEN 255
/*
 * An enumeration of eQMI_IMS_SMS_CONFIG_IND response TLV IDs
 */
enum eQMI_IMS_SMS_CONFIG_IND
{
    eTLV_SMS_FOMT           = 0x10,
    eTLV_SMS_OVER_IP_NW_IND = 0x11,
    eTLV_PHN_CTXT_URI       = 0x12,
};

/*
 * eQMI_IMS_SMS_CONFIG_IND TLVs defined below
 */

/*
 * Name:    SMSFmtTlv
 *
 * Purpose: Structure used to store SMS Format TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          smsFormat  - SMS Format
 */
struct SMSFmtTlv
{
    BYTE TlvPresent;
    BYTE smsFormat;
};

/*
 * Name:    SMSoIPNwTlv
 *
 * Purpose: Structure used to SMS over IP network Indication Flag TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                         in the QMI response
 *
 *          smsoIPNW   - SMS over IP Network Indication Flag
 */
struct SMSoIPNwTlv
{
    BYTE TlvPresent;
    BYTE smsoIPNW;
};

/*
 * Name:    PhCtxtURITlv
 *
 * Purpose: Structure used to store Phone Context URI TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *          PhCtxtURI  - Phone Context URI
 */
struct PhCtxtURITlv
{
    BYTE TlvPresent;
    BYTE PhCtxtURI[MAX_PHONE_CTXT_URI_LEN];
};

/*
 * Name:    QmiCbkImsSMSConfigInd
 *
 * Purpose: Structure used to store SMS Config Indication Parameters.
 *
 * Members: SFTlv   - SMS Format Tlv
 *          SINTlv  - SMS Over IP NW Indication Flag Tlv
 *          PCURTlv - Phone Context URI Tlv
 *
 * Note:    None
 */
struct QmiCbkImsSMSConfigInd{
    struct SMSFmtTlv    SFTlv;
    struct SMSoIPNwTlv  SINTlv;
    struct PhCtxtURITlv PCURTlv;
};

/*
 * Prototypes
 */
enum eQCWWANError UpkQmiCbkImsSMSCfgInd(
    BYTE                         *pMdmResp,
    struct QmiCbkImsSMSConfigInd *pApiResp );

#endif /* __IMS_SMS_CONFIG_IND_H_ */
