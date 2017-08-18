/*
 * \ingroup ims
 *
 * \file qaImsSLQSGetSIPConfig.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaImsSLQSGetSIPConfig.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_SLQS_GET_SIP_CONFIG_H__
#define __IMS_SLQS_GET_SIP_CONFIG_H__

/* enum declarations */

/*
 * An enumeration of eQMI_IMS_SLQS_GET_SIP_CONFIG response TLV IDs
 */
enum eQMI_IMS_SLQS_GET_SIP_CONFIG_RESP
{
    eTLV_SETT_RESP       = 0x10,
    eTLV_SIP_PORT_NUMBER = 0x11,
    eTLV_TMR_REG_SIP     = 0x12,
    eTLV_SUBSCRIBE_TMR   = 0x13,
    eTLV_TMR_T1          = 0x14,
    eTLV_TMR_T2          = 0x15,
    eTLV_TMR_TF          = 0x16,
    eTLV_SIGCOMP_STAT    = 0x17,
};

/*
 * This structure contains the SLQSGetSIPConfig response parameters.
 *
 */
struct QmiImsSLQSGetSIPConfigResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSGetSIPConfig response parameters */
    GetSIPConfigResp *pGetSIPConfigResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiImsSLQSGetSIPConfig (
    WORD            *pMlength,
    BYTE            *pParamField );

extern enum eQCWWANError UpkQmiImsSLQSGetSIPConfig (
    BYTE                              *pMdmResp,
    struct QmiImsSLQSGetSIPConfigResp *pGetSIPConfigResp );

#endif /* __IMS_SLQS_GET_SIP_CONFIG_H__ */
