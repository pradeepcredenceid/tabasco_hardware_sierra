/**
 *    \if CvsId 
 *    $Id: SwiApiCdmaRuim.h,v 1.5 2010/06/10 00:11:16 epasheva Exp $
 *    \endif
 *    \ingroup cdmaruim
 *    \file SwiApiCdmaRuim.h
 * 
 *    \brief SWI CDMA specific Ruim related APIs.
 *
 *    Copyright (c) 2008 Sierra Wireless, Inc.  All rights reserved
 *
 */
#ifndef __SWIAPICDMARUIM_H__
#define __SWIAPICDMARUIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"

/**
 *
 * An enumeration of RUIM status.
 *
 */
typedef enum SWI_TYPE_RuimStatus
{
    SWI_RUIM_STATUS_OK = 0x0000,    /*!< RUIM OK */
    SWI_RUIM_STATUS_INIT_ERR,       /*!< RUIM initialization error (fatal) */
    SWI_RUIM_STATUS_GENERAL_ERR,    /*!< RUIM general failure (fatal) */
    SWI_RUIM_STATUS_LOCKED,         /*!< RUIM locked */
    SWI_RUIM_STATUS_CHV1_BLOCKED,   /*!< CHV1 blocked */
    SWI_RUIM_STATUS_CHV2_BLOCKED,   /*!< CHV2 blocked*/
    SWI_RUIM_STATUS_CHV1_REJECTED,  /*!< CHV1 rejected(no recoverable error) */
    SWI_RUIM_STATUS_CHV2_REJECTED,  /*!< CHV2 rejected */
    SWI_RUIM_STATUS_POLL_ERR,       /*!< Polling error (fatal) */
    
    /*! Wrong RUIM inserted after PC card powered up */
    SWI_RUIM_STATUS_NO_SRV_ERR,

    SWI_RUIM_STATUS_ICCID_MISMATCH_ERR  /*!< RUIM OK */
}SWI_TYPE_RuimStatus;

/**
 *
 * An enumeration of RUIM user required operation.
 *
 */
typedef enum SWI_TYPE_RuimOpReq
{
    SWI_RUIM_OPREQ_NONE = 0x0000,   /*!< None */
    SWI_RUIM_OPREQ_CHV1,            /*!< Reenter CHV1 */
    SWI_RUIM_OPREQ_CHV2,            /*!< Reenter CHV2 */
    SWI_RUIM_OPREQ_CHV_UNBLK1,      /*!< Reenter unblocking CHV1 code */
    SWI_RUIM_OPREQ_CHV_UNBLK2       /*!< Reenter unblocking CHV2 code */
}SWI_TYPE_RuimOpReq;

/**
 *
 * An enumeration of RUIM previous user operation reporting on.
 *
 */
typedef enum SWI_TYPE_RuimPrevOp
{
    SWI_RUIM_PREV_OP_NONE = 0x0000, /*!< None */
    SWI_RUIM_PREV_OP_CHV1CHG,       /*!< Change CHV1 */
    SWI_RUIM_PREV_OP_CHV2CHG,       /*!< Change CHV2 */
    SWI_RUIM_PREV_OP_CHV1VER,       /*!< Verify CHV1 */
    SWI_RUIM_PREV_OP_CHV2VER,       /*!< Verify CHV2 */
    SWI_RUIM_PREV_OP_UNBLK1,        /*!< Unblock CHV1 */
    SWI_RUIM_PREV_OP_UNBLK2,        /*!< Unblock CHV2 */
    SWI_RUIM_PREV_OP_CHV1_EN,       /*!< CHV1 enable */
    SWI_RUIM_PREV_OP_CHV1_DIS       /*!< CHV2 enable */
}SWI_TYPE_RuimPrevOp;

/**
 *
 * An enumeration of RUIM revious user operation result.
 *
 */
typedef enum SWI_TYPE_RuimPrevOpRes
{
    SWI_RUIM_PREV_OP_FAIL = 0x0000, /*!< Failed */
    SWI_RUIM_PREV_OP_SUCCESS,       /*!< Succeeded */
    SWI_RUIM_PREV_OP_FAIL_VER,      /*!< Failed because CHV not enabled */

    /*! Failed because CHV length out of range (4-8 digits valid) */
    SWI_RUIM_PREV_OP_FAIL_CHV_LEN,
    
    /*! Failed because unblocking CHV length out of range(must be 8 digits) */
    SWI_RUIM_PREV_OP_FAIL_UNBLK_LEN,
    
    /*! Failed because disable CHV1 service is not allocated and activated */
    SWI_RUIM_PREV_OP_FAIL_NO_SRV,
    
    /*! Failed because CHV starts with an emergency number */
    SWI_RUIM_PREV_OP_FAIL_EMERG
}SWI_TYPE_RuimPrevOpRes;

/**
 *
 * An enumeration of RUIM retry information present.
 *
 */
typedef enum SWI_TYPE_RuimRetryPres
{
    SWI_RUIM_NO_RETRY_INFO = 0x0000,    /*!< False */
    SWI_RUIM_RETRY_INFO                 /*!< True */
}SWI_TYPE_RuimRetryPres;

/**
 *
 * An enumeration of RUIM retry information type.
 *
 */
typedef enum SWI_TYPE_RuimRetry
{
    SWI_RUIM_RETRY_CHV1 = 0x0000,   /*!< CHV1 */
    SWI_RUIM_RETRY_CHV2,            /*!< CHV2 */
    SWI_RUIM_RETRY_UNBLOCK_CHV1,    /*!< unblocking CHV1 */
    SWI_RUIM_RETRY_UNBLOCK_CHV2     /*!< unblocking CHV2 */
}SWI_TYPE_RuimRetry;

/**
 *
 * An enumeration of RUIM chv.
 *
 */
typedef enum SWI_RUIM_CHV
{
    SWI_RUIM_CHV1 = 0x0001, /*!< CHV1 */
    SWI_RUIM_CHV2,          /*!< CHV1 */
    SWI_RUIM_UNBLOCK_CHV1,  /*!< CHV1 */
    SWI_RUIM_UNBLOCK_CHV2   /*!< CHV1 */
}SWI_RUIM_CHV;

/**
 *
 * Contains RUIM statue data.
 *
 */
typedef struct SWI_STRUCT_RuimStatus
{
    swi_uint32 sizeStruct;      /*!< size of this structure */
    swi_uint16 nStatus;         /*!< RUIM status. See SWI_TYPE_RuimStatus */

    /*! RUIM user required operation. See SWI_TYPE_RuimRetryPres */
    swi_uint16 byActionReq;

    /*! RUIM previous user operation. See SWI_TYPE_RuimRetryPres */
    swi_uint16 byPrevAction;

    /*! RUIM revious user operation result. See SWI_TYPE_RuimRetryPres */
    swi_uint16 byPrevActRes;
    
    /*! RUIM retry information present. See SWI_TYPE_RuimRetryPres */
    swi_uint16 byRetryPres;
    
    /*! RUIM retry information type. See SWI_TYPE_RuimRetry */
    swi_uint16 byRetry;

    swi_uint16 byRetryNum;      /*!< Retry count */
}SWI_STRUCT_RuimStatus;

/* Function prototypes */

SWI_API SWI_RCODE SwiGetRUIMStatus(SWI_STRUCT_RuimStatus* sRuimStatus,
                                   swi_uint32 timeout);

SWI_API SWI_RCODE SwiVerifyCHV(swi_uint16 Chv, const swi_charp szCHV,
                               swi_uint32 timeout);

SWI_API SWI_RCODE SwiEnableCHV(swi_bool bEnable, const swi_charp szCHV,
                               swi_uint32 timeout);

SWI_API SWI_RCODE SwiChangeCHV(swi_uint16 Chv, const swi_charp szOldCHV,
                               const swi_charp szNewCHV, swi_uint32 timeout);

SWI_API SWI_RCODE SwiUnblockCHV(swi_uint16 Chv, const swi_charp szUnblockCHV,
                                const swi_charp szCHV, swi_uint32 timeout);

SWI_API SWI_RCODE SwiGetCHVEnableStatus(swi_bool* bEnable, swi_uint32 timeout);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __SWIAPICDMARUIM_H__ */

/* 
 * $Log: SwiApiCdmaRuim.h,v $
 */
