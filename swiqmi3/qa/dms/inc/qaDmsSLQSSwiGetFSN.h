/*
 * \ingroup dms
 *
 * \file  qaDmsSLQSSwiGetFSN.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaDmsSLQSSwiGetFSN.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __DMS_SLQS_SWI_GET_FSN_H__
#define __DMS_SLQS_SWI_GET_FSN_H__

/* enum declarations */
/*
 *
 * An enumeration of eQMI_DMS_SWI_GET_FSN response TLV IDs
 *
 */
enum eDMS_SLQS_SWI_GET_FSN_RESP
{
    eTLV_DMS_DEV_FSN = 0x01
};

/*
 * This structure contains the SLQSSwiGetFSN response parameters.
 *
 * \sa qaGobiApiDms.h for parameter descriptions
 *
 */
struct QmiDmsSLQSSwiGetFSNResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult    results;

    FactorySequenceNumber *pFSNumber;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiDmsSLQSSwiGetFSN (
    WORD *pMlength,
    BYTE *pParamField );

extern enum eQCWWANError UpkQmiDmsSLQSSwiGetFSN (
    BYTE                           *pMdmResp,
    struct QmiDmsSLQSSwiGetFSNResp *pApiResp );

#endif /* __DMS_SLQS_SWI_GET_FSN_H__ */
