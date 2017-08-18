/*
 * \ingroup qos
 *
 * \file qaQosGetNWStatus.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaQosGetNWStatus.c
 *
 * Copyright: © 2011-2013 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __DMS_GET_NW_STATUS_H__
#define __DMS_GET_NW_STATUS_H__

/*
 *
 * An enumeration of eQMI_QOS_GET_NW_STATUS response TLV IDs
 *
 */
enum eQMI_QOS_GET_NW_STATUS_RESP
{
    eTLV_NW_STATUS  = 0x01,
};

/*
 * This structure contains the Get Network Status response parameters.
 *
 * \sa qaGobiApiQos.h for parameter descriptions
 *
 */
struct QmiQosGetNWStatusResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* Response parameters */
    BYTE    *pStatus;
};

/************
 * Prototypes
 **************/
extern enum eQCWWANError PkQmiQosGetNWStatus (
    WORD    *pMlength,
    BYTE    *pParamField );

extern enum eQCWWANError UpkQmiQosGetNWStatus (
    BYTE     *pMdmResp,
    struct   QmiQosGetNWStatusResp *pAipResp );

#endif /* __DMS_GET_NW_STATUS_H__ */

