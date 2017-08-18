/*
 * \ingroup nas
 *
 * \file  qaSlqsLocStart.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaSlqsLocStart.c
 *
 * Copyright: © 2011-2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_START_H__
#define __LOC_START_H__

#include "qaGobiApiLoc.h"

/* enum declarations */
/*
 *
 * An enumeration of eQMI_LOC_START request TLV IDs
 *
 */
enum eLOC_START_REQ
{
    eTLV_START_SESSION_ID        = 0x01,
    eTLV_RECURRENCE_TYPE         = 0x10,
    eTLV_HORIZONTAL_ACCURACY     = 0x11,
    eTLV_INTERMEDIATE_RPT_STATE  = 0x12,
    eTLV_MINIMUM_INTERVAL        = 0x13,
    eTLV_APP_PARAMENTERS         = 0x14,
    eTLV_CONFIG_ALTITUDE_ASMD    = 0x15
};

/*
 * This structure contains the SLQSLOCStart response parameters
 *
 */
struct QmiLocStartResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiLocStartReq(
            WORD                *pMlength,
            BYTE                *pParamField,
            LOCStartReq         *pLOCStartReq );

extern enum eQCWWANError UpkQmiLocStartResp(
            BYTE                              *pMdmResp,
            struct QmiLocStartResp            *pApiResp);


#endif /* __LOC_START_H__ */
