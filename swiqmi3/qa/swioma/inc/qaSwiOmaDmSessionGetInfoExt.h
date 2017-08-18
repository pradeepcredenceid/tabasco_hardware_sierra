/*
 * \ingroup swioma
 *
 * \file    qaSwiOmaDmSessionGetInfoExt.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaSwiOmaDmSessionGetInfoExt.c
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __SWIOMA_DM_SESSION_GET_INFO_EXT_H__
#define __SWIOMA_DM_SESSION_GET_INFO_EXT_H__
#include "qaGobiApiSwiOmadmsExt.h"

/* enum declarations */

/*
 * An enumeration of eQMI_SWIOMA_DM_SESSION_GET_INFO_EXT response TLV IDs
 */
enum eQMI_SWIOMA_DM_SESSION_GET_INFO_EXT_RESP
{
    eTLV_GET_OMA_DM_SESSION_INFO_EXT = 0x10
};

/*
 * This structure contains the SLQSOMADMGetSessionInfo response parameters
 */
struct QmiSwiOmaDmSessionGetInfoExtResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* SLQSOMADMGetSessionInfo response parameters */
    SLQSOMADMSessionInfoExt *pResp;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiSwiOmaDmSessionGetInfoExt(
    WORD   *pMlength,
    BYTE   *pBuffer);

extern enum eQCWWANError UpkQmiSwiOmaDmSessionGetInfoExt(
    BYTE                                    *pMdmResp,
    struct QmiSwiOmaDmSessionGetInfoExtResp *pApiResp );

#endif /* __SWIOMA_DM_SESSION_GET_INFO_EXT_H__ */
