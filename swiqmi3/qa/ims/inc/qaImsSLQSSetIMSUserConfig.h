/*
 * \ingroup ims
 *
 * \file qaImsSLQSSetImsUserConfig.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaImsSLQSSetImsUserConfig.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_SLQS_SET_IMS_USER_CONFIG_H__
#define __IMS_SLQS_SET_IMS_USER_CONFIG_H__

/* enum declarations */

/*
 * An enumeration of eQMI_IMS_SLQS_SET_IMS_USER_CONFIG request TLV IDs
 */
enum eQMI_IMS_SLQS_SET_IMS_USER_CONFIG_REQ
{
    eTLV_IMS_DOMAIN_NAME = 0x10,
};

/*
 * An enumeration of eQMI_IMS_SLQS_SET_IMS_USER_CONFIG response TLV IDs.
 * The settings resp TLV is common to eQMI_IMS_SLQS_SET_SIP_CONFIG.
 */

/*
 * This structure contains the SLQSSetIMSUserConfig request parameters.
 *
 * \sa qaGobiApiIms.h for parameter descriptions
 *
 */
struct QmiImsSLQSSetIMSUserConfigReq
{
    SetIMSUserConfigReq *pSetIMSUserConfigReq;
};

/*
 * This structure contains the SLQSSetIMSUserConfig response parameters.
 *
 */
struct QmiImsSLQSSetIMSUserConfigResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSSetIMSUserConfig response parameters */
    SetIMSUserConfigResp *pSetIMSUserConfigResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiImsSLQSSetIMSUserConfig (
    WORD                *pMlength,
    BYTE                *pParamField,
    SetIMSUserConfigReq *pSetIMSUserConfigReq );

extern enum eQCWWANError UpkQmiImsSLQSSetIMSUserConfig (
    BYTE                                  *pMdmResp,
    struct QmiImsSLQSSetIMSUserConfigResp *pSetIMSUserConfigResp );

#endif /* __IMS_SLQS_SET_IMS_USER_CONFIG_H__ */
