/*
 * \ingroup ims
 *
 * \file qaImsSLQSImsConfigIndRegister.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaImsSLQSImsConfigIndRegister.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_SLQS_IMS_CONFIG_IND_REGISTER_H__
#define __IMS_SLQS_IMS_CONFIG_IND_REGISTER_H__

/* enum declarations */

/*
 * An enumeration of eQMI_IMS_SLQS_CONFIG_IND_REGISTER resquest TLV IDs
 */
enum eQMI_IMS_SLQS_CONFIG_IND_REGISTER_REQ
{
    eTLV_IMS_SIP_CONFIG_EVENTS     = 0x10,
    eTLV_IMS_REG_MGR_CONFIG_EVENTS = 0x11,
    eTLV_IMS_SMS_CONFIG_EVENTS     = 0x12,
    eTLV_IMS_USER_CONFIG_EVENTS    = 0x13,
    eTLV_IMS_VOIP_CONFIG_EVENTS    = 0x14,
};

/*
 * This structure contains the SLQSImsConfigIndicationRegister response
 * parameters.
 *
 */
struct QmiImsSLQSImsCfgIndRegisterResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiImsSLQSImsCfgIndRegister (
    WORD                  *pMlength,
    BYTE                  *pParamField,
    imsCfgIndRegisterInfo *pImsCfgIndRegisterInfo );

extern enum eQCWWANError UpkQmiImsSLQSImsCfgIndRegister (
    BYTE                                   *pMdmResp,
    struct QmiImsSLQSImsCfgIndRegisterResp *pApiResp );

#endif /* __IMS_SLQS_IMS_CONFIG_IND_REGISTER_H__ */
