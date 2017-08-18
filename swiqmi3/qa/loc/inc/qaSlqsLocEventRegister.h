/*
 * \ingroup loc
 *
 * \file    qaSlqsLocEventRegister.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaSlqsLocEventRegister.c
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __SLQS_LOC_EVENT_REG_H__
#define __SLQS_LOC_EVENT_REG_H__

#include "qaGobiApiLoc.h"

/* enum declarations */
/*
 * An enumeration of eQMI_LOC_SLQS_EVENT_REG_REQ request TLV IDs
 */
enum eQMI_LOC_SLQS_EVENT_REG_REQ
{
    eTLV_LOC_EVENT_MASK = 0x01
};

/*
 * This structure contains the LocSLQSEventRegister response parameters.
 */
struct QmiLocSlqsEventRegisterResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
    ULONGLONG          eventRegister;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiLocSlqsEventRegister(
    WORD                    *pMlength,
    BYTE                    *pBuffer,
    LOCEventRegisterReqResp *pLOCEventRegisterReqResp );

extern enum eQCWWANError UpkQmiLocSlqsEventRegister(
    BYTE                               *pMdmResp,
    struct QmiLocSlqsEventRegisterResp *pApiResp );

#endif /* __SLQS_LOC_EVENT_REG_H__ */
