/*
 * \ingroup swioma
 *
 * \file    qaSwiOmaDmSetSettingsExt.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaSwiOmaDmSetSettings.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __SWIOMA_DM_SET_SETTINGSEXT_H__
#define __SWIOMA_DM_SET_SETTINGSEXT_H__

#include "qaGobiApiSwiOmadmsExt.h"

/* enum declarations */
/*
 * An enumeration of eQMI_SWIOMA_DM_SET_SETTINGS request TLV IDs
 */
enum eSWIOMA_DM_SET_SETTINGS_REQ_EXT
{
    eTLV_SET_FOTA_AUTOMATIC_DOWNLOAD_EXT = 0x11,
    eTLV_SET_FOTA_AUTOMATIC_UPDATE_EXT   = 0x12,
    eTLV_SET_OMADM_ENABLE_EXT  = 0x10,
    eTLV_SET_OMADM_LOG_ENABLE_EXT = 0x14,
    eTLV_SET_OMADM_FUMO_ENABLE_EXT = 0x15,
    eTLV_SET_OMADM_PRL_ENABLE_EXT = 0x16
};

/*
 * This structure contains the SLQSOMADMSetSettings response parameters.
 */
struct QmiSwiOmaDmSetSettingsRespExt
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiSwiOmaDmSetSettingsExt(
    WORD                          *pMlength,
    BYTE                          *pBuffer,
    SLQSOMADMSettingsReqParamsExt *pSLQSOMADMSettings );

extern enum eQCWWANError UpkQmiSwiOmaDmSetSettingsExt(
    BYTE                                 *pMdmResp,
    struct QmiSwiOmaDmSetSettingsRespExt *pApiResp );

#endif /* __SWIOMA_DM_SET_SETTINGS_H__ */
