/*
 * \ingroup ims
 *
 * \file qaImsSLQSGetIMSSMSConfig.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaImsSLQSGetIMSSMSConfig.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_SLQS_GET_IMS_SMS_CONFIG_H__
#define __IMS_SLQS_GET_IMS_SMS_CONFIG_H__

/* enum declarations */

/*
 * An enumeration of eQMI_IMS_SLQS_GET_IMS_SMS_CONFIG response TLV IDs
 */
enum eQMI_IMS_SLQS_GET_IMS_SMS_CONFIG_RESP
{
    eTLV_SETNG_RESP     = 0x10,
    eTLV_SMS_FMT        = 0x11,
    eTLV_SMS_OVER_IPNIF = 0x12,
    eTLV_PH_CTXT_URI    = 0x13,
};

/*
 * This structure contains the SLQSGetIMSSMSConfig response parameters.
 *
 */
struct QmiImsSLQSGetIMSSMSConfigResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSGetIMSSMSConfig response parameters */
    GetIMSSMSConfigParams *pGetIMSSMSConfigParams;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiImsSLQSGetIMSSMSConfig (
    WORD *pMlength,
    BYTE *pParamField );

extern enum eQCWWANError UpkQmiImsSLQSGetIMSSMSConfig (
    BYTE                                 *pMdmResp,
    struct QmiImsSLQSGetIMSSMSConfigResp *pGetIMSSMSConfigResp );

#endif /* __IMS_SLQS_GET_IMS_SMS_CONFIG_H__ */
