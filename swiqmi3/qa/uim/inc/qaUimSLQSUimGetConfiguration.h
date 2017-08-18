/*
 * \ingroup uim
 *
 * \file    qaUimSLQSUimGetConfiguration.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaUimSLQSUimGetConfiguration.c
 *
 * Copyright: © 2016 Sierra Wireless Inc., all rights reserved
 *
 */

#ifndef __UIM_SLQS_UIM_GET_CONFIGURATION_H__
#define __UIM_SLQS_UIM_GET_CONFIGURATION_H__

#include "qaGobiApiUim.h"

/* enum declarations */
/*
 * An enumeration of eUIM_GET_CONFIGURATION_REQ request TLV IDs
 */
enum eUIM_GET_CONFIGURATION_REQ
{
    eTLV_CONFIG_MASK_REQ      = 0x10
};

/*
 * An enumeration of eUIM_GET_CONFIGURATION_RESP request TLV IDs
 */
enum eUIM_GET_CONFIGURATION_RESP
{
    eTLV_GET_AUTO_SELECTION    = 0x10,
    eTLV_GET_PERS_STATUS       = 0x11,
    eTLV_GET_HALT_SUBSCRIPTION = 0x12,
};

/*
 * This structure contains the SLQSUIMGetConfiguration response parameters.
 */
struct QmiUimSLQSGetConfigurationResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    UIMGetConfigurationResp *pUIMGetConfigurationResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiUimSlqsGetConfiguration(
    WORD                    *pMlength,
    BYTE                    *pBuffer,
    UIMGetConfigurationReq  *pUIMGetConfigurationReq);

extern enum eQCWWANError UpkQmiUimSlqsGetConfiguration(
    BYTE                                   *pMdmResp,
    struct QmiUimSLQSGetConfigurationResp  *pApiResp );

#endif /* __UIM_SLQS_UIM_GET_CONFIGURATION_H__ */
