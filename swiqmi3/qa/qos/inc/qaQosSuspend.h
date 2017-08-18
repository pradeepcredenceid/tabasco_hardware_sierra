/*
 * \ingroup qos
 *
 * \file qaQosSuspend.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaQosSuspend.c
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __QOS_SUSPEND_H__
#define __QOS_SUSPEND_H__

#include "qaGobiApiQos.h"

/*
 * An enumeration of eQOS_SUSPEND_QOS_REQ request TLV IDs
 */
enum eQOS_SUSPEND_QOS_REQ
{
    eTLV_QOS_SUSPEND_QOS  = 0x01
};

/*
 * This structure contains the QoS Suspend response parameters.
 *
 * \sa qaGobiApiQos.h for parameter descriptions
 *
 */
struct QmiQosSuspendResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

/************
 * Prototypes
 **************/
extern enum eQCWWANError PkQmiQosSuspend (
    WORD    *pMlength,
    BYTE    *pParamField,
    swiQosIds   *pQosIds );

extern enum eQCWWANError UpkQmiQosSuspend (
    BYTE     *pMdmResp,
    struct   QmiQosSuspendResp *pAipResp );

#endif /* __QOS_SUSPEND_H__ */

