/*
 * \ingroup pds
 *
 * \file qaPdsDeterminePosition.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaPdsDeterminePosition.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __PDS_DETERMINE_POSITION_MSG_H__
#define __PDS_DETERMINE_POSITION_MSG_H__

/*
 * This structure contains the SLQSPDSDeterminePosition response parameters.
 *
 */
struct QmiPdsDeterminePositionResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 *
 */
extern enum eQCWWANError PkQmiPdsDeterminePosition(
    WORD  *pMlength,
    BYTE  *pBuffer );

extern enum eQCWWANError UpkQmiPdsDeterminePosition(
    BYTE                               *pMdmResp,
    struct QmiPdsDeterminePositionResp *pApiResp );

#endif /* __PDS_DETERMINE_POSITION_MSG_H__ */
