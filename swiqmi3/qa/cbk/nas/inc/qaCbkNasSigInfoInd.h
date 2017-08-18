/*
 * \ingroup cbk
 *
 * \file qaCbkNasSigInfoInd.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaCbkNasSigInfoInd.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __NAS_SIG_INFO_IND_H__
#define __NAS_SIG_INFO_IND_H__

#include "qaGobiApiCbk.h"

/*
 * An enumeration of eQMI_NAS_SIG_INFO_IND response TLV IDs
 *
 */
enum eQMI_NAS_SIG_INFO_IND
{
    eTLV_IND_CDMA_SIG_S_INFO      = 0x10,
    eTLV_IND_HDR_SIG_S_INFO       = 0x11,
    eTLV_IND_GSM_SIG_S_INFO       = 0x12,
    eTLV_IND_WCDMA_SIG_S_INFO     = 0x13,
    eTLV_IND_LTE_SIG_S_INFO       = 0x14,
    eTLV_IND_TDSCDMA_SIG_INFO     = 0x15,
    eTLV_IND_TDSCDMA_EXT_SIG_INFO = 0x16
};

/*
 * Prototypes
 *
 */
enum eQCWWANError UpkQmiCbkNasSigInfoInd(
    BYTE       *pMdmResp,
    nasSigInfo *pApiResp );

#endif /* __NAS_SIG_INFO_IND_H__ */
