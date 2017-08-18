/*
 * \ingroup cbk
 *
 * \file    qaCbkImsSLQSUserCfgInd.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaCbkImsSLQSUserCfgInd.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMS_USER_CONFIG_IND_H_
#define __IMS_USER_CONFIG_IND_H_

#include "qaGobiApiIms.h"
#include "qaGobiApiCbk.h"

/* Constant Defs */
#define MAX_IMS_DOMAIN_NAME_LEN 255

/*
 * An enumeration of eQMI_IMS_USER_CONFIG_IND response TLV IDs
 */
enum eQMI_IMS_USER_CONFIG_IND
{
    eTLV_IMS_DMIN = 0x10,
};

/*
 * eQMI_IMS_USER_CONFIG_IND TLVs defined below
 */

/*
 * Name:    IMSDomainTlv
 *
 * Purpose: Structure used to store IMS Domain Name TLV Value.
 *
 * Members: TlvPresent    - Boolean indicating the presence of the TLV
 *                          in the QMI response
 *
 *          imsDomainName - IMS Domain Name
 */
struct IMSDomainTlv
{
    BYTE TlvPresent;
    BYTE imsDomainName[MAX_IMS_DOMAIN_NAME_LEN];
};

/*
 * Name:    QmiCbkImsUserConfigInd
 *
 * Purpose: Structure used to store User Config Indication Parameters.
 *
 * Members: IDTlv - IMS Domain Tlv
 *
 * Note:    None
 */
struct QmiCbkImsUserConfigInd{
    struct IMSDomainTlv IDTlv;
};

/*
 * Prototypes
 */
enum eQCWWANError UpkQmiCbkImsUserCfgInd(
    BYTE                          *pMdmResp,
    struct QmiCbkImsUserConfigInd *pApiResp );

#endif /* __IMS_USER_CONFIG_IND_H_ */
