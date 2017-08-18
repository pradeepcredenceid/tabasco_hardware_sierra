/*
 * \ingroup cbk
 *
 * \file qaCbkNasDDTMSettings.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaCbkNasDDTMSettings.c
 *
 * Copyright: © 2010-2014 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __NAS_DDTM_SETTINGS_IND_H__
#define __NAS_DDTM_SETTINGS_IND_H__

#include "qaGobiApiCbk.h"
#include "qaGobiApiNas.h"
/*
 * An enumeration of eQMI_NAS_DDTM_IND response TLV IDs
 *
 */
enum eQMI_NAS_DDTM_SETTINGS_IND
{
    eTLV_IND_DDTM_SETTINGS = 0x01
};

/*
 * eQMI_NAS_DDTM_IND TLVs defined below
 */
/*
 * Name:    DDTMSettingsTlv
 *
 * Purpose: Structure used to store DDTM settings TLV Value
 *
 *          ddtm_settings      - DDTM settings
 *                               See \ref DDTMSettings for more information
 *
 * Note:    None
 */
struct NasDDTMSettingsTlv
{
    struct DDTMSettings  ddtm_settings;
};

/*
 * Name:    QmiCbkNasDDTMSettingsInd
 *
 * Purpose: Structure used to store all DDTM Settings indication Parameters.
 *
 * Members: DDTMSettingsTlv - DDTM Settings indication parameters
 *
 * Note:    None
 */
struct QmiCbkNasDDTMSettingsInd
{
    struct NasDDTMSettingsTlv DDTMSettingsTlv;
};

/*
 * Prototypes
 *
 */
enum eQCWWANError UpkQmiCbkNasDDTMSettingsInd(
    BYTE       *pMdmResp,
    struct QmiCbkNasDDTMSettingsInd *pApiResp );

#endif /* __NAS_DDTM_SETTINGS_IND_H__ */
