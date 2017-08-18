/*
 * \ingroup swioma
 *
 * \file    qaSwiOmaDmSessionGetInfo.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaSwiOmaDmSessionGetInfo.c
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __SWIOMA_DM_SESSION_GET_INFO_H__
#define __SWIOMA_DM_SESSION_GET_INFO_H__
#include "qaGobiApiSwiOmadms.h"

/* enum declarations */

/*
 * An enumeration of eQMI_SWIOMA_DM_SESSION_GET_INFO request TLV IDs
 */
enum eQMI_SWIOMA_DM_SESSION_GET_INFO_REQ
{
    eTLV_GET_SESSION_INFO_TYPE = 0x10
};

/*
 * An enumeration of eQMI_SWIOMA_DM_SESSION_GET_INFO response TLV IDs
 */
enum eQMI_SWIOMA_DM_SESSION_GET_INFO_RESP
{
    eTLV_GET_OMA_DM_SESSION_INFO = 0x10
};

/*
 * This structure contains the SLQSOMADMGetSessionInfo request parameters
 */
struct QmiSwiOmaDmSessionGetInfoReq
{
    /* SLQSOMADMGetSessionInfo request parameters */
    BYTE *pSessionType;
};

/*
 * This structure contains the SLQSOMADMGetSessionInfo response parameters
 */
struct QmiSwiOmaDmSessionGetInfoResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSOMADMGetSessionInfo response parameters */
    SLQSOMADMSessionInfo *pResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiSwiOmaDmSessionGetInfo(
    WORD   *pMlength,
    BYTE   *pBuffer,
    ULONG  *pSessionType);

extern enum eQCWWANError UpkQmiSwiOmaDmSessionGetInfo(
    BYTE                                 *pMdmResp,
    struct QmiSwiOmaDmSessionGetInfoResp *pApiResp );

#endif /* __SWIOMA_DM_SESSION_GET_INFO_H__ */
