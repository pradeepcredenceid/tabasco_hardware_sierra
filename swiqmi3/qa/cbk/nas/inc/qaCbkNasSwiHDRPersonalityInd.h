/*
 * \ingroup cbk
 *
 * \file    qaCbkNasSwiHDRPersonalityInd.h
 *
 * \brief   This file contains definitions, enumerations, structures and
 *          forward declarations for qaCbkNasSwiHDRPersonalityInd.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __NAS_SWI_HDR_PERSONALITY_IND_H__
#define __NAS_SWI_HDR_PERSONALITY_IND_H__
#include "qaGobiApiCbk.h"

/*
 * An enumeration of eQMI_NAS_SWI_HDR_PERSONALITY_IND
 * response TLV IDs
 *
 */
enum eQMI_NAS_SWI_HDR_PERSONALITY_IND
{
    eTLV_CURRENT_PERS_INDEX_IND     = 0x10,
    eTLV_PERS_PROT_SUBTYPE_LIST_IND = 0x11,
};

/*
 * Name:    QmiCbkHDRPersonalityInd
 *
 * Purpose: Structure used to store all QMI Notification parameters.
 *
 * Members: HDRPersonalityInd    - See \ref HDrPersonalityInd for more Info
 *
 * Note:    None
 */
struct QmiCbkNasSwiHDRPersonalityInd{
    HDRPersonalityInd *pHDRPersInd;
};

/*************
 * Prototypes
 **************/
enum eQCWWANError UpkQmiCbkNasSwiHDRPersonalityInd(
    BYTE                                 *pMdmResp,
    struct QmiCbkNasSwiHDRPersonalityInd *pApiResp );

#endif /* __NAS_SWI_HDR_PERSONALITY_IND_H__ */

