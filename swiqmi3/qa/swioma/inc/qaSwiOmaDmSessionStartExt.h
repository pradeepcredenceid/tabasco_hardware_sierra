/*
 * \ingroup swioma
 *
 * \file    qaSwiOmaDmSessionStartExt.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaSwiOmaDmSessionStart.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __SWIOMA_DM_SSSION_STARTEXT_H__
#define __SWIOMA_DM_SSSION_STARTEXT_H__

/* enum declarations */
/*
 * An enumeration of eQMI_SWIOMA_DM_SESSION_START_EXT request TLV IDs
 */
enum eSWIOMA_DM_SESSION_START_REQ_EXT
{
    eTLV_SESSION_TYPE_EXT = 0x10
};

/*
 * This structure contains the SLQSOMADMStartSessionExt request parameters.
 */
struct QmiSwiOmaDmSessionStartReqExt
{
    ULONG sessionType;
};

/*
 * This structure contains the SLQSOMADMStartSessionExt response parameters.
 */
struct QmiSwiOmaDmSessionStartRespExt
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiSwiOmaDmSessionStartExt(
    WORD  *pMlength,
    BYTE  *pBuffer,
    ULONG sessionType );

extern enum eQCWWANError UpkQmiSwiOmaDmSessionStartExt(
    BYTE                                  *pMdmResp,
    struct QmiSwiOmaDmSessionStartRespExt *pApiResp );

#endif /* __SWIOMA_DM_SSSION_START_H__ */
