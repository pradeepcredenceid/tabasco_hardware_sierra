/*
 * \ingroup ims
 *
 * \file qaImsSLQSSetIMSSMSConfig.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaImsSLQSSetIMSSMSConfig.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_SLQS_SET_IMS_SMS_CONFIG_H__
#define __IMS_SLQS_SET_IMS_SMS_CONFIG_H__

/* enum declarations */

/*
 * An enumeration of eQMI_IMS_SLQS_SET_IMS_SMS_CONFIG request TLV IDs
 */
enum eQMI_IMS_SLQS_SET_IMS_SMS_CONFIG_REQ
{
    eTLV_SMS_FORMAT         = 0x10,
    eTLV_SMS_IP_NW_IND_FLAG = 0x11,
    eTLV_PHONE_CTXT_URI     = 0x12,
};

/*
 * An enumeration of eQMI_IMS_SLQS_SET_REG_MGR_CONFIG response TLV IDs.
 * The settings resp TLV is common to eQMI_IMS_SLQS_SET_SIP_CONFIG.
 */

/*
 * This structure contains the SLQSSetIMSSMSConfig request parameters.
 *
 * \sa qaGobiApiIms.h for parameter descriptions
 *
 */
struct QmiImsSLQSSetIMSSMSConfigReq
{
    SetIMSSMSConfigReq *pSetIMSSMSConfigReq;
};

/*
 * This structure contains the SLQSSetIMSSMSConfig response parameters.
 *
 */
struct QmiImsSLQSSetIMSSMSConfigResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSSetIMSSMSConfig response parameters */
    SetIMSSMSConfigResp *pSetIMSSMSConfigResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiImsSLQSSetIMSSMSConfig (
    WORD               *pMlength,
    BYTE               *pParamField,
    SetIMSSMSConfigReq *pSetIMSSMSConfigReq );

extern enum eQCWWANError UpkQmiImsSLQSSetIMSSMSConfig (
    BYTE                                 *pMdmResp,
    struct QmiImsSLQSSetIMSSMSConfigResp *pSetIMSSMSConfigResp );

#endif /* __IMS_SLQS_SET_IMS_SMS_CONFIG_H__ */
