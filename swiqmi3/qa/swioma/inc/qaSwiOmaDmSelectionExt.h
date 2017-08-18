/*
 * \ingroup swioma
 *
 * \file    qaSwiOmaDmSelectionExt.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaSwiOmaDmSelection.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __SWIOMA_DM_SELECTIONEXT_H__
#define __SWIOMA_DM_SELECTIONEXT_H__

/* enum declarations */
/*
 * An enumeration of eQMI_SWIOMA_DM_SELECTION request TLV IDs
 */
enum eSWIOMA_DM_SELECTION_REQ_EXT
{
    eTLV_USER_SELECTION_EXT = 0x10
};

/*
 * This structure contains the SLQSOMADMSendSelection request parameters.
 */
struct QmiSwiOmaDmSelectionReqExt
{
    ULONG selection;
};

/*
 * This structure contains the SLQSOMADMSendSelection response parameters.
 */
struct QmiSwiOmaDmSelectionRespExt
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiSwiOmaDmSelectionExt(
    WORD  *pMlength,
    BYTE  *pBuffer,
    ULONG selection );

extern enum eQCWWANError UpkQmiSwiOmaDmSelectionExt(
    BYTE                               *pMdmResp,
    struct QmiSwiOmaDmSelectionRespExt *pApiResp );

#endif /* __SWIOMA_DM_SELECTION_H__ */
