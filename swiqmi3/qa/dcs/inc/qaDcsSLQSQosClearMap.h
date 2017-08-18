/**
 * \ingroup dcs
 *
 * \file    qaDcsSLQSQosClearMap.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaDcsSLQSQosClearMap.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __DCS_SLQS_QOS_CLEAR_MAP_H__
#define __DCS_SLQS_QOS_CLEAR_MAP_H__

/* enum declarations */

/* Prototypes */
extern enum eQCWWANError PkQmiDcsSLQSQosClearMap(
    WORD *pMlength,
    BYTE *pParamField);

extern enum eQCWWANError UpkQmiDcsSLQSQosClearMap(
    BYTE   *pMdmResp,
    struct qm_qmi_response_tlvs_values *pApiResp);

#endif /* __DCS_SLQS_QOS_CLEAR_MAP_H__ */


