/*
 * \ingroup nas
 *
 * \file   qaNasSLQSSwiPSDetach.h
 *
 * \brief  This file contains definitions, enumerations, structures and
 *         forward declarations for qaNasSLQSSwiPSDetach.c
 *
 * Copyright: © 2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __NAS_SLQS_SWI_PS_DETACH_H__
#define __NAS_SLQS_SWI_PS_DETACH_H__

/*
 * \internal
 *
 * An enumeration of NAS_SLQS_SWI_PS_DETACH_REQ request TLV IDs
 *
 */
enum eNAS_SLQS_SWI_PS_DETACH_REQ
{
    eTLV_PS_DETACH_ACTION = 0x10,
};

/*
 * This structure contains the SLQSSwiPSDetach request parameters.
 *
 * \sa qaGobiApiNas.h for parameter descriptions
 *
 */
struct QmiNasSlqsSwiPSDetachReq
{
    PSDetachReq *pPSDetachReq;
};

/*
 * \internal
 *
 * This structure contains the SLQSSwiPSDetach response parameters.
 *
 * \sa qaGobiApiNas.h for parameter descriptions
 *
 */
struct QmiNasSlqsSwiPSDetachResp
{
    /*
     * \internal
     *
     * Every response message must have a results structure
     */
    struct qmTlvResult results;

};

/*
 * \internal
 *
 * Prototypes
 */
extern enum eQCWWANError PkQmiNasSlqsSwiPSDetach (
    WORD        *pMlength,
    BYTE        *pParamField,
    PSDetachReq *pPSDetachReq );

enum eQCWWANError UpkQmiNasSlqsSwiPSDetach(
    BYTE                             *pMdmResp,
    struct QmiNasSlqsSwiPSDetachResp *pApiResp );

#endif /* __NAS_SLQS_SWI_PS_DETACH_H__ */
