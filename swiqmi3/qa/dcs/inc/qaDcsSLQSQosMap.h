/**
 * \ingroup dcs
 *
 * \file    qaDcsSLQSQosMap.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaDcsSLQSQosMap.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __DCS_SLQS_QOS_MAP_H__
#define __DCS_SLQS_QOS_MAP_H__

/* enum declarations */

/* Prototypes */
extern enum eQCWWANError PkQmiDcsSLQSQosMap(
    WORD *pMlength,
    BYTE *pParamField,
    BYTE dscp,
    ULONG qos_id );

extern enum eQCWWANError UpkQmiDcsSLQSQosMap(
    BYTE   *pMdmResp,
    struct qm_qmi_response_tlvs_values *pApiResp);

#endif /* __DCS_SLQS_QOS_MAP_H__ */

