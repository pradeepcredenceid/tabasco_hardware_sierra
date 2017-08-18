/*
 * \ingroup dms
 *
 * \file  qaDmsSLQSGetERIFile.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaDmsSLQSGetERIFile.c
 *
 * Copyright: © 2012 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __DMS_SLQS_GET_ERI_FILE_H__
#define __DMS_SLQS_GET_ERI_FILE_H__

/* enum declarations */
/*
 *
 * An enumeration of eQMI_DMS_SLQS_GET_ERI_FILE response TLV IDs
 *
 */
enum eDMS_SLQS_GET_ERI_FILE_RESP
{
    eTLV_DMS_ERI_FILE = 0x01
};

/*
 * This structure contains the SLQSGetERIFile response parameters.
 *
 * \sa qaGobiApiDms.h for parameter descriptions
 *
 */
struct QmiDmsSLQSGetERIFileResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    ERIFileparams      *pERIFileparams;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiDmsSLQSGetERIFile (
    WORD *pMlength,
    BYTE *pParamField );

extern enum eQCWWANError UpkQmiDmsSLQSGetERIFile (
    BYTE                            *pMdmResp,
    struct QmiDmsSLQSGetERIFileResp *pApiResp );

#endif /* __DMS_SLQS_GET_ERI_FILE_H__ */
