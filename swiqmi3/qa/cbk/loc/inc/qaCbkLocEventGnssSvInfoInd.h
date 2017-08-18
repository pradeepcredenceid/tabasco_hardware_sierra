/*
 * \ingroup cbk
 *
 * \file qaCbkLocEventGnssSvInfoInd.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaCbkLocEventGnssSvInfoInd.c
 *
 * Copyright: © 2015 Sierra Wireless, Inc. all rights reserved
 *
 */

#ifndef __LOC_EVENT_GNSS_SV_INFO_IND_H__
#define __LOC_EVENT_GNSS_SV_INFO_IND_H__

#include "qaGobiApiCbk.h"

#define CHECK() \
    if (eRCode!=eQCWWAN_ERR_NONE) return eRCode;

/*
 * An enumeration of QMI_LOC_EVENT_GNSS_SV_INFO_IND response TLV IDs
 *
 */
enum eQMI_LOC_EVENT_GNSS_SV_INFO
{
    eTLV_IND_ALTITUTE_ASSUMED = 0x01,
    eTLV_IND_SATELLITE_INFO  = 0x10
};

/*
 * Prototypes
 *
 */
enum eQCWWANError UpkQmiCbkLocEventGnssSvInfoInd(
        BYTE                              *pMdmResp,
        gnssSvInfoNotification       *pApiResp );

#endif /* __LOC_EVENT_GNSS_SV_INFO_IND_H__ */


