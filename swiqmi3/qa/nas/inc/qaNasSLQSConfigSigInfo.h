/*
 * \ingroup nas
 *
 * \file  qaNasSLQSConfigSigInfo.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaNasSLQSConfigSigInfo.c
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __NAS_SLQS_CONFIG_SIG_INFO_H__
#define __NAS_SLQS_CONFIG_SIG_INFO_H__

#include "qaGobiApiNas.h"

/* enum declarations */
/*
 *
 * An enumeration of eQMI_NAS_SLQS_CONFIG_SIG_INFO request TLV IDs
 *
 */
enum eNAS_SLQS_CONFIG_SIG_INFO_REQ
{
    eTLV_RSSI_THRESHOLD        = 0x10,
    eTLV_ECIO_THRESHOLD        = 0x11,
    eTLV_HDR_SINR_THRESHOLD    = 0x12,
    eTLV_LTE_SNR_THRESHOLD     = 0x13,
    eTLV_IO_THRESHOLD          = 0x14,
    eTLV_RSRQ_THRESHOLD        = 0x15,
    eTLV_RSRP_THRESHOLD        = 0x16,
    eTLV_LTE_SIGNAL_REPORT_CFG = 0x17,
};

/*
 * This structure contains the SLQSConfigSigInfo response parameters
 *
 */
struct QmiNasSLQSConfigSigInfoResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 *
 */
enum eQCWWANError PkQmiNasSLQSConfigSigInfo(
    WORD    *pMlength,
    BYTE    *pBuffer,
    sigInfo *pSigInfo );

enum eQCWWANError UpkQmiNasSLQSConfigSigInfo(
    BYTE                               *pMdmResp,
    struct QmiNasSLQSConfigSigInfoResp *pApiResp );

#endif /* __NAS_SLQS_CONFIG_SIG_INFO_H__ */
