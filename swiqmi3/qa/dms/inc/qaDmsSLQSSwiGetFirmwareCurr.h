/*
 * \ingroup dms
 *
 * \file    qaDmsSLQSGetFirmwareCurr.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaDmsSLQSGetFirmwareCurr.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __DMS_GET_FIRMWARE_CURR_H__
#define __DMS_GET_FIRMWARE_CURR_H__

#include "qaGobiApiDms.h"

/* enum declarations */

/*
 * An enumeration of eQMI_DMS_SWI_GET_FW_CURR response TLV IDs
 */
enum eQMI_DMS_SWI_GET_FW_CURR_RESP
{
    eTLV_CURR_IMG_LIST = 0x01,
};

/*
 * This structure contains the SLQSSwiGetFirmwareCurr response parameters.
 */
struct QmiDmsGetFirmwareCurrResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    CurrentImgList *pCurrentImgList;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiDmsSLQSSwiGetCurrFw(
    WORD *pMlength,
    BYTE *pBuffer );

extern enum eQCWWANError UpkQmiDmsSLQSSwiGetCurrFw(
    BYTE                             *pMdmResp,
    struct QmiDmsGetFirmwareCurrResp *pApiResp );

#endif /* __DMS_GET_FIRMWARE_CURR_H__ */
