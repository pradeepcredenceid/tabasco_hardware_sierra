/*
 * \ingroup ims
 *
 * \file qaImsSLQSSetSIPConfig.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaImsSLQSSetSIPConfig.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_SLQS_SET_SIP_CONFIG_H__
#define __IMS_SLQS_SET_SIP_CONFIG_H__

/* enum declarations */

/*
 * An enumeration of eQMI_IMS_SLQS_SET_SIP_CONFIG request TLV IDs
 */
enum eQMI_IMS_SLQS_SET_SIP_CONFIG_REQ
{
    eTLV_SIP_PORT_NUM    = 0x10,
    eTLV_TIMER_REG_SIP   = 0x11,
    eTLV_SUBSCRIBE_TIMER = 0x12,
    eTLV_TIMER_T1        = 0x13,
    eTLV_TIMER_T2        = 0x14,
    eTLV_TIMER_TF        = 0x15,
    eTLV_SIGCOMP_STATUS  = 0x16,
};

/*
 * An enumeration of eQMI_IMS_SLQS_SET_SIP_CONFIG response TLV IDs
 */
enum eQMI_IMS_SLQS_SET_SIP_CONFIG_RESP
{
    eTLV_SETTINGS_RESP = 0x10,
};

/*
 * This structure contains the SLQSSetSIPConfig request parameters.
 *
 * \sa qaGobiApiIms.h for parameter descriptions
 *
 */
struct QmiImsSLQSSetSIPConfigReq
{
    SetSIPConfigReq *pSetSIPConfigReq;
};

/*
 * This structure contains the SLQSSetSIPConfig response parameters.
 *
 */
struct QmiImsSLQSSetSIPConfigResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSSetSIPConfig response parameters */
    SetSIPConfigResp *pSetSIPConfigResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiImsSLQSSetSIPConfig (
    WORD            *pMlength,
    BYTE            *pParamField,
    SetSIPConfigReq *pSetSIPConfigReq );

extern enum eQCWWANError UpkQmiImsSLQSSetSIPConfig (
    BYTE                              *pMdmResp,
    struct QmiImsSLQSSetSIPConfigResp *pSetSIPConfigResp );

#endif /* __IMS_SLQS_SET_SIP_CONFIG_H__ */
