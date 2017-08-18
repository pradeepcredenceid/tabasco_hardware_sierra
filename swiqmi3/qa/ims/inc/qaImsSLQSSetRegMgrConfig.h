/*
 * \ingroup ims
 *
 * \file qaImsSLQSSetRegMgrConfig.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaImsSLQSSetRegMgrConfig.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_SLQS_SET_REG_MGR_CONFIG_H__
#define __IMS_SLQS_SET_REG_MGR_CONFIG_H__

/* enum declarations */

/*
 * An enumeration of eQMI_IMS_SLQS_SET_REG_MGR_CONFIG request TLV IDs
 */
enum eQMI_IMS_SLQS_SET_REG_MGR_CONFIG_REQ
{
    eTLV_REGMGR_CFG_PCSCF_PORT = 0x10,
    eTLV_REGMGR_PRI_CSCF       = 0x11,
    eTLV_IMS_TEST_MODE_ENABLED = 0x12,
};

/*
 * An enumeration of eQMI_IMS_SLQS_SET_REG_MGR_CONFIG response TLV IDs.
 * The settings resp TLV is common to eQMI_IMS_SLQS_SET_SIP_CONFIG.
 */

/*
 * This structure contains the SLQSSetRegMgrConfig request parameters.
 *
 * \sa qaGobiApiIms.h for parameter descriptions
 *
 */
struct QmiImsSLQSSetRegMgrConfigReq
{
    SetRegMgrConfigReq *pSetRegMgrConfigReq;
};

/*
 * This structure contains the SLQSSetRegMgrConfig response parameters.
 *
 */
struct QmiImsSLQSSetRegMgrConfigResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSSetSIPConfig response parameters */
    SetRegMgrConfigResp *pSetRegMgrConfig;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiImsSLQSSetRegMgrConfig (
    WORD               *pMlength,
    BYTE               *pParamField,
    SetRegMgrConfigReq *pSetRegMgrConfigReq );

extern enum eQCWWANError UpkQmiImsSLQSSetRegMgrConfig (
    BYTE                                 *pMdmResp,
    struct QmiImsSLQSSetRegMgrConfigResp *pSetRegMgrConfigResp );

#endif /* __IMS_SLQS_SET_REG_MGR_CONFIG_H__ */
