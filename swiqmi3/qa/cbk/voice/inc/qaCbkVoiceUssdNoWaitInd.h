/*
 * \ingroup cbk
 *
 * \file    qaCbkVoiceUssdNoWaitInd.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaCbkVoiceUssdNoWaitInd.c
 *
 * Copyright: © 2011 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __VOICE_USSD_NO_WAIT_IND_H_
#define __VOICE_USSD_NO_WAIT_IND_H_

#include "qaGobiApiVoice.h"

/*
 * An enumeration of eQMI_VOICE_USSD_NO_WAIT_IND response TLV IDs
 */
enum eQMI_VOICE_USSD_IND_NO_WAIT
{
    eTLV_USSD_ERROR             = 0x10,
    eTLV_USSD_FAILURE_CAUSE     = 0x11,
    eTLV_USSD_DATA_FROM_NETWORK = 0x12,
    eTLV_USSD_ALPHA_IDENTIFIER  = 0x13,
};

/*
 * eQMI_VOICE_USSD_NO_WAIT_IND TLVs defined below
 */

/*
 * Name:    VoiceUssdNoWaitErrorTlv
 *
 * Purpose: Structure used to store Error TLV Value
 *
 * Members: TlvPresent         - Boolean indicating the presence of the TLV
 *                               in the QMI response
 *
 *          Error              - Type of error
 *
 * Note:    None
 */
struct VoiceUssdNoWaitErrorTlv
{
    BYTE             TlvPresent;
    BYTE             Error;
};

/*
 * Name:    VoiceUssdNoWaitFailCauseTlv
 *
 * Purpose: Structure used to store FailCause TLV Value
 *
 * Members: TlvPresent         - Boolean indicating the presence of the TLV
 *                               in the QMI response
 *
 *          Failure_Cause      - Supplementary services failure cause
 *
 * Note:    None
 */
struct VoiceUssdNoWaitFailCauseTlv
{
    BYTE             TlvPresent;
    BYTE             Failure_Cause;
};

/*
 * Name:    VoiceUssdNoWaitUSSINFOTlv
 *
 * Purpose: Structure used to store USSD Infor TLV Value
 *
 * Members: TlvPresent         - Boolean indicating the presence of the TLV
 *                               in the QMI response
 *
 *          NetworkInfo        - USSD information from the network
 *                               See qaGobiApiVoice.h for more info.
 *
 * Note:    None
 */
struct VoiceUssdNoWaitUSSINFOTlv
{
    BYTE           TlvPresent;
    struct USSInfo NetworkInfo;
};

/*
 * Name:    VoiceUssdNoWaitAlphaIDTlv
 *
 * Purpose: Structure used to store Alpha identifier TLV Value
 *
 * Members: TlvPresent         - Boolean indicating the presence of the TLV
 *                               in the QMI response
 *
 *          Alpha_Identifier   - Alpha coding scheme. Values:
 *                               0x01 – ALPHA_DCS_GSM – GSM Default_Char
 *                               0x02 – ALPHA_DCS_UCS2 – UCS2
 *
 * Note:    None
 */
struct VoiceUssdNoWaitAlphaIDTlv
{
    BYTE             TlvPresent;
    alphaIDInfo      Alpha_Identifier;

};

/*
 * Name:    QmiCbkVoiceUssdIndNoWait
 *
 * Purpose: Structure used to store all Notification Parameters.
 *
 * Members: VSTlv - USSD Notification Tlv
 *
 * Note:    None
 */
struct QmiCbkVoiceUssdIndNoWait{
    struct VoiceUssdNoWaitErrorTlv     NwETlv;
    struct VoiceUssdNoWaitFailCauseTlv FCTlv;
    struct VoiceUssdNoWaitUSSINFOTlv   UInfoTLV;
    struct VoiceUssdNoWaitAlphaIDTlv   AIDTlv;
};

/*
 * Prototypes
 */
enum eQCWWANError UpkQmiCbkVoiceUssdIndNoWait(
    BYTE                            *pMdmResp,
    struct QmiCbkVoiceUssdIndNoWait *pApiResp );

#endif /* __VOICE_USSD_NO_WAIT_IND_H_ */
