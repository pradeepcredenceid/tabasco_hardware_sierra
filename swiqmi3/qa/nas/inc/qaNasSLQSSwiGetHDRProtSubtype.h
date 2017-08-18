/*
 * \ingroup nas
 *
 * \file   qaNasSLQSSwiGetHDRProtSubtype.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaNasSLQSSwiGetHDRProtSubtype.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __NAS_SLQS_SWI_HDR_PROT_SUBTYPE_H__
#define __NAS_SLQS_SWI_HDR_PROT_SUBTYPE_H__

/*
 * \internal
 *
 * An enumeration of NAS_SLQS_SWI_GET_HDR_PROT_SUBTYPE_RESP response TLV IDs
 *
 */
enum eNAS_SLQS_SWI_GET_HDR_PROT_SUBTYPE_RESP
{
    eTLV_PROTO_CURRENT_PERS_INDEX     = 0x10,
    eTLV_PROTO_PERS_PROT_SUBTYPE_LIST = 0x11,
    eTLV_PROTO_APP_SUBTYPES           = 0x12,
};

/*
 * \internal
 *
 * This structure contains the GetHDRPersonality response parameters.
 *
 * \sa qaGobiApiNas.h for parameter descriptions
 *
 */
struct QmiNasSlqsSwiGetHDRProtSubtypeResp
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
    HDRProtSubtypResp *pHDRProtSubtypResp;
};

/*
 * \internal
 *
 * Prototypes
 */
extern enum eQCWWANError PkQmiNasSlqsSwiGetHDRProtSubtype (
    WORD *pMlength,
    BYTE *pParamField );

enum eQCWWANError UpkQmiNasSlqsSwiGetHDRProtSubtype(
    BYTE                                      *pMdmResp,
    struct QmiNasSlqsSwiGetHDRProtSubtypeResp *pApiResp);

#endif /* __NAS_SLQS_SWI_HDR_PROT_SUBTYPE_H__ */
