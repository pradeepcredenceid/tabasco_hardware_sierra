/*
 * \ingroup nas
 *
 * \file   qaNasSLQSSwiGetHDRPersonality.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaNasSLQSSwiGetHDRPersonality.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __NAS_SLQS_SWI_HDR_PERSONALITY_H__
#define __NAS_SLQS_SWI_HDR_PERSONALITY_H__

/*
 * \internal
 *
 * An enumeration of NAS_SLQS_SWI_GET_HDR_PERSONALITY_RESP response TLV IDs
 *
 */
enum eNAS_SLQS_SWI_GET_HDR_PERSONALITY_RESP
{
    eTLV_CURRENT_PERS_INDEX     = 0x10,
    eTLV_PERS_PROT_SUBTYPE_LIST = 0x11,
};

/*
 * \internal
 *
 * This structure contains the GetHDRPersonality response parameters.
 *
 * \sa qaGobiApiNas.h for parameter descriptions
 *
 */
struct QmiNasSlqsSwiGetHDRPersonalityResp
{
    /*
     * \internal
     *
     * Every response message must have a results structure
     */
    struct qmTlvResult results;

    /*
     * \internal
     *
     * Response parameters
     */
    HDRPersonalityResp *pHDRPersonalityResp;
};

/*
 * \internal
 *
 * Prototypes
 */
extern enum eQCWWANError PkQmiNasSlqsSwiGetHDRPersonality (
    WORD *pMlength,
    BYTE *pParamField );

enum eQCWWANError UpkQmiNasSlqsSwiGetHDRPersonality(
    BYTE                                      *pMdmResp,
    struct QmiNasSlqsSwiGetHDRPersonalityResp *pApiResp);

#endif /* __NAS_SLQS_SWI_HDR_PERSONALITY_H__ */
