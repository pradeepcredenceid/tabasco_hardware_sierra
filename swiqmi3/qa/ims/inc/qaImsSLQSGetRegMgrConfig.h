/*
 * \ingroup ims
 *
 * \file qaImsSLQSGetRegMgrConfig.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaImsSLQSGetRegMgrConfig.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_SLQS_GET_REG_MGR_CONFIG_H__
#define __IMS_SLQS_GET_REG_MGR_CONFIG_H__

/* enum declarations */

/*
 * An enumeration of eQMI_IMS_SLQS_GET_REG_MGR_CONFIG response TLV IDs
 */
enum eQMI_IMS_SLQS_GET_REG_MGR_CONFIG_RESP
{
    eTLV_SETTING_RESP  = 0x10,
    eTLV_PCSCF_PORT    = 0x11,
    eTLV_PRI_CSCF_PORT = 0x12,
    eTLV_IMS_TEST_MODE = 0x13,
};

/*
 * This structure contains the SLQSGetRegMgrConfig response parameters.
 *
 */
struct QmiImsSLQSGetRegMgrConfigResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSGetRegMgrConfig response parameters */
    GetRegMgrConfigParams *pGetRegMgrConfigParams;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiImsSLQSGetRegMgrConfig (
    WORD *pMlength,
    BYTE *pParamField );

extern enum eQCWWANError UpkQmiImsSLQSGetRegMgrConfig (
    BYTE                                 *pMdmResp,
    struct QmiImsSLQSGetRegMgrConfigResp *pGetRegMgrConfigResp );

#endif /* __IMS_SLQS_GET_REG_MGR_CONFIG_H__ */
