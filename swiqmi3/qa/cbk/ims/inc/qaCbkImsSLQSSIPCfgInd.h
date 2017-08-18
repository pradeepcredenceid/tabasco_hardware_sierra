/*
 * \ingroup cbk
 *
 * \file    qaCbkImsSLQSSIPCfgInd.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaCbkImsSLQSSIPCfgInd.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_SIP_CONFIG_IND_H_
#define __IMS_SIP_CONFIG_IND_H_

#include "qaGobiApiIms.h"
#include "qaGobiApiCbk.h"

/*
 * An enumeration of eQMI_IMS_SIP_CONFIG_IND response TLV IDs
 */
enum eQMI_IMS_SIP_CONFIG_IND
{
    eTLV_SIP_PORT_NO  = 0x10,
    eTLV_SIP_REGN_TMR = 0x11,
    eTLV_SUBSCR_TMR   = 0x12,
    eTLV_TIMR_T1      = 0x13,
    eTLV_TIMR_T2      = 0x14,
    eTLV_TIMR_TF      = 0x15,
    eTLV_SIGCOMP_EN   = 0x16,
};

/*
 * eQMI_IMS_SIP_CONFIG_IND TLVs defined below
 */

/*
 * Name:    SIPPortTlv
 *
 * Purpose: Structure used to store SIP Port Number TLV Value.
 *
 * Members: TlvPresent   - Boolean indicating the presence of the TLV
 *                         in the QMI response
 *
 *          SIPLocalPort - SIP local port
 */
struct SIPPortTlv
{
    BYTE TlvPresent;
    WORD SIPLocalPort;
};

/*
 * Name:    SIPRegnTmrTlv
 *
 * Purpose: Structure used to store SIP Regn Tmr TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *          tmrSIPRegn - SIP Registration Timer
 */
struct SIPRegnTmrTlv
{
    BYTE  TlvPresent;
    ULONG tmrSIPRegn;
};

/*
 * Name:    SubscrTmrTlv
 *
 * Purpose: Structure used to store Subscribe Timer TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *          subscrTmr  - Subscriber Timer
 */
struct SubscrTmrTlv
{
    BYTE  TlvPresent;
    ULONG subscrTmr;
};

/*
 * Name:    TmrT1Tlv
 *
 * Purpose: Structure used to store Timer T1 TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *          TmrT1      - Timer T1
 */
struct TmrT1Tlv
{
    BYTE  TlvPresent;
    ULONG tmrT1;
};

/*
 * Name:    TmrT2Tlv
 *
 * Purpose: Structure used to store Timer T2 TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *          TmrT2      - Timer T2
 */
struct TmrT2Tlv
{
    BYTE  TlvPresent;
    ULONG tmrT2;
};

/*
 * Name:    TmrTfTlv
 *
 * Purpose: Structure used to store Timer Tf TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *          TmrTf      - Timer Tf
 */
struct TmrTfTlv
{
    BYTE  TlvPresent;
    ULONG tmrTf;
};

/*
 * Name:    SigCompEnTlv
 *
 * Purpose: Structure used to store SigComp Status TLV Value.
 *
 * Members: TlvPresent - Boolean indicating the presence of the TLV
 *                       in the QMI response
 *
 *          SigCompEn  - SigComp Enabled
 */
struct SigCompEnTlv
{
    BYTE TlvPresent;
    BYTE SigCompEn;
};

/*
 * Name:    QmiCbkImsSIPConfigInd
 *
 * Purpose: Structure used to store SIP Config Indication Parameters.
 *
 * Members: SPTlv  - SIP Port Tlv
 *          SRTlv  - SIP Registration Timer Tlv
 *          STTlv  - Subscriber Timer Tlv
 *          TT1Tlv - Timer T1 Tlv
 *          TT2Tlv - Timer T2 Tlv
 *          TTfTlv - Timer Tf Tlv
 *          SCTlv  - SigComp Status Tlv
 *
 * Note:    None
 */
struct QmiCbkImsSIPConfigInd{
    struct SIPPortTlv    SPTlv;
    struct SIPRegnTmrTlv SRTlv;
    struct SubscrTmrTlv  STTlv;
    struct TmrT1Tlv      TT1Tlv;
    struct TmrT2Tlv      TT2Tlv;
    struct TmrTfTlv      TTfTlv;
    struct SigCompEnTlv  SCTlv;
};
/*
 * Prototypes
 */
enum eQCWWANError UpkQmiCbkImsSIPCfgInd(
    BYTE                         *pMdmResp,
    struct QmiCbkImsSIPConfigInd *pApiResp );

#endif /* __IMS_SIP_CONFIG_IND_H_ */
