/**
 * \ingroup dcs
 *
 * \file    qaDcsSLQSQosUnmap.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaDcsSLQSQosUnmap.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __DCS_SLQS_QOS_UNMAP_H__
#define __DCS_SLQS_QOS_UNMAP_H__

/* enum declarations */

/* Prototypes */
extern enum eQCWWANError PkQmiDcsSLQSQosUnmap(
    WORD *pMlength,
    BYTE *pParamField,
    BYTE dscp);

extern enum eQCWWANError UpkQmiDcsSLQSQosUnmap(
    BYTE   *pMdmResp,
    struct qm_qmi_response_tlvs_values *pApiResp);

#endif /* __DCS_SLQS_QOS_UNMAP_H__ */


