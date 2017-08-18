/*
 * \ingroup ims
 *
 * \file qaImsSLQSSetImsVoIPConfig.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaImsSLQSSetImsVoIPConfig.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_SLQS_SET_IMS_VOIP_CONFIG_H__
#define __IMS_SLQS_SET_IMS_VOIP_CONFIG_H__

/* enum declarations */

/*
 * An enumeration of eQMI_IMS_SLQS_SET_IMS_VOIP_CONFIG request TLV IDs
 */
enum eQMI_IMS_SLQS_SET_IMS_VOIP_CONFIG_REQ
{
    eTLV_SESS_EXP_TIMER     = 0x10,
    eTLV_MIN_SESS_EXP       = 0x11,
    eTLV_AMR_WB_ENABLE      = 0x12,
    eTLV_SCR_AMR_ENABLE     = 0x13,
    eTLV_SCR_AMR_WB_ENABLE  = 0x14,
    eTLV_AMR_MODES          = 0x15,
    eTLV_AMR_WB_MODES       = 0x16,
    eTLV_AMR_OCTET_ALIGN    = 0x17,
    eTLV_AM_WB_OCTET_ALIGN  = 0x18,
    eTLV_RING_TIMER         = 0x19,
    eTLV_RINGBACK_TIMER     = 0x1A,
    eTLV_RTPRTCPINACTTIMER  = 0x1B,

};

/*
 * An enumeration of eQMI_IMS_SLQS_SET_IMS_VOIP_CONFIG response TLV IDs.
 * The settings resp TLV is common to eQMI_IMS_SLQS_SET_SIP_CONFIG.
 */

/*
 * This structure contains the SLQSSetIMSVoIPConfig request parameters.
 *
 * \sa qaGobiApiIms.h for parameter descriptions
 *
 */
struct QmiImsSLQSSetIMSVoIPConfigReq
{
    SetIMSVoIPConfigReq *pSetIMSVoIPConfigReq;
};

/*
 * This structure contains the SLQSSetIMSVoIPConfig response parameters.
 *
 */
struct QmiImsSLQSSetIMSVoIPConfigResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSSetIMSVoIPConfig response parameters */
    SetIMSVoIPConfigResp *pSetIMSVoIPConfigResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiImsSLQSSetIMSVoIPConfig (
    WORD                *pMlength,
    BYTE                *pParamField,
    SetIMSVoIPConfigReq *pSetIMSVoIPConfigReq );

extern enum eQCWWANError UpkQmiImsSLQSSetIMSVoIPConfig (
    BYTE                                  *pMdmResp,
    struct QmiImsSLQSSetIMSVoIPConfigResp *pSetIMSVoIPConfigResp );

#endif /* __IMS_SLQS_SET_IMS_VOIP_CONFIG_H__ */
