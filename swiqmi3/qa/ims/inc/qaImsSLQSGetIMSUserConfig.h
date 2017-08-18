/*
 * \ingroup ims
 *
 * \file qaImsSLQSGetIMSUserConfig.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaImsSLQSGetIMSUserConfig.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_SLQS_GET_IMS_USER_CONFIG_H__
#define __IMS_SLQS_GET_IMS_USER_CONFIG_H__

/* enum declarations */

/*
 * An enumeration of eQMI_IMS_SLQS_GET_IMS_USER_CONFIG response TLV IDs
 */
enum eQMI_IMS_SLQS_GET_IMS_USER_CONFIG_RESP
{
    eTLV_SETING_RESP = 0x10,
    eTLV_IMS_DOMAIN  = 0x11,
};

/*
 * This structure contains the SLQSGetIMSUserConfig response parameters.
 *
 */
struct QmiImsSLQSGetIMSUserConfigResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSGetIMSUserConfig response parameters */
    GetIMSUserConfigParams *pGetIMSUserConfigParams;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiImsSLQSGetIMSUserConfig (
    WORD *pMlength,
    BYTE *pParamField );

extern enum eQCWWANError UpkQmiImsSLQSGetIMSUserConfig (
    BYTE                                  *pMdmResp,
    struct QmiImsSLQSGetIMSUserConfigResp *pGetIMSUserConfigResp );

#endif /* __IMS_SLQS_GET_IMS_USER_CONFIG_H__ */
