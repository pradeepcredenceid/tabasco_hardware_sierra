/*
 * \ingroup nas
 *
 * \file  qaNasSLQSSetDDTMPref.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaNasSLQSSetDDTMPref.c
 *
 * Copyright: © 2011-2014 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __NAS_SLQS_SET_DDTM_PREF_H__
#define __NAS_SLQS_SET_DDTM_PREF_H__

/* enum declarations */

/*
 *
 * An enumeration of eQMI_NAS_SET_DDTM_PREF request TLV IDs
 *
 */
enum eQMI_NAS_SET_DDTM_PREF_REQ
{
    eTLV_DDTM_PREF = 0x01
};

/*
 * This structure contains the SLQSSetDDTTMPref request parameters.
 */
struct QmiNasSLQSSetDDTMPrefReq
{
    struct DDTMPreference *pDDTMPref;
};

/*
 * This structure contains the SLQSSetDDTTMPref response parameters.
 */
struct QmiNasSLQSSetDDTMPrefResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/* Function Prototypes */
enum eQCWWANError PkQmiNasSLQSSetDDTMPref ( WORD *pMlength,
                                                BYTE *pParamField,
                                                struct DDTMPreference *pDDTMPref );

enum eQCWWANError UpkQmiNasSLQSSetDDTMPref( BYTE *pMdmResp,
                                                struct QmiNasSLQSSetDDTMPrefResp *pApiResp );

#endif /* __NAS_SLQS_SET_DDTM_PREF_H__ */
