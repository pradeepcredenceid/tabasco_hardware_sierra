/*
 * \ingroup imsa
 *
 * \file qaImsaSLQSIndRegister.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaImsaSLQSIndRegister.c
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __IMSA_SLQS_IND_REGISTER_H__
#define __IMSA_SLQS_IND_REGISTER_H__

/* enum declarations */

/*
 * An enumeration of eQMI_IMSA_SLQS_IND_REGISTER request TLV IDs
 */
enum eQMI_IMSA_SLQS_IND_REGISTER_REQ
{
    eTLV_IMSA_REG_STATUS_CONFIG     = 0x10,
    eTLV_IMSA_SVC_STATUS_CONFIG     = 0x11,
    eTLV_IMSA_RAT_HANDOVER_STATUS_CONFIG     = 0x12
};

/*
 * This structure contains the SLQSRegisterIMSAIndication response
 * parameters.
 *
 */
struct QmiImsaSLQSIndRegisterResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiImsaSLQSIndRegister (
    WORD                  *pMlength,
    BYTE                  *pParamField,
    IMSAIndRegisterInfo   *pImsaIndRegisterInfo );

extern enum eQCWWANError UpkQmiImsaSLQSIndRegister (
    BYTE                                   *pMdmResp,
    struct QmiImsaSLQSIndRegisterResp      *pApiResp );

#endif /* __IMSA_SLQS_IND_REGISTER_H__ */
