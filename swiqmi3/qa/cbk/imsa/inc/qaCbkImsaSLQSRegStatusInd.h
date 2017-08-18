/*
 * \ingroup cbk
 *
 * \file    qaCbkImsaSLQSRegStatusInd.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaCbkImsSLQSRegStatusInd.c
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMSA_REG_STATUS_IND_H_
#define __IMSA_REG_STATUS_IND_H

#include "qaGobiApiImsa.h"
#include "qaGobiApiCbk.h"

/*
 * An enumeration of eQMI_IMSA_REGISTRATION_STATUS_IND response TLV IDs
 */
enum eQMI_IMSA_REGISTRATION_STATUS_IND_TLV
{
    eTLV_IMS_REG_STATUS = 0x01,
    eTLV_IMS_REG_STATUS_ERROR_CODE = 0x10,
    eTLV_NEW_IMS_REG_STATUS = 0x11
};

/*
 * eQMI_IMSA_REGISTRATION_STATUS_IND TLVs defined below
 */

/*
 * Name:    IMSRegStatusTlv
 *
 * Purpose: Structure used to store IMS Registration Status TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          ImsRegistered - IMS registered -TRUE/FALSE
 */
struct IMSRegStatusTlv
{
    BYTE TlvPresent;
    BYTE ImsRegistered;
};

/*
 * Name:    IMSRegStatusErrorCodeTlv
 *
 * Purpose: Structure used to store IMS Registration Status Error code TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          ErrorCode - Error code value if registration is failure
 */
struct IMSRegStatusErrorCodeTlv
{
    BYTE TlvPresent;
    WORD ErrorCode;
};

/*
 * Name:    NewIMSRegStatusTlv
 *
 * Purpose: Structure used to store IMS Registration Status TLV Value.
 *
 * Members: TlvPresent  - Boolean indicating the presence of the TLV
 *                        in the QMI response
 *
 *          ImsRegStatus - IMS Registration Status
 */
struct NewIMSRegStatusTlv
{
    BYTE TlvPresent;
    ULONG ImsRegStatus;
};


/*
 * Name:    QmiCbkImsaRegStatusInd
 *
 * Purpose: Structure used to store IMS Registration Status Indication Parameters.
 *
 * Members: IMSRegistration
 *          IMSRegistrationError
 *          NewIMSRegistration
 *
 * Note:    None
 */
struct QmiCbkImsaRegStatusInd{
    struct IMSRegStatusTlv    IMSRegistration;
    struct IMSRegStatusErrorCodeTlv IMSRegistrationError;
    struct NewIMSRegStatusTlv  NewIMSRegistration;
};

/*
 * Prototypes
 */
enum eQCWWANError UpkQmiCbkImsaRegStatusInd(
    BYTE                            *pMdmResp,
    struct QmiCbkImsaRegStatusInd   *pApiResp );

#endif /* __IMSA_REG_STATUS_IND_H_ */
