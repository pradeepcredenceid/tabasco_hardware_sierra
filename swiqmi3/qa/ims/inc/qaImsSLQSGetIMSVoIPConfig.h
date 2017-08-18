/*
 * \ingroup ims
 *
 * \file qaImsSLQSGetVoIPConfig.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaImsSLQSGetVoIPConfig.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_SLQS_GET_VOIP_CONFIG_H__
#define __IMS_SLQS_GET_VOIP_CONFIG_H__

/* enum declarations */

/*
 * An enumeration of eQMI_IMS_SLQS_GET_VOIP_CONFIG response TLV IDs
 */
enum eQMI_IMS_SLQS_GET_VOIP_CONFIG_RESP
{
    eTLV_SETTING_RSP         = 0x10,
    eTLV_SESS_EXPIRY_TMR     = 0x11,
    eTLV_MIN_SESS_EXPIRY_TMR = 0x12,
    eTLV_AMR_WB_ENAB         = 0x13,
    eTLV_ENABLE_SCR_AMR      = 0x14,
    eTLV_ENABLE_SCR_AMR_WB   = 0x15,
    eTLV_AMR_MODE            = 0x16,
    eTLV_AMR_WB_MODE         = 0x17,
    eTLV_AMR_OCTET_ALGN      = 0x18,
    eTLV_AMR_WB_OCTET_ALIGN  = 0x19,
    eTLV_RINGING_TIMER       = 0x1A,
    eTLV_RINGBK_TIMER        = 0x1B,
    eTLV_RTPRTCP_TIMER       = 0x1C,
};

/*
 * This structure contains the SLQSGetIMSVoIPConfig response parameters.
 *
 */
struct QmiImsSLQSGetIMSVoIPConfigResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSGetIMSVoIPConfig response parameters */
    GetIMSVoIPConfigResp *pGetIMSVoIPConfigResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiImsSLQSGetIMSVoIPConfig (
    WORD *pMlength,
    BYTE *pParamField );

extern enum eQCWWANError UpkQmiImsSLQSGetIMSVoIPConfig (
    BYTE                                  *pMdmResp,
    struct QmiImsSLQSGetIMSVoIPConfigResp *pGetIMSVoIPConfigResp );

#endif /* __IMS_SLQS_GET_VOIP_CONFIG_H__ */
