/*
 * \ingroup nas
 *
 * \file  qaSlqsLocStop.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaSlqsLocStop.c
 *
 * Copyright: © 2011-2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_STOP_H__
#define __LOC_STOP_H__

#include "qaGobiApiLoc.h"

/* enum declarations */
/*
 *
 * An enumeration of eQMI_LOC_STOP request TLV IDs
 *
 */
enum eLOC_STOP_REQ
{
    eTLV_STOP_SESSION_ID              = 0x01,
};

/*
 * This structure contains the SLQSLOCStop response parameters
 *
 */
struct QmiLocStopResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiLocStopReq(
            WORD                *pMlength,
            BYTE                *pParamField,
            LOCStopReq          *pLOCStopReq );

extern enum eQCWWANError UpkQmiLocStopResp(
            BYTE                              *pMdmResp,
            struct QmiLocStopResp             *pApiResp);

#endif /* __LOC_STOP_H__ */
