/**
 * \ingroup dcs
 *
 * \file    qaDcsSLQSStartSrv.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaDcsSLQSStartSrv.c
 *
 * Copyright: © 2011 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __DCS_SLQSSTARTSRV_H__
#define __DCS_SLQSSTARTSRV_H__

/* enum declarations */

/* Prototypes */
extern enum eQCWWANError PkQmiDcsSLQSStartSrv(
    WORD *pMlength,
    BYTE *pParamField,
    BYTE action,
    BYTE mask );

extern enum eQCWWANError UpkQmiDcsSLQSStartSrv(
    BYTE   *pMdmResp,
    struct qm_qmi_response_tlvs_values *pApiResp);

#endif /* __DCS_SLQSSTARTSRV_H__ */
