/*
 * \ingroup fms
 *
 * \file    qaFmsEnableForceDownload.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaFmsEnableForceDownload.c
 *
 * Copyright: © 2011 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __FMS_ENABLEFORCEDOWNLOAD_H__
#define __FMS_ENABLEFORCEDOWNLOAD_H__

/* Prototypes */
ULONG EnableForceDownload();

enum eQCWWANError PkQmiFmsEnableForceDownload(
    WORD    *pMlength,
    BYTE    *pParamField );

enum eQCWWANError UpkQmiFmsEnableForceDownload(
    BYTE   *pMdmResp,
    struct qm_qmi_response_tlvs_values *pApiResp );

#endif /* __FMS_ENABLEFORCEDOWNLOAD_H__ */
