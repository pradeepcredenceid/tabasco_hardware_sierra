/*
 * \ingroup cbk
 *
 * \file    qaCbkImsaSLQSSvcStatusInd.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaCbkImsSLQSSvcStatusInd.c
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMSA_SVC_STATUS_IND_H_
#define __IMSA_SVC_STATUS_IND_H

#include "qaGobiApiImsa.h"
#include "qaGobiApiCbk.h"

/*
 * An enumeration of eQMI_IMSA_SERVICE_STATUS_IND response TLV IDs
 */
enum eQMI_IMSA_SERVICE_STATUS_IND_TLV
{
    eTLV_SMS_SVC_STATUS = 0x10,
    eTLV_VOIP_SVC_STATUS = 0x11,
    eTLV_VT_SVC_STATUS = 0x12,
    eTLV_SMS_RAT = 0x13,
    eTLV_VOIP_RAT = 0x14,
    eTLV_VT_RAT = 0x15,
    eTLV_UT_SVC_STATUS = 0x16,
    eTLV_UT_RAT = 0x17
};

/*
 * eQMI_IMSA_SERVICE_STATUS_IND TLVs defined below
 */

/*
 * Name:    SmsSvcStatusTlv
 *
 * Purpose: Structure used to store Sms Service TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          SmsSvcStatus - Sms service status
 */
struct SmsSvcStatusTlv
{
    BYTE TlvPresent;
    ULONG SmsSvcStatus;
};

/*
 * Name:    VoipSvcStatusTlv
 *
 * Purpose: Structure used to store VOIP Service TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          VoipSvcStatus - Voip service status
 */
struct VoipSvcStatusTlv
{
    BYTE TlvPresent;
    ULONG VoipSvcStatus;
};

/*
 * Name:    VtSvcStatusTlv
 *
 * Purpose: Structure used to store VT Service TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          VtSvcStatus - VT service status
 */
struct VtSvcStatusTlv
{
    BYTE TlvPresent;
    ULONG VtSvcStatus;
};

/*
 * Name:    SmsRatTlv
 *
 * Purpose: Structure used to store Sms RAT TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          SmsRat - Sms service RAT
 */
struct SmsRatTlv
{
    BYTE TlvPresent;
    ULONG SmsRatVal;
};

/*
 * Name:    VoipRatTlv
 *
 * Purpose: Structure used to store Voip RAT TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          VoipRat - VOIP service RAT
 */
struct VoipRatTlv
{
    BYTE TlvPresent;
    ULONG VoipRatVal;
};

/*
 * Name:    VtRatTlv
 *
 * Purpose: Structure used to store VT RAT TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          VtRat - VT service RAT
 */
struct VtRatTlv
{
    BYTE TlvPresent;
    ULONG VtRatVal;
};

/*
 * Name:    UtSvcStatusTlv
 *
 * Purpose: Structure used to store UT Service TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          UtSvcStatus - VT service status
 */
struct UtSvcStatusTlv
{
    BYTE TlvPresent;
    ULONG UtSvcStatus;
};

/*
 * Name:    UtRatTlv
 *
 * Purpose: Structure used to store UT RAT TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          UtRat - UT service RAT
 */
struct UtRatTlv
{
    BYTE TlvPresent;
    ULONG UtRatVal;
};


/*
 * Name:    QmiCbkImsaSvcStatusInd
 *
 * Purpose: Structure used to store Service Status Indication Parameters.
 *
 * Members: SmsService
 *          VoipService
 *          VtService
 *          SmsRat
 *          VoipRat
 *          VtRat
 *          UtService
 *          UtRat
 * Note:    None
 */
struct QmiCbkImsaSvcStatusInd{
    struct SmsSvcStatusTlv    SmsService;
    struct VoipSvcStatusTlv   VoipService;
    struct VtSvcStatusTlv     VtService;
    struct SmsRatTlv          SmsRat;
    struct VoipRatTlv         VoipRat;
    struct VtRatTlv           VtRat;
    struct UtSvcStatusTlv     UtService;
    struct UtRatTlv           UtRat;
};

/*
 * Prototypes
 */
enum eQCWWANError UpkQmiCbkImsaSvcStatusInd(
    BYTE                            *pMdmResp,
    struct QmiCbkImsaSvcStatusInd   *pApiResp );

#endif /* __IMSA_SVC_STATUS_IND_H_ */
