/*
 * \ingroup loc
 *
 * \file    qaSlqsLocExtPowerState.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaSlqsLocExtPowerState.c
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __SLQS_LOC_EXT_POWER_STATE_H__
#define __SLQS_LOC_EXT_POWER_STATE_H__

#include "qaGobiApiLoc.h"

/* enum declarations */
/*
 * An enumeration of eQMI_LOC_SLQS_EVENT_REG_REQ request TLV IDs
 */
enum eQMI_LOC_SLQS_SET_EXT_POWER_STATE
{
    eTLV_LOC_EXT_POWER_STATE = 0x01
};
/*
 * This structure contains the LocSLQSExtPowerState response parameters.
 */
struct QmiLocSlqsExtPowerStateResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
    ULONG          extPowerState;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiLocSlqsExtPowerState(
    WORD                    *pMlength,
    BYTE                    *pParamField,
    LOCExtPowerStateReqResp *pLOCExtPowerStateReqResp );

extern enum eQCWWANError UpkQmiLocSlqsExtPowerState(
    BYTE                               *pMdmResp,
    struct QmiLocSlqsExtPowerStateResp *pApiResp );


#endif /* __SLQS_LOC_EXT_POWER_STATE_H__ */
