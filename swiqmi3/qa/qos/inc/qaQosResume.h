/*
 * \ingroup qos
 *
 * \file qaQosResume.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaQosResume.c
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __QOS_RESUME_H__
#define __QOS_RESUME_H__

#include "qaGobiApiQos.h"

/*
 * An enumeration of eQOS_RESUME_QOS_REQ request TLV IDs
 */
enum eQOS_RESUME_QOS_REQ
{
    eTLV_QOS_RESUME_QOS  = 0x01
};

/*
 * This structure contains the QoS Suspend response parameters.
 *
 * \sa qaGobiApiQos.h for parameter descriptions
 *
 */
struct QmiQosResumeResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/************
 * Prototypes
 **************/
extern enum eQCWWANError PkQmiQosResume (
    WORD    *pMlength,
    BYTE    *pParamField,
    swiQosIds   *pQosIds );

extern enum eQCWWANError UpkQmiQosResume (
    BYTE     *pMdmResp,
    struct   QmiQosResumeResp *pAipResp );

#endif /* __QOS_RESUME_H__ */

